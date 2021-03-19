#include "l1borg.h"

void help()
{
    std::cout << "Valid options are:" << std::endl
      << "--init-repo   : Initialize all repositories." << std::endl
      << "--make-backup : Make backup." << std::endl
      << "--cmd         : Execute a BorgBackup command. " << std::endl
      << "                Double quotes are mandatory:" << std::endl
      << "                e.g.: l1borg --cmd \"info PATH/REPONAME --prefix=srv1\"" << std::endl
      << "--help        : See this help." << std::endl << std::endl
      << "l1borg.xml    : The possible installation folders for the l1borg.xml file are:" << std::endl
      << "                /etc | /etc/borg | /etc/l1borg | same folder as l1borg executable." << std::endl;
}

int main(int argc, char **argv)
{
  int opt = 0;
  int long_index = 0;

  MiniLog mlog;

  static struct option long_options[] = {
    {"init-repo",   no_argument,        0, 'a'},
    {"make-backup", no_argument,        0, 'b'},
    {"cmd",         required_argument,  0, 'c'},
    {"help",        no_argument,        0, 'w'},
    {0,             0,                  0, 0}
  };

  std::string folder;
  if( !EnvConfig::get().findCfgFile(folder)) {
      std::string_view sv = "\n<warn> l1borg.xml NOT FOUND! Please create it!\n"
         "The possible installation folders for the l1borg.xml file are:\n"
         "/etc | /etc/borg | /etc/l1borg | same folder as l1borg executable.\n\n";
      std::cout << sv;
      return EXIT_FAILURE;
  }

  while ((opt=getopt_long_only(argc,argv,"",long_options, &long_index)) != -1) {
    switch(opt) {
      case 'a':
        if(EnvConfig::get().ErrorText() != std::string()) {
          DEBUG( EnvConfig::get().ErrorText() );
          exit( EXIT_FAILURE );
         }
        EnvRepos::get().InitRepo();
        break;
      case 'b':
        if(EnvConfig::get().ErrorText() != std::string()) {
          DEBUG( EnvConfig::get().ErrorText() );
          exit(EXIT_FAILURE);
         }
        EnvRepos::get().MakeBackup();
        break;
      case 'c':
        EnvRepos::get().ExecCmd(std::string(optarg));
        break;
      case 'w':
        help();
        break;
      default:
        help();
          exit(EXIT_SUCCESS);
    }
  }

  if( optind == 1) {
    help();
  }

  mlog.closeLog();

  exit(EXIT_SUCCESS);
}
