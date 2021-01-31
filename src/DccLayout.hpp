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

#ifndef DccLayout_h
#define DccLayout_h

#include "DccModel.hpp"
#include "DccGraph.hpp"


#include <nlohmann/json.hpp>
#include <nlohmann/json-schema.hpp>


using nlohmann::json;
using nlohmann::json_schema::json_validator;

class DccLayout {
private:

  dccexlayout::DccExLayout layout;   // the model structure as parsed from the layout file; filled by readLayout
  json_validator validator; 
  json schema;
  DccGraph g; // g will hold the full graph with all vertices ( with their
              // connections i.e. is build in adjacency list form / double
              // vertices etc ...

public:

  auto readLayout() -> int;
  auto build() -> int;

  DccLayout() = default;
  ~DccLayout() = default;
};

#endif