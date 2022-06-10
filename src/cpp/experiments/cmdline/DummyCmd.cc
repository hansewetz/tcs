#include "DummyCmd.h"
#include <iostream>
using namespace std;
namespace tcs{

DummyCmd::DummyCmd(string const&progn,int argc,char*argv[],bool exitOnHelp,function<void(string const&)>cmderr):
    CmdBase(progn,argc,argv,exitOnHelp,cmderr){
  parseCmdline();
}
bool DummyCmd::match(string const&cmd){
  return cmd=="dummy";
}
void DummyCmd::print(ostream&os)const{
}
void DummyCmd::addCmdlineOptionsAux(po::options_description&desc,po::positional_options_description&posdesc){
    desc.add_options()("dummy",po::value<string>(&dummy_),"dummy cmd line option (mandatory)");
}
void DummyCmd::parseCmdlineAux(po::variables_map const&vm){
// NOTE!    if(!vm.count("dummy"))cmderror("missing madatory 'dummy' cmd line parameter");
}
}
