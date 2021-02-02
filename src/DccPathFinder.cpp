/*
 * © 2020 Gregor Baues. All rights reserved.
 *
 * This is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * See the GNU General Public License for more details
 * <https://www.gnu.org/licenses/>
 */

#include "DccPathFinder.hpp"

#include "DccPath.hpp"
#include "DccGraph.hpp"
#include "DccVertex.hpp"
#include "Diag.hpp"


void DccPathFinder::calculate() {
  //Diag::push();

  //Diag::setFileInfo(true);
  //Diag::setLogLevel(DiagLevel::LOGV_DEBUG);

  if (_graph->isBuild()) {
    findAllRail2RailPaths();
    isCalculated = true;
    _path->setCalculatedFlag(true);
  } else {
    isCalculated = false;
    _path->setCalculatedFlag(false);
    ERR("Layout Graph has not been build; Can't continue");
  }

  //Diag::pop();
  return;
}

/**
 * @brief Fetches the vretices to start from for the provided nodeid
 *
 * @param nodeid Nodeid to start from
 * @param start pointer to the vector in which to store the vertices
 * @param g map of all the vertices of the graph to look at
 * @return true if vertices have been found
 * @return false otherwise
 */
auto findStartVertex(int32_t nodeid, std::vector<DccVertexPtr_t> *start,
                     DccVertexGraph_t *g) -> bool{
  bool foundStart = false;
  for (auto vertex : *g) {
    if (vertex.second.get()->getNodeid() == nodeid) {
      switch (vertex.second.get()->getTeType()) {
      case BUMPER: // start from its sibling
      {
        DccVertexPtr_t pb =
            vertex.second.get()->getDv().get()->findSibling(vertex.second);
        if (pb == nullptr) {
          break;
        }
        start->push_back(pb);
        foundStart = true;
        break;
      }
      case JUNCTION:
      case CROSSING:
      case TURNOUT:
      case RAIL: // add both except if the sibling is a BUMPER
      {
        start->push_back(vertex.second);
        DccVertexPtr_t pb =
            vertex.second.get()->getDv().get()->findSibling(vertex.second);
        if (pb == nullptr) {
          break;
        }
        if (pb.get()->getTeType() != BUMPER) {
          start->push_back(pb);
        }
        foundStart = true;
      }
      }
      // no need to continue the loop
      if (foundStart) {
        break;
      }
    }
  }
  return foundStart;
}

/**
 * @brief Calculates all paths in the layout from any rail segment to any other
 * As we can/shall have trains only on rail segments these are they only
 * reasonable ones
 */
void DccPathFinder::findAllRail2RailPaths() {
  int n;
  int m;
  
  for (auto var : _graph->getGraph()) {
    if (var.second->getTeType() == RAIL) {
      var.second->cantorDecode(var.second->getNodeid(), &m, &n);
      DBG("Find paths for %d - (%d/%d)", var.second->getNodeid(), m, n);
      findAllPaths(var.second->getNodeid());
    }
  }

  // find all nodeid from rail segments
  // for each nodeid run the path finder and insert all the pointers of found
  // paths into a nxn matrix ( n be the modeids) and store in (x,y) the poitner
  // to the path allowing to go from x to y providing a second graph allwing to
  // determine ev reverse operatons to get from one segment to another where you
  // can not go directly e.g. in time saver 1 6 R 12 R 15 R 20
}

/**
 * @brief Public: Finds all paths from the start nodeid; the found paths will be
 * stored in paths
 *
 * @param nodeid nodeid of the start point
 */
void DccPathFinder::findAllPaths(int32_t nodeid) {
  std::vector<DccVertexPtr_t> start;
  auto paths = _path->getNodePaths();
  auto allPaths = _path->getAllPaths();

  int _sn, _sm;
  DBG("-> findAllPaths");
  DccVertex::cantorDecode(nodeid, &_sm, &_sn);

  _start = nodeid;
  _end = 0;

  auto v = _graph->getGraph();

  // reset the mark left from a previous run
  for (auto vertex : v) {
    vertex.second->mark = NOK;
  }

  DBG("start: %d end: %d", _start, _end);
  DBG("Paths %x  size %d", paths, paths->size());

  if (findStartVertex(nodeid, &start, &v)) {
    TRC("Calculating direct paths for module [%d] node [%d]", _sm, _sn);
    DBG("Number of start nodes %d", start.size());
    for (auto v : start) {
      DccPath_t p;
      DBG("v:%x p:%x sizeof p: %d", v, &p, p.size());
      DFS(p, v);
    }
  } else {
    ERR("DFS::No such nodeid [%d]", _sn);
    return;
  }

  allPaths->insert({nodeid, *paths});
  paths->clear();
}

