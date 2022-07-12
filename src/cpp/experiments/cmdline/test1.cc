#include "TestCmd.h"     
#include "ProcTestCmd.h"      

#include "general/cmdline/CmdLine.h"       // main cmndline class
#include "general/cmdline/NopCmd.h"        // cmdline args
#include "general/cmdline/DummyCmd.h"      // ...
#include "general/cmdline/ProcNopCmd.h"    // cmdline processors
#include "general/cmdline/ProcDummyCmd.h"  // ...

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
