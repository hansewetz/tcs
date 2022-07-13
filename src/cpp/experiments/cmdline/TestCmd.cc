#include "TestCmd.h"
#include <general/utils/strutils.h>
#include <filesystem>
#include <iostream>
using namespace std;
namespace fs=std::filesystem;

namespace tcs{

// name of this sub command
const string TestCmd::CMDNAME="test";

// ctor
TestCmd::TestCmd(string const&progn,int argc,char*argv[],bool exitOnHelp,function<void(string const&)>cmderr,bool printOnPrint):
    CmdBase(progn,argc,argv,exitOnHelp,cmderr,printOnPrint){
  parseCmdline();

}
// check if command matches a passed command string
bool TestCmd::match(string const&cmd){
  return cmd==CMDNAME;
}
// get cmd name
string TestCmd::cmdname(){
  return CMDNAME;
}
// get description of sub command
string TestCmd::cmddescr(){
  return "the "s+CMDNAME+" is a test operation that is meant to be used for quick testing of commands";
}
// getters
string const&TestCmd::testFile()const noexcept{return testFile_;}
string const&TestCmd::testDir()const noexcept{return testDir_;}

// print cmd options
void TestCmd::printAux(ostream&os)const{
  os<<"testFile: "<<testFile_<<endl;
  os<<"testDir: "<<testDir_<<endl;
}
// setup accepted cmd line options
void TestCmd::addCmdlineOptionsAux(po::options_description&desc,po::positional_options_description&posdesc){
    desc.add_options()("testFile",po::value<string>(&testFile_),"filename (mandatory)");
    desc.add_options()("testDir",po::value<string>(&testDir_),"dirname (mandatory)");
}
// check/parse cmd line options
void TestCmd::parseCmdlineAux(po::variables_map const&vm){
  if(!vm.count("testFile"))cmderr_("missing mandatory 'testFile' cmd line parameter (must be a filename)");
  if(!vm.count("testDir"))cmderr_("missing mandatory 'testDir' cmd line parameter (must be a dircetory name)");
}
// calculate propsals for twmnbm
void TestCmd::twmnbmAux(set<string>&baseset,optional<string>const&lstcmd,optional<string>const&lstopt)const{
  if(twmnbmCheckCmdParamDirFile(true,"testFile",baseset,lstcmd,lstopt))return;
  if(twmnbmCheckCmdParamDirFile(false,"testDir",baseset,lstcmd,lstopt))return;
}
}
