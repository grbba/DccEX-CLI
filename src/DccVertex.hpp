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
 * See the GNU General Public License for more details <https://www.gnu.org/licenses/>
 */

#ifndef DccVertex_h
#define DccVertex_h

#include <string>
#include <vector>
#include <algorithm>
#include <variant>
#include <memory>

#include "../include/formatter.h"

#include "DccModel.hpp"
#include "Diag.hpp"

/**
 * @brief defines for finding out the type of track element Track elements
 * 
 */
#define isTurnout(x) std::get_if<std::shared_ptr<dccexlayout::Turnout>>(&x) != nullptr
#define getTurnout(x) std::get_if<std::shared_ptr<dccexlayout::Turnout>>(&x)

#define isBumper(x) std::get_if<std::shared_ptr<dccexlayout::Bumper>>(&x) != nullptr
#define getBumper(x) std::get_if<std::shared_ptr<dccexlayout::Bumper>>(&x)

#define isSegement(x) std::get_if<std::shared_ptr<dccexlayout::Rail>>(&x) != nullptr
#define getSegment(x) std::get_if<std::shared_ptr<dccexlayout::Rail>>(&x)

#define isCrossing(x) std::get_if<std::shared_ptr<dccexlayout::Crossing>>(&x) != nullptr
#define getCrossing(x) std::get_if<std::shared_ptr<dccexlayout::Crossing>>(&x)

#define isJunction(x) std::get_if<std::shared_ptr<dccexlayout::Junction>>(&x) != nullptr
#define getJunction(x) std::get_if<std::shared_ptr<dccexlayout::Junction>>(&x)

class DccDoubleVertex;
class DccVertex;

enum Telement
{
    BUMPER,
    RAIL,
    TURNOUT,
    CROSSING,
    JUNCTION        // connection between modules
};

enum GraphMarker
{
    NOK,
    OK,
    UNKNOWN
};

// to be completed with crossing, singe/double slips
typedef std::variant<std::shared_ptr<dccexlayout::Bumper>,
                     std::shared_ptr<dccexlayout::Rail>,
                     std::shared_ptr<dccexlayout::Turnout>,
                     std::shared_ptr<dccexlayout::Crossing>,
                     std::shared_ptr<dccexlayout::Junction>>
    TrackElement_t;

typedef std::shared_ptr<DccVertex> DccVertexPtr_t;
typedef std::shared_ptr<DccDoubleVertex> DccDoubleVertexPtr_t;
typedef std::map<int32_t, DccVertexPtr_t> DccVertexGraph_t;
typedef std::map<std::pair<int64_t, int64_t>, DccDoubleVertexPtr_t> DccDoubleVertexGraph_t;
typedef int32_t Node_t;

class DccVertex
{
private:
    int64_t  gid;     // graphid unique number of each vertex
    int16_t  module;  // module on which the node is located
    int32_t  dccid;   // unique id per trackelement ( but not for the vertices of a trackelement )
    Node_t   nodeid;  // Attn the node id is not unique as the same id will be used in the layout description multiple times to represent connectoins
    Telement teType; // enum indicating the Track element type
    TrackElement_t trackelement;

    std::vector<DccVertexPtr_t> connections; // all the other vertices to which this one is connected
    DccDoubleVertexPtr_t dv;                 // The double vertex wich this vertex is part of

public:
    GraphMarker mark = NOK;
    // Getter/Setter
    void setTrackelement(TrackElement_t value)
    {
        trackelement = value;
    }
    TrackElement_t getTrackelement()
    {
        return trackelement;
    }

    void setTeType(Telement value)
    {
        teType = value;
    }
    Telement getTeType()
    {
        return teType;
    }

    int16_t getModule()
    {
        return module;
    }
    void setModule(int16_t value)
    {
        module = value;
    }

    void setGid(int64_t value)
    {
        gid = value;
    };
    int64_t getGid()
    {
        return gid;
    };

    void setDccid(int32_t value)
    {
        dccid = value;
    };
    int32_t getDccid()
    {
        return dccid;
    }

    int32_t getNodeid()
    {
        return nodeid;
    }
    void setNodeid(int32_t value)
    {
        nodeid = value;
    };

    void addConnection(DccVertexPtr_t v)
    {
        connections.push_back(v);
    }
    std::vector<DccVertexPtr_t> getConnections()
    {
        return connections;
    }

    void setDv(DccDoubleVertexPtr_t value)
    {
        dv = value;
    }
    DccDoubleVertexPtr_t getDv()
    {
        return dv;
    }

    static int cantorEncode(int a, int b);
    static void cantorDecode(int c, int *a, int *b);

    // Function Methods
    /**
     * @brief Determines the TrackElement type from the trackelement variant attribute; Only for printing purposes
     * 
     * @return std::string Name of the track element
     */
    auto getTrackElementType() -> std::string;
    void printVertex();
    void printConnections();

    // Constructor/Destructor
    DccVertex() = default;
    DccVertex(int64_t gid, int16_t module, int32_t dccid, Node_t nodeid, TrackElement_t te);
    ~DccVertex() = default;
};

class DccDoubleVertex
{
private:
    std::vector<DccVertexPtr_t> siblings;
    std::vector<DccDoubleVertexPtr_t> connections; // all double vertices to which the vertex v1 and v2 are connected

public:
    GraphMarker mark;
    std::vector<DccVertexPtr_t> *getSiblings()
    {
        return &siblings;
    };
    DccVertexPtr_t findSibling(DccVertexPtr_t v);

    std::vector<DccDoubleVertexPtr_t> getConnections()
    {
        return connections;
    }

    void printDoubleVertex();

    DccDoubleVertex() = default;
    ~DccDoubleVertex() = default;
};

#endif