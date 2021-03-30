
/*
 * © 2021 Gregor Baues. All rights reserved.
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

#ifndef CLIReciever_h
#define CLIReciever_h

#include <iostream>
#include <string>
#include <sstream>


enum recvState
{
  _Text,         // inital state, print everything as things arrive
  _OpenDcc,      // when '<' is seen, write to buffer
  _CloseDcc,     // if OpenDcc and > is next
  _OpenDiag,     // when we are in _OpenDcc and get a '*'
  _PreCloseDiag, // when _OpenDiag and * is seen if the next is not a > then state = _OpenDiag
  _CloseDiag,    // when _PreCloseDiag and > is seen
};

class CliReciever
{
private:
    
    std::stringstream  csMesg;                           // commandstation message e.g. result of status, read etc i.e. < > -> magenta
    std::stringstream  dMesg;                            // comandstation diag message i.e. <* *> tagged -> INFO msg from spdlog ( change ev. to custom )
    recvState nState(recvState s, char c);               // state machine for reading / printing incomming message flow 

    recvState state = _Text;                              // current state for the nState()

public:

    void recieve(const char *data, unsigned int len);    // callback for reading 
    CliReciever() = default;
    ~CliReciever() = default;
};

#endif