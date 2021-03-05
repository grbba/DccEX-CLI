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

#include "DccPath.hpp"
#include "DccGraph.hpp"
// #include "DccPathFinder.hpp"
#include "DccVertex.hpp"
#include "Diag.hpp"

// do not create a share_ptr at each call !!! always reply with the same of this object
DccPathMap_t *DccPath::getAllPaths() {
    return &allPaths; 
}

DccNodePaths_t *DccPath::getNodePaths() {
    return &paths;
}

// get all the possible paths for a given node ( un encoded encoded !)
DccNodePaths_t *DccPath::getPathsByNode(int module, Node_t nodeid) {
  auto it = allPaths.find(DccVertex::cantorEncode(module, nodeid));
  if (it == allPaths.end()) {
    return nullptr;
  } else {
    return &it->second;
  }
}
// get all the possible paths for a given node ( encoded !)
DccNodePaths_t *DccPath::getPathsByNode(Node_t nodeid) {
  auto it = allPaths.find(nodeid);
  if (it == allPaths.end()) {
    return nullptr;
  } else {
    return &it->second;
  }
}

void DccPath::printPathsByNode(Node_t nodeid) {
  int n, m;

  DccVertex::cantorDecode(nodeid, &m, &n);
  auto p = getPathsByNode(nodeid);
  
  // report all paths found
  CLI_INFO;
  fmt::print("[{}] direct paths are available from module [{}] node [{}]\n", p->size(),
       m, n);

  Diag::setPrintln(false);
  for (auto pa : *p) {
    int _pn, _pm;
    CLI_INFO;
    fmt::print("[");
    for (auto n : pa) {
      DccVertex::cantorDecode(n, &_pm, &_pn);
      fmt::print(" {}.{} ", _pm, _pn);
    }
    fmt::print("]\n");
  }
  Diag::setPrintln(true);
}

void DccPath::printAllPaths() {
  CLI_INFO;
  fmt::print("Printing ALL paths available for this Layout\n");
  for (auto ap : allPaths) {
    printPathsByNode(ap.first);
  }
}