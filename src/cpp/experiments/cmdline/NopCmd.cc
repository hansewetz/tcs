#include "NopCmd.h"
#include <iostream>
using namespace std;
namespace tcs{

const string NopCmd::CMDNAME="nop";

NopCmd::NopCmd(string const&progn,int argc,char*argv[],bool exitOnHelp,function<void(string const&)>cmderr):
    CmdBase(progn,argc,argv,exitOnHelp,cmderr){
  parseCmdline();
}
bool NopCmd::match(string const&cmd){
  return cmd==CMDNAME;
}
string NopCmd::cmdname(){
  return CMDNAME;
}
void NopCmd::print(ostream&os)const{
}
void NopCmd::addCmdlineOptionsAux(po::options_description&desc,po::positional_options_description&posdesc){
}
void NopCmd::parseCmdlineAux(po::variables_map const&vm){
}
void NopCmd::twmnbmAux(set<string>&baseset,optional<string>const&lstcmd,optional<string>const&lstopt)const{
  //  NOTE!
}
}
