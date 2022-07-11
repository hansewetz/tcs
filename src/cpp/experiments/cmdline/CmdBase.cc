#include "CmdBase.h"
#include "CmdHelper.h"
#include "general/utils/strutils.h"
#include "general/utils/utils.h"
#include "general/utils/typeutils.h"
#include <iostream>
using namespace std;
namespace fs=std::filesystem;
namespace tcs{

// debug print operator
ostream&operator<<(ostream&os,CmdBase const&p){
  os<<"----- (begin) cmd line options -----"<<endl;
  os<<"debug: "<<p.debug()<<endl;
  os<<"help: "<<boolalpha<<p.help()<<endl;
  os<<"print: "<<boolalpha<<p.print()<<endl;
  os<<"noexec: "<<boolalpha<<p.noexec()<<endl;
  p.printAux(os);
  os<<"----- (end) cmd line options -----"<<endl;
  return os;
}
// ctor
CmdBase::CmdBase(string const&progn,int argc,char*argv[],bool exitOnHelp,function<void(string const&)>cmderr,bool printOnPrint):
    progn_(progn),cmd_(argv[0]),argc_(argc),argv_(argv),exitOnHelp_(exitOnHelp),cmderr_(cmderr),printOnPrint_(printOnPrint){

  // check if we have a twmnbm cmd line parameter - if so, process it manually
  // usage: --twmnbm <nspace>
  // where <nspace> is 0 if there is no space at end of command line and 1 if there is a space
  // (for some cmd line parameters, it matters if there is a space. For example, for '--debug 1' if there is a space)
  // (after '1' then the suggestins we generate will be '--noexec' etc. If there is no space, the suggestion we generate is '1'.)
  auto twmnbmSpacesAtEnd=checkAndGetCmdlineParam<int>(argc_,argv_,"--twmnbm",cmderr_);
  if(twmnbmSpacesAtEnd){
    istwmnbm_=true;
    twmnbmSpacesAtEnd_=twmnbmSpacesAtEnd.value();
  }
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

  // handle twmnbm for '--debug'
// NOTE! should make this better
//       encapsulate it into a function that can be used for other options that have a fixed set of parameters
  if(lastcmd()&&lastcmd().value()=="--debug"){                       // if we have '--debug' option ...
    if(lastopt()==std::nullopt)return {"0","1","2"};                 // if no parameter to '--debug'
    if(istwmnbm_&&twmnbmSpacesAtEnd_==0)return {lastopt().value()};  // if '--debug 0' with no space after '0' (or 1 or 2)
  }
  // if here, last command does not require a parameter
  // remove cmd line parameters from return value that we already have
  set<string>baseset={"--help","--debug","--print","--noexec"};
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

  // check if we should print cmd line parameters
  if(print_&&printOnPrint_)cout<<*this;
 
}
// print usage info for this command
// (uses 'desc_' and 'posdesc_' to extract usage info)
void CmdBase::cmdusage(bool exitwhendone,string const&msg)const{
  if(msg!="")cerr<<msg<<endl;
  tcs::cmdusage(exitwhendone,*desc_,posdesc_,progn_,cmd_,progn_+"-"+cmd_+"(1)");
}
// check cmd line parametr for twmnbm
bool CmdBase::twmnbmCheckCmdParam(string const&cmdparam,string const&defparam,set<string>&baseset,optional<string>const&lstcmd,optional<string>const&lstopt)const{
  if(!lstcmd||lstcmd.value()!=cmdparam)return false;   // check if last cmd parameter == lstcmd, if not, then return false
  if(!lstopt)baseset={defparam};                       // we now have the lstcmd, if last option is not set, then set 'baseset' to defparam
  return true;                                         // return true sionce we have a match
}
// generate file proposal for an argument
bool CmdBase::twmnbmCheckCmdParamDirFile(bool isfile,string const&cmdparam,
                                      set<string>&baseset,optional<string>const&lstcmd,
                                      optional<string>const&lstopt)const{

  // if last cmd line option is not 'cmdparam' then add it to proposed options
  if(!lstcmd||lstcmd.value()!=cmdparam){
    baseset=subfromargv(baseset,{cmdparam});
    return false;
  }
  // if we have parameter to 'cmdparam' and there is a space after the parameters we are done
  // (we don't need to add something to baseset)
  if(lstopt&&twmnbmSpacesAtEnd_>0){
    return true;
  }
  // determine directory from which to generate suggested file names
  fs::path basedir="./";        // if we don't have an option to '--dummy' then directory for suggested files is current directory
  fs::path userpath;
  if(lstopt){
    // get user specified path and what filesystem determines to be the filename (could be a dircetory)
    basedir=lstopt.value();
    auto fname=basedir.filename();

    // track if we should strip filename from userpath
    // (if we have a directory as parent, then use parent dir as 'basedir' to list replies from)
    std::error_code ec;
    if(!(fname=="."||fname==".."||fs::is_directory(basedir,ec))){
      basedir.remove_filename();
    }
  }
  // check that 'basedir' is actually a directory
  std::error_code ec_isdir;
  if(!fs::is_directory(basedir,ec_isdir)||ec_isdir){   // should also check for link to dir
    return false;
  }
  // generate files in 'basedir' directory
  baseset.clear();
  std::error_code dir_ec;
  auto diriterator=fs::directory_iterator(basedir,dir_ec);
  if(dir_ec)return false;               // we migh have been dealing with a basdir that is syntactically correct but doe snot match anything in the file system
  for (auto&&entry:diriterator){
    auto pentry=entry.path();

    // check if we have a directory
    if(isfile&&fs::is_directory(pentry,ec_isdir)&&!ec_isdir){
      pentry+="/*";   // mark that we don't want to expand this directory as teh final result
    }
    // filter entries on full or partial filename (option to 'cmdparam')
    // (not actually needed since; 'compgen' will do this for us)

    baseset.insert(pentry.string());
  }
  return true;
}
}
