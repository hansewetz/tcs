#pragma once

/*
NOTE! TODO
- add twmnbm as a command object so we can print usage info for the program
- add a bash script so we can do bash command interpolation
- add support for getting files/dirs when we have an empty string or when we have the start of a file/dir
*/
#include "CmdHelper.h"
#include "general/utils/typeutils.h"
#include "general/utils/stringconst.h"
#include "general/utils/strutils.h"
#include <variant>
#include <iostream>
#include <vector>
#include <memory>
#include <map>
#include <optional>
#include <string>
#include <stdlib.h>
namespace tcs{

// class managing parsing/execution of command line
template<typename...CTs>class CmdLine;

template<typename F,typename...CTs>
requires requires(F f,CTs...cts,std::string cmd){
  (f(cts),...);                                        // must be able to call processing function
  {(CTs::match(cmd),...)}->std::same_as<bool>;         // must be able to check if command matches a string
  {(CTs::cmdname(),...)}->std::same_as<std::string>;   // must be able to get cmd name from command object
  {(CTs::cmddescr(),...)}->std::same_as<std::string>;  // must be able to get description from command object
}
class CmdLine<F,Typelist<CTs...>>{
public:
  using CmdTypes=Typelist<CTs...>;

  // ctors
  CmdLine(int argc,char*argv[],F const&f,std::string const&version):
      argc_(argc),argv_(argv),f_(f),
      cmdobj_(std::nullopt),
      version_(version),
      cmdnames_(getCmdnamesFromTypes<CTs...>()), // get cmd names from all sub commands
      cmddescr_(getCmddescrFromTypes<CTs...>())  // get cmd descriptions from all sub commands
  {
  }
  // parse (but do not execute) cmd line
  void parseCmdline(){
    // check if we have already parsed cmd line
    if(hasParsedCmdline_)return;

    // check two cases: (1) we have sub-command, (2) no sub-command
    if(hassubcmd()){
      cmdobj_=getMatchedCmdObject(CmdTypes(),argv_[0],argc_-1,&argv_[1],true,cmderr_);
      if(cmdobj_==std::nullopt)cmderr_(std::string("invalid/unknown sub-command, ")+tryhelpstr());
    }else{
      // check for no cmd line options first
      if(argc_==1){
        cmderr_(std::string("no command line option specified, "+tryhelpstr()));
      }
      // check for cmd line optiknos to main that prints and then exits (help, version etc.)
      for(auto&&[c,m]:printAndExitMainOpts_){
        auto&&[func,visible]=m;
        if(c==argv_[1])func();
      }
      // if here, then unknown option
      cmderr_(std::string("invalid/unknown option, ")+tryhelpstr());
    }
    hasParsedCmdline_=true;
  }
  // execute commands
  void executeCmd(){
    if(!hasParsedCmdline_)parseCmdline();

    // at this point we know we have a sub command
    visit(f_,cmdobj_.value());
  }
  // get all commands as strings
  std::vector<std::string>const&cmdnames()const{
    return cmdnames_;
  }
  // get all command descriptins as strings
  std::vector<std::string>const&cmddescr()const{
    return cmddescr_;
  }
  // quick way of printing info for about commands (cmd - cmd-descr)
  // (if user wants to format the info herself, she can get the command descriptions and then print)
  void printCmdDescr(std::ostream&os)const{
    std::cout<<"---------------------------------------------------------"<<std::endl;
    os<<BOLD<<"[-h|--help]: print this info"<<std::endl;
    os<<BOLD<<"[-v|--version]: print version of program"<<std::endl;
    auto itdescr=begin(cmddescr_);
    for(auto&&name:cmdnames_){
      os<<BOLD<<name<<UNBOLD<<": "<<*itdescr++<<std::endl;
    }
    std::cout<<"---------------------------------------------------------"<<std::endl;
  }
private:
  // default command line error function
  static void defaultCmderr(std::string const&msg){
    std::cerr<<msg<<std::endl;
    ::exit(1);
  }
  template<typename...Ts>
  static std::vector<std::string>getCmdnamesFromTypes(){
    std::vector<std::string>ret;
    (ret.push_back(Ts::cmdname()),...);
    return ret;
  }
  // get cmd descriptins from cmd types
  template<typename...Ts>
  static std::vector<std::string>getCmddescrFromTypes(){
    std::vector<std::string>ret;
    (ret.push_back(Ts::cmddescr()),...);
    return ret;
  }
  // do we have a sub command
  bool hassubcmd()const{
// NOTE! should check if we can do this better
    if(argc_<=1)return false;          // if no cmd line parameters, then no sub command
    if(argv_[1][0]=='-')return false;  // if first cmd line parameter starts with '-' then no sub command
    if(!strcmp(argv_[1],"twmnbm"))return false;  // if first cmd line parameter is 'twmnbm' treat is as special
    return true;
  }
  // helper - returning a string: 'try: <progname> --help'
  // (witgh the command in BOLD
  std::string tryhelpstr()const{
    return "try: "+BOLD+argv_[0]+" --help"+UNBOLD;
  }
  // helper options to main program that prints somethoing and then exits
  void printHelpAndExit()const{
    printCmdDescr(std::cout);
    exit(0);
  }
  void printVersionAndExit()const{
    std::cout<<"version: "<<version_<<std::endl;
    exit(0);
  }
  void printTwmmnbmAndExit()const{
// NOTE! hard codeded names of options
    std::vector<std::string>opts{"-h","--help","-v","--version"};
    copy(begin(cmdnames_),end(cmdnames_),back_inserter(opts));
    std::cout<<tcs::strcat(" ",opts)<<std::endl;
    exit(0);
  }

  // private data
  int argc_;
  char**argv_;
  F f_;
  std::optional<std::variant<CTs...>>cmdobj_;         // before parsing, set to std::nullopt
  std::vector<std::string>cmdnames_;                  // names of all commands
  std::vector<std::string>cmddescr_;                  // names of all commands
  std::string version_;                               // version of program
  std::function<void(std::string)>cmderr_=defaultCmderr;    // NOTE! hard coded

  // dynamic data
  bool hasParsedCmdline_=false;

  // cmd line options to main program that maps to actions that prints something and then exits
  struct MainOpt{
    std::function<void()>func;
    bool visible;
  };
  std::map<std::string,MainOpt>printAndExitMainOpts_={
    {"-h",{[&]{printHelpAndExit();},true}},
    {"--help",{[&]{printHelpAndExit();},true}},
    {"-v",{[&]{printVersionAndExit();},true}},
    {"--version",{[&]{printVersionAndExit();},true}},
    {"twmnbm",{[&]{printTwmmnbmAndExit();},false}}
  };
};
}
