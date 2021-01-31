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

#ifndef DccPathFinder_h
#define DccPathFinder_h


#include "DccGraph.hpp"
#include "DccModel.hpp"
#include "DccVertex.hpp"

typedef std::vector<int32_t> DccPath_t;
typedef std::vector<DccPath_t> DccNodePaths_t;

class DccPathFinder {
private:
  DccGraph *g;
  int32_t _start;
  int32_t _end;

  DccNodePaths_t paths; // contains all paths found by either findAllPaths or
                        // DFS for one run of any find... function
  std::map<Node_t, DccNodePaths_t>
      allPaths; // a map of all paths found in a graph <encoded nodeid, vector
                // of all the paths between Rail segments

  void DFS(DccPath_t path, DccVertexPtr_t start);
  void DFS(DccPath_t path, DccVertexPtr_t start, int32_t end);
  void findAllPaths(int32_t start,
                    int32_t end); // find all paths between two nodes
  void
  findAllRail2RailPaths(); // find all paths between any two rail segment nodes
  void findAllPaths(int32_t nodeid); // Expolore all paths from a given node

public:
  // to be called to setup all the paths for the layout
  void init() {
    if (g->isBuild()) {
      findAllRail2RailPaths();
    } else {
      ERR("Layout Graph has not been build; Can't continue");
    }
  }

  // get all the possible paths for a given node ( un encoded encoded !)
  DccNodePaths_t *getPathsByNode(int module, Node_t nodeid) {
    auto it = allPaths.find(DccVertex::cantorEncode(module, nodeid));
    if (it == allPaths.end()) {
      return nullptr;
    } else {
      return &it->second;
    }
  }
  // get all the possible paths for a given node ( encoded !)
  DccNodePaths_t *getPathsByNode(Node_t nodeid) {
    auto it = allPaths.find(nodeid);
    if (it == allPaths.end()) {
      return nullptr;
    } else {
      return &it->second;
    }
  }

  void printPathsByNode(Node_t nodeid);
  void printAllPaths();

  DccPathFinder(DccGraph *graph);
  ~DccPathFinder() = default;
};

#endif