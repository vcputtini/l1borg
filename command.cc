#include "command.h"

Command::Command()
{
  exists(CmdBorg);
}

int Command::processSyncIO(const std::vector<std::pair<std::string, std::string>> vEnv,
                     const std::vector<std::string>& vParms, const bool logger)
{
    MiniLog mlog;

    struct {
      boost::optional<int> exit_code;
      boost::optional<int> signal;
      std::error_code ec{};
    } result;

    std::string sParms(Command::CmdBorg);
    DEBUG( sParms );

    if(vParms.size() == 1) {
        sParms += " " + vParms.at(0);
    } else { // <params></params>
        std::for_each(vParms.begin(), vParms.end(), [&sParms](std::string a)
            {sParms += " " + a; return sParms;}
        );
    }
    DEBUG( sParms );

    // Custom environment variables
    auto env = boost::this_process::environment();
    for(auto &m : vEnv) {
        env[m.first.c_str()] = m.second.c_str();
    }

    bp::ipstream pipe_stream_err;
    bp::ipstream pipe_stream_out;

    // Exec child process
    try {
        std::ofstream logfile(mlog.fileNameLog(), std::ios_base::app);
        bp::child c(sParms.c_str(),  env, (bp::std_out & bp::std_err) > pipe_stream_out,
                    bp::on_exit = [&result, &c](int, const std::error_code &ec) {
                        auto exit_status = c.native_exit_code();
                        result.exit_code = boost::make_optional(WIFEXITED(exit_status),WEXITSTATUS(exit_status));
                        result.signal = boost::make_optional(WIFSIGNALED(exit_status),WTERMSIG(exit_status));
                        result.ec = ec;
                    });

        std::string  line;
        /**
         * Constants for use in ofstream
         * std::ios_base::app       = Append mode
         * std::ios_base::binary    = Binary mode
         * std::ios_base::in        = Read-only
         * std::ios_base::out       = Write-only
         * std::ios_base::trunc     = Truncate the file before opening. Erase the content
         * std::ios_base::ate       = Go to the end of the file after opening
         */

        while(c.running() && std::getline(pipe_stream_out, line) && !line.empty()) {
            if(logger) mlog.printLog("", line);
            else std::cout << line << std::endl;
        }
        c.wait();

        if (result.exit_code) {
            mlog.printLog("Exited with  =", *result.exit_code, MiniLog::eLogLevel::LogInfo, MiniLog::eLogFormat::LogTime);
        }
        if (result.signal) {
            mlog.printLog("Signaled with signal #  =", *result.exit_code, MiniLog::eLogLevel::LogInfo, MiniLog::eLogFormat::LogTime);
        }
        mlog.printLog("Exit Code  =", *result.exit_code, MiniLog::eLogLevel::LogInfo, MiniLog::eLogFormat::LogTime);
        mlog.printLog("Error Code =", result.ec.value(), MiniLog::eLogLevel::LogInfo, MiniLog::eLogFormat::LogTime);
    } catch(std::exception const& ex) {
       mlog.printLog("", ex.what(), MiniLog::eLogLevel::LogFatal, MiniLog::eLogFormat::LogDateTime);
    };

    return result.ec.value() == 0 ? result.ec.value() : 255;
}

bool Command::exists(const fs::path& p)
{
  MiniLog mlog;
  try {
    fs::file_status fstatus = fs::file_status(fs::status(p));
    if(fs::status_known(fstatus) ? fs::exists(fstatus) : fs::exists(p)) {
      return true;
    }
  } catch (fs::filesystem_error err){
    mlog.printLog("Command()::exists() error", err.what(), MiniLog::eLogLevel::LogFatal, MiniLog::eLogFormat::LogDateTime);
  }
  return false;
}

bool Command::mkdir(const fs::path& p)
{
  MiniLog mlog;
  if(exists(p))
     return true;

  try {
    std::error_code ec;
    bool b = fs::create_directories(p, ec );
    if(!b) {
       mlog.printLog("Command()::mkdir() error", ec.message(), MiniLog::eLogLevel::LogFatal, MiniLog::eLogFormat::LogDateTime);
    }
    return b;
  } catch (fs::filesystem_error err){
    mlog.printLog("Command()::mkdir() error", err.what(), MiniLog::eLogLevel::LogFatal, MiniLog::eLogFormat::LogDateTime);
  }
  return false;
}

bool Command::rmdir(const fs::path& p)
{
  MiniLog mlog;
  try {
    std::error_code ec;
    bool b = fs::remove(p, ec );
    if(!b) {
       mlog.printLog("Command()::rmdir() error", ec.message(), MiniLog::eLogLevel::LogFatal, MiniLog::eLogFormat::LogDateTime);
    }
    return b;
  } catch (fs::filesystem_error err){
    mlog.printLog("Command()::rmdir() error", err.what(), MiniLog::eLogLevel::LogFatal, MiniLog::eLogFormat::LogDateTime);
  }
  return false;
}
