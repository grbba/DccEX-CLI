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

#ifndef DccGraph_h
#define DccGraph_h

#include "DccModel.hpp"
#include "DccVertex.hpp"
#include "Diag.hpp"

class DccGraph {
private:
  int mn = 0; // number of modules; should be the same as tpn at the end of the
              // build process
  int tpn =
      1; // number of trackplans used by modules; there may be more trackplans
         // in the Json file but only the ones actually used will be counted
  int tsn = 1;    // number of tracksections across all modules/trackplans
  int ten = 0;    // number of trackelements across all modules/trackplans
  int length = 0; // total length calculated across all trackplans
  int tn = 0;     // current turnout number ( module scoped )
  int sn = 0;     // current segment number ( module scoped )
  int bn = 0;     // current bumper number ( module scoped )
  int cn = 0;     // current crossing number ( module scoped ) no disticntion
                  // between crossing, single and double slips

  DccModel::DccExLayout *layout; // entry to the layout model from the json file
  
  std::vector<DccModel::Trackplan>
      tps; // vector of all 'used' trackplans by the layout
  
  std::map<int32_t, std::vector<DccVertexPtr_t>>
      tev;                // map of all vertices per trackelement
  
  bool _isBuild = false;  // will be set to true once the graph has been build
  
  DccVertexGraph_t graph; // map<nodeid,Vertex> of the complete simple vertex
                          // graph with connections for each track element
  DccDoubleVertexGraph_t
      dvGraph; // map<<Gid1,Gid2>< vertex vector> of the complete double vertex
               // graph with connections for each double vertex keys iare the
               // Gids of the two vertices in one dcc double vertex
  int64_t ngid = 0;                   // counter of simple vertices in the graph
  std::map<std::string, int> modules; // map of all modules with key being their
                                      // names as we need them for the junctions
  std::set<Node_t> nodes; // cointains all encoded nodeids from the oevrall
                          // graph i.e. for all modules. BY dedofing it we find
                          // the module and the nodeid wihin the module

  auto checkModule(DccModel::Module *module) -> int;
  void buildGraph(int mn);

  void buildBumperVertex(int mn, int ten, DccModel::Trackelement *te);
  void buildTurnoutVertex(int mn, int ten, DccModel::Trackelement *te);
  void buildRailVertex(int mn, int ten, DccModel::Trackelement *te);
  void buildCrossingVertex(int mn, int ten, DccModel::Trackelement *te);
  DccVertexPtr_t createVertex(int16_t module, int32_t dccid, int32_t nodeid,
                              TrackElement_t te);

  int64_t obtainNGid() {
    int64_t current = ngid;
    ngid++;
    return current;
  }

public:
  DccVertexGraph_t getGraph() { return graph; }
  DccDoubleVertexGraph_t *getDvGraph() { return &dvGraph; }

  /**
   * @brief takes a layout and builds all the required graph structures
   */
  void build(DccModel::DccExLayout *layout);
  bool isBuild() { return _isBuild; }

  /**
   * @brief prints out some info about the graph build from the layout
   */
  void printGraph();
  void printInfo();
  void printTrackElements();

  DccGraph() = default;
  ~DccGraph() = default;
};

#endif