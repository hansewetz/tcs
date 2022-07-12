#include "general/cmdline/NopCmd.h"
#include <iostream>
using namespace std;
namespace tcs{

const string NopCmd::CMDNAME="nop";

NopCmd::NopCmd(string const&progn,int argc,char*argv[],bool exitOnHelp,function<void(string const&)>cmderr,bool printOnPrint):
    CmdBase(progn,argc,argv,exitOnHelp,cmderr,printOnPrint){
  parseCmdline();
}
bool NopCmd::match(string const&cmd){
  return cmd==CMDNAME;
}
string NopCmd::cmdname(){
  return CMDNAME;
}
string NopCmd::cmddescr(){
  return "the "s+CMDNAME+" is a nop operation and does not execute anything";
}
void NopCmd::printAux(ostream&os)const{
}
void NopCmd::addCmdlineOptionsAux(po::options_description&desc,po::positional_options_description&posdesc){
}
void NopCmd::parseCmdlineAux(po::variables_map const&vm){
}
void NopCmd::twmnbmAux(set<string>&baseset,optional<string>const&lstcmd,optional<string>const&lstopt)const{
}
}
