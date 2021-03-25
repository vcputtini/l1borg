#include "envconfig.h"

namespace EnvironConfig
{
  EnvConfig::EnvConfig()
    : bError(false),
     errorText(nullptr)
  {
     mVars = {
        { eEnvVars::BorgRepoDir,        "BORG_REPO" },
        { eEnvVars::BorgConfigDir,      "BORG_CONFIG_DIR" },
        { eEnvVars::BorgSecurityDir,    "BORG_SECURITY_DIR" },
        { eEnvVars::BorgKeysDir,        "BORG_KEYS_DIR" },
        { eEnvVars::BorgCacheDir,       "BORG_CACHE_DIR" },
        { eEnvVars::BorgLogDir,         "BORG_LOG_DIR" },
        { eEnvVars::BorgTmpDir,         "TMPDIR" }
     };

     findCfgFile(configXMLFName);

     if(doc.LoadFile(configXMLFName.c_str()) == tinyxml2::XML_SUCCESS) {
           root=doc.RootElement();
           XMLElement *env;
           if( (env=root->FirstChildElement("env")) == nullptr) {
             bError = true;
             errorText = doc.ErrorStr();
             return;
           }

           for(auto e : EnvVars) {
             auto varname = Name(e);
             if(env->FirstChildElement( varname.c_str() ) ) {
               mEnv.insert( std::make_pair(e,
                 std::make_tuple( varname,
                       std::string(env->FirstChildElement(varname.c_str())->GetText()))));
               // Create directories
              Command cmd;
              cmd.mkdir(std::string(env->FirstChildElement(varname.c_str())->GetText()));
             }
           }
     } else {
       bError = true;
       errorText = doc.ErrorStr();
     } // doc.LoadFile
  }

  std::string EnvConfig::Name(EnvironConfig::eEnvVars e)
  {
    std::map<eEnvVars, std::string>::iterator it = mVars.find(e);
    if( it != mVars.end() )
       return it->second;

    return nullptr;
  }

  std::string EnvConfig::Data(EnvironConfig::eEnvVars e)
  {
     std::map<eEnvVars, tData>::iterator it = mEnv.find(e);
     if( it != mEnv.end() )
        return std::get<1>(it->second);

     return nullptr;
  }

  std::string EnvConfig::Combo(EnvironConfig::eEnvVars e)
  {
    std::map<eEnvVars, tData>::iterator it = mEnv.find(e);
    if( it != mEnv.end() ) {
      std::string s = std::get<0>(it->second) + "=" + std::get<1>(it->second);
      return s;
     }
    return nullptr;
  }

  std::vector<std::string> EnvConfig::GetVars()
  {
    std::vector<std::string> v = {
      EnvConfig::get().Combo(eEnvVars::BorgRepoDir),
      EnvConfig::get().Combo(eEnvVars::BorgConfigDir),
      EnvConfig::get().Combo(eEnvVars::BorgSecurityDir),
      EnvConfig::get().Combo(eEnvVars::BorgSecurityDir),
      EnvConfig::get().Combo(eEnvVars::BorgKeysDir),
      EnvConfig::get().Combo(eEnvVars::BorgCacheDir),
      EnvConfig::get().Combo(eEnvVars::BorgTmpDir)
    };
    return v;
  }

  std::vector<std::pair<std::string, std::string>> EnvConfig::GetVarsPair()
  {
      std::vector<std::pair<std::string, std::string>> v = {
        std::make_pair(EnvConfig::get().Name(eEnvVars::BorgRepoDir),EnvConfig::get().Data(eEnvVars::BorgRepoDir)),
        std::make_pair(EnvConfig::get().Name(eEnvVars::BorgConfigDir),EnvConfig::get().Data(eEnvVars::BorgConfigDir)),
        std::make_pair(EnvConfig::get().Name(eEnvVars::BorgSecurityDir),EnvConfig::get().Data(eEnvVars::BorgSecurityDir)),
        std::make_pair(EnvConfig::get().Name(eEnvVars::BorgKeysDir),EnvConfig::get().Data(eEnvVars::BorgKeysDir)),
        std::make_pair(EnvConfig::get().Name(eEnvVars::BorgCacheDir),EnvConfig::get().Data(eEnvVars::BorgCacheDir)),
        std::make_pair(EnvConfig::get().Name(eEnvVars::BorgTmpDir),EnvConfig::get().Data(eEnvVars::BorgTmpDir))
      };
      return v;
  }

