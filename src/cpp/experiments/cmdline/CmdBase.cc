#include "CmdBase.h"
#include "CmdUsageInfo.h"
#include "general/utils/strutils.h"
#include "general/utils/utils.h"
#include "general/utils/typeutils.h"
#include <iostream>
using namespace std;
namespace tcs{

// debug print operator
ostream&operator<<(ostream&os,CmdBase const&p){
  os<<"progn: "<<p.progn()<<", cmd: "<<p.cmd()<<", argv: [";
  vector<string>vargv(p.argv_,p.argv_+p.argc_);
  os<<tcs::strcat(", ",vargv)<<"], ";
  os<<"debug: "<<p.debug()<<", noexec: "<<boolalpha<<p.noexec()<<", print: "<<p.print()<<", help: "<<p.help()<<", ";
  os<<"[";
  p.print(os);
  os<<"]";
  return os;
}
// ctor
CmdBase::CmdBase(string const&progn,int argc,char*argv[],bool exitOnHelp,function<void(string const&)>cmderr):
    progn_(progn),argc_(argc),argv_(argv),exitOnHelp_(exitOnHelp),cmderr_(cmderr){
}
// getters
int CmdBase::debug()const noexcept{return debug_;}
bool CmdBase::help()const noexcept{return help_;}
bool CmdBase::noexec()const noexcept{return noexec_;}
bool CmdBase::print()const noexcept{return print_;}
string CmdBase::progn()const noexcept{return progn_;}
string CmdBase::cmd()const noexcept{return  cmd_;}

// main alg parsing and evaluating cmd line parameters
void CmdBase::parseCmdline(){
  // add commandline options from base class
  desc_.add_options()("debug,d",po::value<int>(&debug_),"debug level 0, 1, 2, default: 1 (optional)");
  desc_.add_options()("help,h",po::bool_switch(&help_),"help (optional)");
  desc_.add_options()("noexec",po::bool_switch(&noexec_),"if set do not execute operation (optional)");
  desc_.add_options()("print,p",po::bool_switch(&print_),"print all command line parameters (optional)");

  // add positional commandline options from base class
  // ...

  // add cmd line options from derived class
  addCmdlineOptionsAux(desc_,posdesc_);

  // process cmd line parameters
  po::variables_map vm;
  po::store(po::command_line_parser(argc_,argv_).options(desc_).positional(posdesc_).run(),vm);
  po::notify(vm);

  // check if 'help' was specified
  if(help_)cmdusage(exitOnHelp_,"");

  // get some general command line parameters
cout<<"=== debug: "<<debug_<<endl;
  if(debug_<0||debug_>3)cmderr_("invalid debug level specified");

  // get cmd line parameters from derived class
  parseCmdlineAux(vm);
}
// print usage info for this command
// (uses 'desc_' and 'posdesc_' to extract usage info)
void CmdBase::cmdusage(bool exitwhendone,string const&msg)const{
  if(msg!="")cerr<<msg<<endl;
  tcs::cmdusage(desc_,posdesc_,progn_,cmd_,"tcs-"s+cmd_+"(1)");
  if(exitwhendone)exit(1);
}
}
