#include "apputils/cmdline/CmdHelper.h"
#include "general/utils/stringconst.h"
#include <iostream>
#include <filesystem>
using namespace std;
namespace fs=std::filesystem;
namespace tcs{

// print usage info from boost::program_options::positional_options_description
// (and exit)
void cmdusage(bool exitwhendone,boost::program_options::options_description const&desc,boost::program_options::positional_options_description const&posdesc,
           string const&progname){
  cmdusage(exitwhendone,desc,posdesc,progname,std::nullopt,std::nullopt);

}
void cmdusage(bool exitwhendone,boost::program_options::options_description const&desc,boost::program_options::positional_options_description const&posdesc,
           string const&progname,optional<string>const&subcmd,std::optional<string>const&manpage){
  ostream&os=cerr;
  os<<"Usage: ";
  if(manpage)os<<"[for additional information please see: "<<BOLD<<"man "<<manpage.value()<<UNBOLD<<"]";
  os<<endl;
  std::filesystem::path p(progname);
  os<<"  "<<p.filename().string();
  if(subcmd)os<<" "<<subcmd.value();
  os<<" [options] --";
  string last;
  int rep=0;
  for(int pos=0;pos<posdesc.max_total_count();pos++){
    auto&&nm=posdesc.name_for_position(pos);
    if(nm==last){
      if(!rep)os<<" ...";
      if(rep++>1000) break;
    }else {
      os<<" "<<nm;
      last=nm;
      rep=0;
    }
  }
  os<<endl<<endl;
  os<<desc<<endl;
  if(exitwhendone)exit(1);
}
}
