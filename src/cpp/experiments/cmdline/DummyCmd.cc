#include "DummyCmd.h"
#include <general/utils/strutils.h>
#include <filesystem>
#include <iostream>
using namespace std;
namespace fs=std::filesystem;

namespace tcs{

const string DummyCmd::CMDNAME="dummy";

DummyCmd::DummyCmd(string const&progn,int argc,char*argv[],bool exitOnHelp,function<void(string const&)>cmderr,bool printOnPrint):
    CmdBase(progn,argc,argv,exitOnHelp,cmderr,printOnPrint){
  parseCmdline();

}
bool DummyCmd::match(string const&cmd){
  return cmd==CMDNAME;
}
string DummyCmd::cmdname(){
  return CMDNAME;
}
string DummyCmd::cmddescr(){
  return "the "s+CMDNAME+" is a dummy operation that is meant to be used for quick testing of commands";
}
// getters
string const&DummyCmd::dummyFile()const noexcept{return dummyFile_;}
string const&DummyCmd::dummyDir()const noexcept{return dummyDir_;}

void DummyCmd::printAux(ostream&os)const{
  os<<"dummyFile: "<<dummyFile_<<endl;
  os<<"dummyDir: "<<dummyDir_<<endl;
}
void DummyCmd::addCmdlineOptionsAux(po::options_description&desc,po::positional_options_description&posdesc){
    desc.add_options()("dummyFile",po::value<string>(&dummyFile_),"filename (mandatory)");
    desc.add_options()("dummyDir",po::value<string>(&dummyDir_),"dirname (mandatory)");
}
void DummyCmd::parseCmdlineAux(po::variables_map const&vm){
  if(!vm.count("dummyFile"))cmderr_("missing mandatory 'dummyFile' cmd line parameter (must be a filename)");
  if(!vm.count("dummyDir"))cmderr_("missing mandatory 'dummyDir' cmd line parameter (must be a dircetory name)");
}
// calculate propsals for twmnbm
void DummyCmd::twmnbmAux(set<string>&baseset,optional<string>const&lstcmd,optional<string>const&lstopt)const{
  if(twmnbmCheckCmdParamDirFile(true,"--dummyFile",baseset,lstcmd,lstopt))return;
  if(twmnbmCheckCmdParamDirFile(false,"--dummyDir",baseset,lstcmd,lstopt))return;
}
}
