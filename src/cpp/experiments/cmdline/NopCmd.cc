#include "NopCmd.h"
#include <iostream>
using namespace std;
namespace tcs{

NopCmd::NopCmd(string const&progn,int argc,char*argv[],bool exitOnHelp,function<void(string const&)>cmderr):
    CmdBase(progn,argc,argv,exitOnHelp,cmderr){
  parseCmdline();
}
bool NopCmd::match(string const&cmd){
  return cmd=="nop";
}
void NopCmd::print(ostream&os)const{
}
void NopCmd::addCmdlineOptionsAux(po::options_description&desc,po::positional_options_description&posdesc){
}
void NopCmd::parseCmdlineAux(po::variables_map const&vm){
}
}
