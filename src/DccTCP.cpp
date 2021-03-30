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

#include <iostream>
#include <vector>
#include <fmt/core.h>
#include <fmt/color.h>
#include <fmt/ostream.h>

#include "DccTCP.hpp"
#include "Diag.hpp"

std::stringstream  DccTCP::csMesg;       // commandstation message e.g. reslut of status, reda etc i;e. <> -> magenta
std::stringstream  DccTCP:: dMesg; 

recvState state = _Text;

recvState DccTCP::nState(recvState s, char c)
{
  switch (c)
  {
    case '\n': {
      // ignore \n
      return s;
    }
    case '<':
    {
      if( s == _OpenDiag ) { // the <> is part of a diag
        DccTCP::dMesg << c;
        return s;
      }
      DccTCP::csMesg << c;              // store the char in the csmesg
      return _OpenDcc;
    }
    case '*':
    {
      if (s == _OpenDcc) {
        csMesg.str(""); // clear the stream
        // DccTCP::dMesg << c;
        return _OpenDiag;
      }
      if (s == _OpenDiag) {
        // DccTCP::dMesg << c;
        return _PreCloseDiag;
      }
    }
    case '>':
    {
      if (s == _OpenDcc) {
          DccTCP::csMesg << c;
          fmt::print(fg(fmt::color::magenta), "{}\n", csMesg.str());
          // fmt::print("print dcc message {}\n", csMesg.str());
          csMesg.str(""); // clear the stream
          // csMesg.clear();
          return _CloseDcc; // print the dcc message 
      }
      if (s == _PreCloseDiag) {
          // DccTCP::dMesg << c;
          INFO("{}", dMesg.str());
          dMesg.str(""); // clear the stream
          // fmt::print("print diag message\n"); 
          return _CloseDiag; // print the diag message 
      } 
    }
    default:
    {
      if (s == _OpenDcc) { DccTCP::csMesg << c; return _OpenDcc; };
      if (s == _OpenDiag) { DccTCP::dMesg << c; return _OpenDiag; };
      fmt::print(fg(fmt::color::magenta), "{}", c); // print the char
      return _Text;
    }
  }
  ERR("Can't understand incomming message");
  return _Text;
}

/**
 * @brief  Reciever callback
 *
 * @param data
 * @param len
 */
void DccTCP::recieve(const char *data, unsigned int len)
{
  std::vector<char> v(data, data + len);

  for (unsigned int i = 0; i < v.size(); i++)
  {
      auto s = nState(state, v[i]);
      //  fmt::print("{} {} {} {}\n", state, v[i], s, "action" );
      state = s;
      std::cout.flush(); // Flush screen buffer
  }
}
  /**
 * @brief
 *
 */
  bool DccTCP::openConnection()
  {
    // That means that ipAddress and port are properly initalized!!
    // INFO("openConnection ...");
    server.setCallback(recieve);
    server.open(ipAddress, port);
    return server.isOpen();
  };

  bool DccTCP::openConnection(std::string ip, std::string p)
  {
    // if already open and same ip --> warning that the ip is already connected --> did you mean to open another ip?
    if (open)
    {
      if (ip.compare(ipAddress) == 0)
      {
        WARN("Already connected to {}. Did you mean to open a different address?", ipAddress);
        return DCC_SUCCESS;
      }
      else
      {
        // if open and open request to different ip --> warning changing ip --> close old ip first before moving on
        WARN("switching connection {} to {}", ipAddress, ip);
        server.close();
      }
    }

    ipAddress = ip;
    port = p;

    if (openConnection())
    {
      open = true;
    }
    else
    {
      ERR("Could not open ethernet connection.\n");
      return DCC_FAILURE;
    }
    return DCC_SUCCESS;
  }

  /**
 * @brief
 *
 */
  void DccTCP::closeConnection()
  {
    server.clearCallback();
    server.close();
  };

  void DccTCP::write(const std::string *cmd)
  {
    server.write(cmd->c_str(), cmd->size());
  };