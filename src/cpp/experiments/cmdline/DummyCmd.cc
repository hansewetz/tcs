#include "DummyCmd.h"
#include <iostream>
using namespace std;
namespace tcs{

const string DummyCmd::CMDNAME="dummy";

DummyCmd::DummyCmd(string const&progn,int argc,char*argv[],bool exitOnHelp,function<void(string const&)>cmderr):
    CmdBase(progn,argc,argv,exitOnHelp,cmderr){
  parseCmdline();

}
bool DummyCmd::match(string const&cmd){
  return cmd==CMDNAME;
}
string DummyCmd::cmdname(){
  return CMDNAME;
}
void DummyCmd::print(ostream&os)const{
}
void DummyCmd::addCmdlineOptionsAux(po::options_description&desc,po::positional_options_description&posdesc){
    desc.add_options()("dummy",po::value<string>(&dummy_),"dummy cmd line option (mandatory)");
}
void DummyCmd::parseCmdlineAux(po::variables_map const&vm){
  if(!vm.count("dummy"))cmderr_("missing mandatory 'dummy' cmd line parameter");
}
void DummyCmd::twmnbmAux(set<string>&baseset,optional<string>const&lstcmd,optional<string>const&lstopt)const{
  // check all cmd line parameters
  if(twmnbmCheckCmdParam("--dummy","dummy-parameter",baseset,lstcmd,lstopt))return;

  // if here we'll just merge in our own parameters with base class parameters
  set<string>argset{"--dummy"};
  baseset=subfromargv(baseset,argset);
}
}
