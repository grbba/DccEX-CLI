
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

#ifndef DccTCP_h
#define DccTCP_h

#include "AsyncTCP.hpp"
#include "CliReciever.hpp"

enum recvState
{
  _Text,         // inital state, print everything as things arrive
  _OpenDcc,      // when '<' is seen, write to buffer
  _CloseDcc,     // if OpenDcc and > is next
  _OpenDiag,     // when we are in _OpenDcc and get a '*'
  _PreCloseDiag, // when _OpenDiag and * is seen if the next is not a > then state = _OpenDiag
  _CloseDiag,    // when _PreCloseDiag and > is seen
};

class DccTCP {

private:
  CallbackAsyncTCP  server;           
  std::string       ipAddress;            // IP Address 1.2.3.4
  std::string       port;                 // port number; default is 2560 for the command station
  bool              open = false;


  CliReciever recv;                       // reciever for the TCP Connection

  static std::stringstream  csMesg;       // commandstation message e.g. reslut of status, reda etc i;e. <> -> magenta
  static std::stringstream  dMesg;        // comandstation diag message i.e. <* *> tagged -> yellowish 
 
  static void recieve(const char *data, unsigned int len); // callback for reading 
  static recvState nState(recvState s, char c);            // state machine for reading incomming message flow 
  
  bool openConnection();                                   // open the connection / setting the callback for reception

public:
  bool openConnection(std::string ip, std::string port);   // open the connection  
  void closeConnection();                                  // close the connection                                    
  void write(const std::string *cmd); 

  void setIpAddress(std::string a) { ipAddress = a; }
  void setPort(std::string p) { port = p; }
  
  std::string getIpAddress() { return ipAddress; }
  std::string getPort() { return port; }
  CallbackAsyncTCP *getServer() { return &server; }

  bool isOpen() { return open; };

  DccTCP() = default;
  ~DccTCP() = default;
  DccTCP(std::string a, std::string p) : ipAddress(a), port(p) {};
};

#endif


