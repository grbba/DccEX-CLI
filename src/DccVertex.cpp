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
#include "DccVertex.hpp"
#include "Diag.hpp"

// pairing functions
int DccVertex::cantorEncode(int a, int b) {
    return (((a + b) * (a + b + 1)) / 2) + b;
}

void DccVertex::cantorDecode(int c, int *a, int *b) {

    int w = floor((sqrt(8*c + 1) - 1) / 2);
    int t = (w*(w+1))/2;

    *b = c - t;
    *a = w - *b;
}

DccVertex::DccVertex(int64_t g, int16_t m, int32_t d, Node_t n, TrackElement_t t) {
    module = m;
    dccid = d;
    nodeid = cantorEncode(m,n); // ( to be the encoded nodeid !! )
    trackelement = t;
    gid = g;

   // figure out which trackelement type we got
    if (isBumper(t))
    {
        teType = BUMPER;
    }
    if (isTurnout(t))
    {
        teType = TURNOUT; 
    }
    if (isSegement(t))
    {
        teType = RAIL; 

    }
    if (isCrossing(t))
    {
        teType = CROSSING; 
    }
    if (isJunction(t))
    {
        teType = JUNCTION; 
    }
}

DccVertexPtr_t DccDoubleVertex::findSibling(DccVertexPtr_t v)
{
    if (siblings.size() != 2)
    {
        WARN("No sibling for");
        v.get()->printVertex();
        return nullptr;
    }
    if (v == siblings.at(0))
    {
        return siblings.at(1);
    }
    else
    {
        return siblings.at(0);
    }
}

void DccDoubleVertex::printDoubleVertex()
{
    INFO( "(%d,%d) Siblings((%s  \t[%d],%d), <-> (%s  \t[%d],%d))",
        siblings.at(0).get()->getModule(),
        siblings.at(1).get()->getModule(),
        siblings.at(0).get()->getTrackElementType(),
        siblings.at(0).get()->getDccid(),
        siblings.at(0).get()->getNodeid(),
        siblings.at(1).get()->getTrackElementType(),
        siblings.at(1).get()->getDccid(),
        siblings.at(1).get()->getNodeid());
}

auto DccVertex::getTrackElementType() -> std::string
{
    std::string type;
    switch (teType)
    {
    case BUMPER:
    {
        type = "Bumper  ";
        break;
    }
    case TURNOUT:
    {
        type = "Turnout ";
        break;
    }
    case RAIL:
    {
        type = "Rail    ";
        break;
    }
    case CROSSING:
    {
        type = "Crossing";
        break;
    }
    case JUNCTION:
    {
        type = "Junction";
        break;
    }
    }
    return type;
}

void DccVertex::printConnections()
{
    for (auto v : connections)
    {
        INFO("      Connected to: V(%d,%d)", v.get()->dccid, v.get()->nodeid);
    }
}

void DccVertex::printVertex()
{
    INFO("Module[%d] : V( %s(%d), %d ) with %d connections", module, getTrackElementType(), dccid, nodeid, connections.size());
    /*
    if (connections.size() > 0)
    {
        printConnections();
        // std::cout << std::endl;
    }
*/
}
