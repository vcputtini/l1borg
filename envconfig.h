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

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "globals.h"

#include <iostream>
#include <sstream>
#include <iterator>
#include <utility>  // make_pair
#include <algorithm>
#include <map>
#include <tuple>
#include <set>
#include <initializer_list>
#include <tinyxml2.h>
#include <boost/algorithm/string.hpp>


#include "command.h"

using namespace tinyxml2;

namespace EnvironConfig {
  enum class eEnvVars
  {
                        BorgRepoDir,
                        BorgConfigDir,
                        BorgSecurityDir,
                        BorgKeysDir,
                        BorgCacheDir,
                        BorgLogDir,
                        BorgTmpDir
  };

  enum class eRepoParms
  {
                        RepoName,
                        BkpName,
                        BkpSource,
                        BkpExcludes
  };

  constexpr std::initializer_list<eEnvVars> EnvVars =
  {
                        eEnvVars::BorgRepoDir,
                        eEnvVars::BorgConfigDir,
                        eEnvVars::BorgSecurityDir,
                        eEnvVars::BorgKeysDir,
                        eEnvVars::BorgCacheDir,
                        eEnvVars::BorgLogDir,
                        eEnvVars::BorgTmpDir
  };

  // Singleton based in
  //     http://www.vishalchovatiya.com/singleton-design-pattern-in-modern-cpp/
  class EnvConfig
  {
      XMLDocument doc;
      XMLElement *root;

      constexpr static std::string_view configLocals[] = {
          {"/etc/l1borg.xml"},
          {"/etc/borg/l1borg.xml"},
          {"/etc/l1borg/l1borg.xml"},
          {"./l1borg.xml"}
      };

      typedef std::tuple<std::string, std::string> tData;
      std::map<EnvironConfig::eEnvVars, tData> mEnv;
      std::map<eEnvVars, std::string> mVars;

      bool bError;
      const char* errorText;
      std::string configXMLFName;

      EnvConfig();

  public:
      EnvConfig(EnvConfig const &) = delete;
      EnvConfig &operator=(EnvConfig const &) = delete;

      static EnvConfig &get() {
          static EnvConfig var;
          return var;
      }

      void InitEnv();

      std::string Name(EnvironConfig::eEnvVars);
      std::string Data(EnvironConfig::eEnvVars);
      std::string Combo(EnvironConfig::eEnvVars);

      inline static std::vector<std::string> GetVars();
      inline static std::vector<std::pair<std::string, std::string>> GetVarsPair();

      const std::string ErrorText()
      {
        return std::string((errorText == nullptr) ? std::string() : errorText);
      }

      const bool findCfgFile(std::string& folder)
      {
          Command cmd;
          for(std::string_view s : configLocals) {
              if(cmd.exists(s)) {
                  folder = s;
                  return true;
              }
          }
          return false;
      };
  };

  class EnvRepos
  {
      XMLDocument doc;
      XMLElement *root;

      typedef std::tuple<std::string, std::string, std::string> tRepoIdx;
      typedef std::tuple<std::string, std::string, std::string, std::string, std::string> tRepoParms;
      std::map<tRepoIdx, tRepoParms> mRepos;
      std::set<std::string> repoSet;  // nome dos repos ja criados

      bool bError;
      bool eError;
      const char* errorText;
      std::string configXMLFName;


      MiniLog mlog;

      EnvRepos();

      void SetCreated(std::string repo);

  public:
      EnvRepos(EnvRepos const &) = delete;
      EnvRepos &operator=(EnvRepos const &) = delete;

      static EnvRepos &get() {
          static EnvRepos var;
          return var;
      }

      std::string Name(EnvironConfig::eEnvVars);
      std::string Data(EnvironConfig::eEnvVars);
      std::string Combo(EnvironConfig::eEnvVars);

      const std::string ErrorText()
      {
        return std::string((errorText == nullptr) ? std::string() : errorText);
      }

      void InitRepo();
      void MakeBackup();
      void ExecCmd(std::string cmd);
      void ExecAlias(std::string parm, std::string cmd, std::string cmd1=std::string());

      // how to get data from maps
      /**
      void test_only()
      {
        for(auto &m : mRepos) {
          std::cout <<  std::get<0>(m.first) << std::get<1>(m.first) << std::get<2>(m.first) << std::endl;
          std::cout <<  std::get<0>(m.second) << std::get<1>(m.second) <<
          std::get<2>(m.second) << std::get<3>(m.second) << std::endl;
        }
      }
      */
  };



}; // namespace

#endif
