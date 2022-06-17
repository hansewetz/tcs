#pragma once
#include "InvalidCmd.h"
#include "CmdHelper.h"
#include "general/utils/typeutils.h"
#include <variant>
#include <iostream>
#include <vector>
#include <memory>
namespace tcs{

// class managing parsing/execution of command line
template<typename...CTs>class CmdLine;

template<typename F,typename...CTs>
requires requires(F f,CTs...cts,std::string cmd){
  (f(cts),...);                    // must be able to call processing function
  (CTs::match(cmd),...);           // must be able to check if command matches a string
  (CTs::cmdname(),...);            // must be able to get cmd name from command object
}
class CmdLine<F,Typelist<CTs...>>{
public:
  using CmdTypes=Typelist<CTs...>;

  // ctors
  CmdLine(int argc,char*argv[],F const&f):
      argc_(argc),argv_(argv),f_(f),
      cmdobj_(InvalidCmd(argv[0])),              // note: InvalidCmd object must not parse cmd line parameters
      cmdnames_(getCmdnamesFromTypes<CTs...>())  // get cmd names from all sub commands
  {
    if(argc>1)cmdobj_=InvalidCmd(argv[0],argc,&argv[1]);
  }
  // parse (but do not execute) cmd line
  void parseCmdline(){
    if(hasParsedCmdline_)return;
    cmdobj_=getMatchedCmdObject(CmdTypes(),argv_[0],argc_-1,&argv_[1],exitOnHelp_,cmderr_);
    hasParsedCmdline_=true;
  }
  // execute commands
  void executeCmd(){
    if(!hasParsedCmdline_)parseCmdline();
    visit(f_,cmdobj_);
  }
  // get all commands
  std::vector<std::string>const&cmdnames()const{
    return cmdnames_;
  }
private:
  // default command line error function
  static void defaultCmderr(std::string const&msg){
    std::cerr<<msg<<std::endl;
    ::exit(1);
  }
  // get cmd names from cmd types
  template<typename...Ts>
  static std::vector<std::string>getCmdnamesFromTypes(){
    std::vector<std::string>ret;
    (ret.push_back(Ts::cmdname()),...);
    return ret;
  }
  // private data
  int argc_;
  char**argv_;
  F f_;
  std::variant<CTs...>cmdobj_;         // before parsing, set to InvalidCmd(...)
  std::vector<std::string>cmdnames_;   // names of all commands
  bool exitOnHelp_=true;                                    // NOTE! hard coded
  std::function<void(std::string)>cmderr_=defaultCmderr;    // NOTE! hard coded

  // dynamic data
  bool hasParsedCmdline_=false;
};
}
