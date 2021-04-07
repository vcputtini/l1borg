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

#ifndef MINILOG_H
#define MINILOG_H

#include <string>
#include <sstream>
#include <fstream>
#include <ctime>

class MiniLog
{
    std::ofstream logfile;

    inline static tm *logDate() {
      std::time_t c_time;
      tm *c_tm;
      std::time(&c_time);
      c_tm = std::localtime(&c_time);
      return c_tm;
    };
    std::string path;

public:
    MiniLog(std::string Path="/var/log") {
        path = std::move(Path);
        logfile.open(fileNameLog(), std::ios_base::app);
    };

    enum class eLogFormat : int
    {
        LogDateTime,
        LogDate,
        LogTime,
        None
    };

    enum class eLogLevel : int
    {
        LogInfo,
        LogWarn,
        LogCrit,
        LogFatal,
        None
    };

    inline std::string fileNameLog()
    {
        char dt_str[80];
        std::string s(path + "/l1borg_%y_%m_%d.log");
        std::strftime(dt_str,80, s.c_str(), logDate());
        return dt_str;
    };


    template<typename T>
    void printLog(std::string hdr=nullptr, T s=nullptr, eLogLevel lvl=eLogLevel::None, eLogFormat fmt=eLogFormat::None)
    {
        char c_dt[30];
        switch(fmt) {
            case eLogFormat::LogTime:
                std::strftime(c_dt,30,"[%T]: ", logDate());
                break;
            case eLogFormat::LogDate:
                std::strftime(c_dt,30,"[%Y-%m-%d]: ", logDate());
                break;
            case eLogFormat::LogDateTime:
                std::strftime(c_dt,30,"[%Y-%m-%d %T]: ", logDate());
                break;
            default:
                std::strftime(c_dt,30," ", logDate());
        }

        std::stringstream ss;
        switch(lvl) {
            case eLogLevel::LogInfo:
                ss << "<info> " << c_dt;
                break;
            case eLogLevel::LogWarn:
                ss << "<warn> " << c_dt;
                break;
            case eLogLevel::LogCrit:
                ss << "<critical> " << c_dt;
                break;
            case eLogLevel::LogFatal:
                ss << "<fatal> " << c_dt;
                break;
            default:
                ss << c_dt;
        }

        if(hdr.empty()) {
           logfile << "> " << ss.str() << s << std::endl;
        } else {
           logfile << ss.str() << hdr << s << std::endl;
        }
    };

    void closeLog()
    {
        logfile.close();
    };

};


#endif // MINILOG_H
