/***************************************************************************
 * Copyright (c) 2021                                                      *
 *      Volnei Cervi Puttini.  All rights reserved.                        *
 *      vcputtini@gmail.com
 *                                                                         *
 * Redistribution and use in source and binary forms, with or without      *
 * modification, are permitted provided that the following conditions      *
 * are met:                                                                *
 * 1. Redistributions of source code must retain the above copyright       *
 *    notice, this list of conditions and the following disclaimer.        *
 * 2. Redistributions in binary form must reproduce the above copyright    *
 *    notice, this list of conditions and the following disclaimer in the  *
 *    documentation and/or other materials provided with the distribution. *
 * 4. Neither the name of the Author     nor the names of its contributors *
 *    may be used to endorse or promote products derived from this software*
 *    without specific prior written permission.                           *
 *                                                                         *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND *
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE   *
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR      *
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS  *
 * BE LIABLEFOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR   *
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF    *
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS*
 * INTERRUPTION)                                                           *
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,     *
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING   *
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE      *
 * POSSIBILITY OFSUCH DAMAGE.                                              *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef COMMAND_H
#define COMMAND_H

#include "globals.h"

#include <vector>
#include <system_error>
#include <iostream>
#include <filesystem>    // C++17
#include <cstdio>
#include <ctime>
#include <boost/process.hpp>

namespace fs = std::filesystem;
namespace bp = boost::process;      // Boost::process

class Command
{
  std::string errorText;
  int errorCode;
  int bError;
  int eError;

public:
  Command();

  constexpr static std::string_view CmdBorg= "/usr/bin/borg";


  bool mkdir(const fs::path& p);
  bool rmdir(const fs::path& p);
  bool exists(const fs::path& p);

  // 1. Based on (Jan'21): https://www.boost.org/doc/libs/master/doc/html/boost_process/tutorial.html#boost_process.tutorial.env
  // 2. Based on (Jan'21): https://stackoverflow.com/questions/57732569/cant-get-segmentation-fault-exit-code-from-boost-child-process
  // Syncronous I/O
  static int processSyncIO(const std::vector<std::pair<std::string, std::string> > vEnv,
              const std::vector<std::string>& vParms, const bool logger=true);

  // 3. Based on (Mar'21): https://www.programiz.com/cpp-programming/library-function/ctime/ctime
  inline static char* currentDate() {
    std::time_t c_time;
    c_time = std::time(nullptr);
    return std::ctime(&c_time);
  };
};

#endif // COMMAND_H