  // **************************************************************************
  EnvRepos::EnvRepos()
    : bError(false),
      errorText(nullptr)
  {
    MiniLog mlog;
    auto toLowerCase = [](std::string str) {
         std::transform(str.begin(), str.end(), str.begin(), ::tolower);
         return str;
     };

    EnvConfig::get().findCfgFile(configXMLFName);

    if(doc.LoadFile(configXMLFName.c_str()) == tinyxml2::XML_SUCCESS) {
          root=doc.RootElement();
          XMLElement *repos;
          if( (repos=root->FirstChildElement("repos")) == nullptr) {
            bError = true;
            errorText = doc.ErrorStr();
            return;
          }
          int idx=0;
          while(repos)
          {
            // <repos name="name" created="yes|no">
            const XMLAttribute *attributeOfNodes = repos->FirstAttribute();
            std::string nameRepo = attributeOfNodes->Value();
            std::string sCreated = toLowerCase(repos->Attribute("created"));
            std::string sRunrepo = toLowerCase(repos->FirstChildElement("run")->GetText());

            XMLElement *backup = repos->FirstChildElement("backup");
            while(backup) {
              const XMLAttribute *attributeOfBackup = backup->FirstAttribute();
              std::string sSource;
              if(backup->FirstChildElement("source"))
                sSource =  backup->FirstChildElement("source")->GetText();
              else {
                bError = true;
                DEBUG( "l1borg.xml: <source></source> is mandatory." );
                mlog.printLog("l1borg.xml", "<source></source> is mandatory.", MiniLog::eLogLevel::LogFatal, MiniLog::eLogFormat::LogDateTime);
                return;
              }
              std::string sExcludes;
              if(backup->FirstChildElement("exclude-file")) {
                 if(backup->GetText() != nullptr)
                    sExcludes = toLowerCase( backup->FirstChildElement("exclude-file")->GetText() );
                 else
                    sExcludes = "";
               } else
                    sExcludes = "";
              std::string sDryRun;
              if(backup->FirstChildElement("dry-run"))
                 sDryRun = toLowerCase( backup->FirstChildElement("dry-run")->GetText() );
              else
                 sDryRun = "no";

              std::string sParms;
              if(backup->FirstChildElement("params"))
                 sParms = toLowerCase( backup->FirstChildElement("params")->GetText() );
              else
                 sParms = "";

              DEBUG( sParms );
              std::stringstream ss;
              ss << nameRepo << ":" << idx;  // deduplicate the index
              mRepos.insert( std::make_pair( std::make_tuple(ss.str(), sRunrepo, sCreated),
                      std::make_tuple(attributeOfBackup->Value(), sSource, sExcludes, sDryRun, sParms)
                    )
              ); // mRepos
              idx++;
              backup=backup->NextSiblingElement(); // jump to next node
            }
            repos=repos->NextSiblingElement(); // jump to next node
          }
    } else {
      bError = true;
      errorText = doc.ErrorStr();
    } // doc.LoadFile
  }

  void EnvRepos::SetCreated(std::string repo)
  {
    std::set<std::string>::iterator it;
    if(doc.LoadFile(configXMLFName.c_str()) == tinyxml2::XML_SUCCESS) {
          root=doc.RootElement();
          XMLElement *repos;
          if( (repos=root->FirstChildElement("repos")) == nullptr) {
            bError = true;
            errorText = doc.ErrorStr();
            return;
          }
          while(repos) {
            const XMLAttribute *attributeOfNodes = repos->FirstAttribute();
            std::string nameRepo = attributeOfNodes->Value();
            if(repo == nameRepo) {
                repos->SetAttribute("created","yes");
            }
            repos=repos->NextSiblingElement();
          }
          doc.SaveFile(configXMLFName.c_str());  // You've to save the file after the change
          repoSet.clear();
    } else {
      bError = true;
      errorText = doc.ErrorStr();
    } // doc.LoadFile
  }

  void EnvRepos::MakeBackup()
  {
    std::vector<std::string> vCmd;
    std::vector<std::string> vEnv = EnvConfig::GetVars();

    // lambda: extracts the name part of the repository
    // R1:0 retorn R1
    auto subStr = [](std::string s) {
      return s.substr(0, s.find(":"));
    };

    MiniLog mlog;
    mlog.printLog("Begin Backup: ", "",MiniLog::eLogLevel::LogInfo, MiniLog::eLogFormat::LogDateTime);

    for(auto &v : mRepos) {
      if( std::get<1>(v.first) != "no") {
        vCmd.push_back(""); // the first element must be empty
        vCmd.push_back("create");
        if( std::get<3>(v.second) == "yes")
           vCmd.push_back("--dry-run");

        // split <params>
        std::vector<std::string> vsplit;
        boost::split(vsplit, std::get<4>(v.second), boost::is_any_of(" "));
        for(size_t i=0; i < vsplit.size(); i++)
            vCmd.push_back(vsplit[i]);

        if( std::get<2>(v.second) != "") { // <excludes> == void
          vCmd.push_back("--exclude-from");
          vCmd.push_back(std::get<2>(v.second));
        }
        // std::get<0>(v.first): reponame
        // std::get<0>(v.second): filename
        std::stringstream ss;
        ss << EnvConfig::get().Data(eEnvVars::BorgRepoDir) << "/"
            << subStr(std::get<0>(v.first)) << "::" << std::get<0>(v.second);
        vCmd.push_back(ss.str());
        vCmd.push_back( std::get<1>(v.second)); // source
        int e = Command::processSyncIO( EnvConfig::GetVarsPair(), vCmd);
        if(e == 255)
            break;
        vCmd.clear();
      }
    }
    mlog.printLog("End Backup: ", "",MiniLog::eLogLevel::LogInfo, MiniLog::eLogFormat::LogDateTime);
    mlog.closeLog();
  }

