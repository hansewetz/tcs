#include "general/cmdline/CmdLine.h"
#include "general/cmdline/NopCmd.h"
#include "general/cmdline/DummyCmd.h"

using namespace std;
using namespace tcs;
namespace po=boost::program_options;


// --- command processors
struct ProcNopCmd{
  void operator()(NopCmd const&cmd){cout<<"executing ProcNopCmd: "<<cmd<<endl;}
};
struct ProcDummyCmd{
  void operator()(DummyCmd const&cmd){cout<<"executing ProcDummyCmd"<<endl;}
};


// --- test main program
int main(int argc,char*argv[]){
  try{
    // some config parameters
    bool printOnPrint=true;
    string version="v1.001";

    // setup cmd objects
    using CmdTypes=Typelist<NopCmd,DummyCmd>;                  // types of cmd objects
    auto fprocs=overloaded{ProcNopCmd{},ProcDummyCmd{}};       // cmd processing function objects
    CmdLine<decltype(fprocs),CmdTypes>cmdline(argc,argv,fprocs,version,printOnPrint);

    // parse cmd line and execute command
    cmdline.executeCmd();
  }
  catch(std::exception const&exc){
    cerr<<"cought exception: "<<exc.what()<<endl;
    exit(1);
  }
}