/**
 * @brief Public: Finds all possible paths between two nodes. Start and end
 * nodes only from rail segments are allowed
 *
 * @param start nodeid for the starting point
 * @param end  nodeif for the end point
 */
void DccPathFinder::findAllPaths(int32_t s, int32_t e) {
  int _sm, _em;
  int _sn, _en;
  auto paths = _path->getNodePaths();
 
  DccVertex::cantorDecode(s, &_sm, &_sn);
  DccVertex::cantorDecode(e, &_em, &_en);

  std::vector<DccVertexPtr_t> start;

  _start = s;
  _end = e;

  auto v = _graph->getGraph();
  // reset the marks left from a previous run
  for (auto vertex : v) {
    vertex.second->mark = NOK;
  }

  if (findStartVertex(s, &start, &v)) {

    INFO("Calculating direct paths from node [%d] to node [%d]", _sn, _en);
    for (auto v : start) {
      DccPath_t p;
      DFS(p, v, _end);
    }
  } else {
    ERR("DFS::No such nodeid [%d]", _sn);
    return;
  }
  // report all paths found
  INFO("[%d] direct paths are available from node [%d] to node [%d]",
       paths->size(), _sn, _en);
  for (auto pa : *paths) {
    int _pn, _pm;

    for (auto n : pa) {
      DccVertex::cantorDecode(n, &_pm, &_pn);
      std::cout << _pn << " ";
    }
    std::cout << std::endl;
  }
  paths->clear();
}

/**
 * @brief Private: Finds all paths from start to end; Called by
 * findAllFGrahs(start , end)
 *
 * @param path
 * @param start Vertex pointer to the start node
 * @param end  nodeid of the end point (that this is a rail segment node was
 * checked before in findAllGraphs)
 */
void DccPathFinder::DFS(DccPath_t path, DccVertexPtr_t start, int32_t end) {
  auto paths = _path->getNodePaths();
  // auto allPaths = *_path->getAllPaths();
  std::vector<DccVertexPtr_t> toVisit;

  // we are done if there are no more connections
  // or if we find the inital start node again ( circle )
  // or when we reached the end node
  if (start.get()->getConnections().size() == 0 ||
      start.get()->getNodeid() == end ||
      (start.get()->getNodeid() == _start && start.get()->mark == OK)) {
    // add the last node and save the path in paths
    if (start.get()->getNodeid() == end || start.get()->getNodeid() == _start) {
      path.push_back(start.get()->getNodeid());
      paths->push_back(path);
    }
    return;
  }

  start.get()->mark = OK; // mark as visited
  path.push_back(start.get()->getNodeid());

  // Add the sibiling of the reachable vertex
  for (auto c : start.get()->getConnections()) {

    // for each connected vertex of start get the dv of that vertex and look for
    // its sibling
    DccVertexPtr_t add = c.get()->getDv().get()->findSibling(c);
    if (add != nullptr) {
      toVisit.push_back(add);
    }
  }

  for (auto v : toVisit) {
    DFS(path, v, end);
  }
}

/**
 * @brief Private: Recursivley finds all paths starting from a vertex and stores
 * each path in the collection of paths
 *
 * @param path Vector which contains all the nodes collected until a path is
 * found
 * @param start Pointer to the vertex fro which we shall start. This vertex will
 * be defined by the public function DFS
 */
void DccPathFinder::DFS(DccPath_t path, DccVertexPtr_t start) {
  auto paths = _path->getNodePaths();
  // auto allPaths = *_path->getAllPaths();
  std::vector<DccVertexPtr_t> toVisit;

  // we are done if there are no more connections
  // or if we find the inital start node again and this one has been visited
  // already ( circle )
  if (start.get()->getConnections().size() == 0 ||
      (start.get()->getNodeid() == _start && start.get()->mark == OK)) {
    // we are done and can push the graph into the paths vector
    path.push_back(start.get()->getNodeid());
    // _path->printDccPath(path);
    paths->push_back(path);
    return;
  }

  start.get()->mark = OK; // mark as visited
  path.push_back(start.get()->getNodeid());

  // Add the sibiling of the reachable vertex
  for (auto c : start.get()->getConnections()) {
    DccVertexPtr_t add = c.get()->getDv().get()->findSibling(c);
    if (add != nullptr) {
      toVisit.push_back(add);
    }
  }

  for (auto v : toVisit) {
    DFS(path, v);
  }
  // get all dccVertices adjacent to start
}

DccPathFinder::DccPathFinder(DccGraph *g, DccPath *p) {
  _graph = g;
  _path = p;
  _start = 0;
  _end = 0;
};