  void EnvRepos::ExecCmd(std::string cmd)
  {
      std::vector<std::string> vParms;
      vParms.push_back(cmd);
      int e = Command::processSyncIO( EnvConfig::GetVarsPair(), vParms, false);
      DEBUG( e );
  }

  void EnvRepos::ExecAlias(std::string parm, std::string cmd, std::string cmd1)
  {
      auto subStr = [](std::string s) {
        return s.substr(0, s.find(":"));
      };

      std::vector<std::string> vParms;

      if(parm == "diff") {
          vParms.push_back(parm);
          vParms.push_back(EnvConfig::get().Data(EnvironConfig::eEnvVars::BorgRepoDir) + "/" + cmd + " " + cmd1);
          int e = Command::processSyncIO( EnvConfig::GetVarsPair(), vParms, false);
          DEBUG(e);
          return;
      }

      if(cmd == "all") {
         for(auto &v : mRepos) {
             if(std::get<1>(v.first) == "yes") {
                DEBUG( subStr(std::get<0>(v.first)) );
                vParms.push_back(parm);
                vParms.push_back(EnvConfig::get().Data(EnvironConfig::eEnvVars::BorgRepoDir) + "/" + subStr(std::get<0>(v.first)));
                int e = Command::processSyncIO( EnvConfig::GetVarsPair(), vParms, false);
                DEBUG(e);
                std::cout << "\n\n\n";
             }
         }
      } else {
         vParms.push_back(parm);
         vParms.push_back(EnvConfig::get().Data(EnvironConfig::eEnvVars::BorgRepoDir) + "/" + cmd);
         int e = Command::processSyncIO( EnvConfig::GetVarsPair(), vParms, false);
         DEBUG(e);
      }
  }

  void EnvRepos::InitRepo()
  {
    MiniLog mlog;
    std::vector<std::string> vCmd;
    std::vector<std::string> vEnv = EnvConfig::GetVars();

    // lambda: extracts the name part of the repository
    // R1:0 return R1
    auto subStr = [](std::string s) {
      return s.substr(0, s.find(":"));
    };

    std::string aux="";

    mlog.printLog("InitRepo ","Initializating.");
    for(auto &r : mRepos) {
      if( std::get<2>(r.first) == "yes") {// already been created
        std::stringstream ss;
        ss << " l1borg: Repository [" << subStr(std::get<0>(r.first))
          << "] already been created at ["
          << EnvConfig::get().Data(eEnvVars::BorgRepoDir) << "]" << std::endl;
        DEBUG(ss.str());
        mlog.printLog("InitRepo ",ss.str(), MiniLog::eLogLevel::LogInfo);
      } else if( aux != subStr(std::get<0>(r.first)) ) { // avoid duplication
        mlog.printLog("InitRepo ", subStr(std::get<0>(r.first)) );
        vCmd.push_back(""); // the first element must be empty
        vCmd.push_back("init");
      	vCmd.push_back("-e");
      	vCmd.push_back("none");
        std::stringstream ss;
        ss << EnvConfig::get().Data(eEnvVars::BorgRepoDir) << "/";
        ss << subStr(std::get<0>(r.first));
        DEBUG(subStr(std::get<0>(r.first)) );
        vCmd.push_back(ss.str());

        int e = Command::processSyncIO( EnvConfig::GetVarsPair(), vCmd);
        if( e == 1) {
            DEBUG( "error" );
            mlog.printLog("Command() (InitRepo) error=", e, MiniLog::eLogLevel::LogFatal, MiniLog::eLogFormat::LogDateTime);
            break;
        } else {
            SetCreated(subStr(std::get<0>(r.first)));
        }

        vCmd.clear(); // size to 0
        aux = subStr(std::get<0>(r.first));
     }
    } // for()
    mlog.printLog("InitRepo ","Done.");
    mlog.closeLog();
    DEBUG( "*********" );
  }
} // EnvironConfig
