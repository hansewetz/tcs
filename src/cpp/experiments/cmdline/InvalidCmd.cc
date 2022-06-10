#include "InvalidCmd.h"
#include <iostream>
using namespace std;
namespace tcs{

InvalidCmd::InvalidCmd(string const&progn,int argc,char*argv[],bool exitOnHelp,function<void(string const&)>cmderr):
    CmdBase(progn,argc,argv,exitOnHelp,cmderr){
  parseCmdline();
}
bool InvalidCmd::match(string const&cmd){
  return false;
}
void InvalidCmd::print(ostream&os)const{
}
void InvalidCmd::addCmdlineOptionsAux(po::options_description&desc,po::positional_options_description&posdesc){
}
void InvalidCmd::parseCmdlineAux(po::variables_map const&vm){
}
}
