#include "TestCmd.h"     
#include "ProcTestCmd.h"      

#include "apputils/cmdline/CmdLine.h"       // main cmndline class
#include "apputils/cmdline/NopCmd.h"        // cmdline args
#include "apputils/cmdline/DummyCmd.h"      // ...
#include "apputils/cmdline/ProcNopCmd.h"    // cmdline processors
#include "apputils/cmdline/ProcDummyCmd.h"  // ...

using namespace std;
using namespace tcs;

// --- test main program
int main(int argc,char*argv[]){
  try{
    // some config parameters
    bool printOnPrint=true;
    string version="v1.001";

    // setup cmdline definition
    using CmdTypes=Typelist<
                        NopCmd,
                        DummyCmd,
                        TestCmd>;    
    auto fprocs=overloaded{
                        ProcNopCmd{},
                        ProcDummyCmd{},
                        ProcTestCmd{}}; 
    CmdLine<decltype(fprocs),CmdTypes>cmdline(argc,argv,fprocs,version,printOnPrint);

    // parse cmd line and execute command
    cmdline.executeCmd();
  }
  catch(std::exception const&exc){
    cerr<<"cought exception: "<<exc.what()<<endl;
    exit(1);
  }
}
