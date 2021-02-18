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
/**
 * @brief ShellCmdConfig List of all commands to insert
 * @note n/a
 * @author grbba
 */

#include <nlohmann/json.hpp>

const std::string csMenuItems = R"(
{
    "Commands": [
      {
        "name": "dosomething",
        "params": [
          {
            "type": "string",
            "desc": "string parameter text 1",
            "mandatory": 1
          },
          {
            "type": "string",
            "desc": "string parameter text 2",
            "mandatory": 1
          },
          {
            "type":"integer",
            "desc": "int parameter text 3",
            "mandatory": 0
          }
        ],
        "help": "Help text decribing the menu item"
      },
      {
        "name": "dosomethingelse",
        "params": [
          {
            "type": "string",
            "desc": "string parameter text 1",
            "mandatory": 1
          },
          {
            "type": "string",
            "desc": "string parameter text 2",
            "mandatory": 1
          },
          {
            "type":"integer",
            "desc": "int parameter text 3",
            "mandatory": 0,
          }
        ],
        "help": "Help text decribing the menu item"
      }
      ]
  }
)";