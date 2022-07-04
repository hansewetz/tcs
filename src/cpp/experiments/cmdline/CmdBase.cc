#include "CmdBase.h"
#include "CmdHelper.h"
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
    progn_(progn),cmd_(argv[0]),argc_(argc),argv_(argv),exitOnHelp_(exitOnHelp),cmderr_(cmderr){
  // check if we have a twmnbm cmd line parameter
  auto it=find_if(&argv_[1],&argv_[argc_],[](char const*s){return strcmp(s,"--twmnbm")==0;});
  istwmnbm_=it!=&argv_[argc_];
}
// getters
int CmdBase::debug()const noexcept{return debug_;}
bool CmdBase::help()const noexcept{return help_;}
bool CmdBase::noexec()const noexcept{return noexec_;}
bool CmdBase::print()const noexcept{return print_;}
bool CmdBase::istwmnbm()const noexcept{return istwmnbm_;}
string const&CmdBase::progn()const noexcept{return progn_;}
string const&CmdBase::cmd()const noexcept{return  cmd_;}

// generate options that are allowed with current command line
vector<string>CmdBase::twmnbm()const{
  // first check if last cmd line parameter requires a parameter
  // (this is custom code for base class command line parameters)
  auto lstcmd=lastcmd();
  if(lstcmd){
    if(lstcmd.value()=="--debug")return {"0","1","2"};
  }
  // if here, last command does not require a parameter
  // remove cmd line parameters from return value that we already hvae
  set<string>baseset={"--help","--debug","--print","--noexec"};  // all valid options
  auto retset=subfromargv(baseset,{});

  // let derived class fill in return value
  // (we might get a reduces set form derived class in case derived class determines that)
  // (last cmd requires a parameter)
  twmnbmAux(retset,lastcmd(),lastopt());

  // return what we have left
  return vector<string>(begin(retset),end(retset));
}
// subtract a set of cmd line options from options in argv and return the remaining part
set<string>CmdBase::subfromargv(set<string>const&baseset1,set<string>const&baseset2)const{
  set<string>baseset=baseset1;
  for(auto&&s:baseset2)baseset.insert(s);
  set<string>retset=baseset;                                   // options to return
  set<string>argset;                                           // options we have on cmd line
  for(int i=1;i<argc_;++i)argset.insert(argv_[i]);             // ...
  for(auto&&basecmd:baseset){                                  // remove options that we already have
    if(argset.count(basecmd))retset.erase(basecmd);            // remove options we already have on cmd line
  }
  return retset;
}
// get last cmd line parameter on command line that is a '--' spec
optional<string>CmdBase::lastcmd()const{
  optional<string>ret;                           // return value
  if(argc_==0)return ret;                        // nothing to return
  for(int i=argc_-1;i>=0;--i){
    if(argv_[i][0]=='-')return argv_[i];
  }
  return std::nullopt;
}
// get last option on cmd line that is not a '--' parameter
optional<string>CmdBase::lastopt()const{
  optional<string>ret;                           // return value
  if(argc_==0)return ret;                        // nothing to return
  if(argv_[argc_-1][0]=='-')return std::nullopt; // return nullopt if last option is a '--xxx' parameter
  return argv_[argc_-1];
}


// main alg parsing and evaluating cmd line parameters
void CmdBase::parseCmdline(){
  // if we have an twmnbm request then do nothing
  if(istwmnbm()){
// NOTE! not sure if we should have this here
    cout<<tcs::strcat(" ",twmnbm())<<endl;
    exit(0);
  }
  // add commandline options from base class
  desc_=make_shared<po::options_description>();
  desc_->add_options()("debug,d",po::value<int>(&debug_),"debug level 0, 1, 2, default: 1 (optional)");
  desc_->add_options()("help,h",po::bool_switch(&help_),"help (optional)");
  desc_->add_options()("noexec",po::bool_switch(&noexec_),"if set do not execute operation (optional)");
  desc_->add_options()("print,p",po::bool_switch(&print_),"print all command line parameters (optional)");
  desc_->add_options()("twmnbm",po::bool_switch(&istwmnbm_),"generate all command line parameters for a sub-command");

  // add positional commandline options from base class
  // ...

  // add cmd line options from derived class
  addCmdlineOptionsAux(*desc_,posdesc_);

  // process cmd line parameters
  po::variables_map vm;                                                                            // variable map (stores all cmd line options + values)
  po::store(po::command_line_parser(argc_,argv_).options(*desc_).positional(posdesc_).run(),vm);   // parse cmd line optios + store result in variable map
  po::notify(vm);                                                                                  // notify in case user notficatino function specified for cmd option(s)

  // check if 'help' was specified
  if(help_)cmdusage(exitOnHelp_,"");
 
  // get some general command line parameters
  if(debug_<0||debug_>3)cmderr_("invalid debug level specified, debug level must be 0, 1 or 2");

  // get cmd line parameters from derived class
  parseCmdlineAux(vm);
}
// print usage info for this command
// (uses 'desc_' and 'posdesc_' to extract usage info)
void CmdBase::cmdusage(bool exitwhendone,string const&msg)const{
  if(msg!="")cerr<<msg<<endl;
// NOTE! we have to make sure that 'progn_' only contains the name of the program - i.e, not './test1'
  tcs::cmdusage(exitwhendone,*desc_,posdesc_,progn_,cmd_,progn_+"-"+cmd_+"(1)");
}
// check cmd line parametr for twmnbm
bool CmdBase::twmnbmCheckCmdParam(string const&cmdparam,string const&defparam,set<string>&baseset,optional<string>const&lstcmd,optional<string>const&lstopt)const{
  if(!lstcmd||lstcmd.value()!=cmdparam)return false;
  if(!lstopt)baseset={defparam};
  return true;
}
}
