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

/**
 * @class DccPath 
 * Holds all the path information for a layout and allows to get information about the paths for the layout 
 * controller ( loading , setting etc of paths in the command station)
 * @note The layout controller is work in progress
 * @author grbba
 */

#ifndef DccPath_h
#define DccPath_h

#include "DccGraph.hpp"
#include "DccModel.hpp"
#include "DccVertex.hpp"

typedef std::vector<int32_t> DccPath_t;
typedef std::vector<DccPath_t> DccNodePaths_t;
typedef std::map<Node_t, DccNodePaths_t> DccPathMap_t;

class DccPath {
private:

  DccNodePaths_t paths;     // contains all paths found by either findAllPaths or
                            // DFS for one run of any find... function
  DccPathMap_t  allPaths;   // a map of all paths found in a graph <encoded nodeid,
                            // vector of all the paths between Rail segments

  bool isCalculated = false;// true if tthe allPaths Map has the calculated paths

public:
  // Tell the Path object that is path vaiables have been filled
  void setCalculatedFlag(bool state) {
    isCalculated = state;
  }
  // retrun a unique ptr to the calculated paths if the paths have been calculated nullptr otherwise
  DccPathMap_t *getAllPaths();
  DccNodePaths_t *getNodePaths();
  
  // get all the possible paths for a given node ( un encoded encoded !)
  DccNodePaths_t *getPathsByNode(int module, Node_t nodeid);
  // get all the possible paths for a given node ( encoded !)
  DccNodePaths_t *getPathsByNode(Node_t nodeid);

  int getNumberOfPaths() {
      int _np = 0;
      for ( auto p: allPaths ) {
        _np = _np + p.second.size();
      }
      return _np;
  }

  void printDccPath( DccPath_t p ){
    std::cout << "[ ";
    for (auto n: p ) {
      std::cout << " " << n << " ";
    }
    std::cout << "]\n";
  }

  void printDccNodePaths( DccNodePaths_t ps){
    for(auto p : ps) {
      printDccPath(p);
    }
  }

  void printPathsByNode(Node_t nodeid);
  void printAllPaths();

  DccPath() = default;
  ~DccPath() = default;
};

#endif