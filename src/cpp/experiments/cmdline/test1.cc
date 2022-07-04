#include "CmdLine.h"

#include "NopCmd.h"
#include "DummyCmd.h"

#include "CmdHelper.h"

using namespace std;
using namespace tcs;
namespace po=boost::program_options;


// --- (*) command processors
struct ProcNopCmd{
  void operator()(NopCmd const&cmd){cout<<"executing ProcNopCmd: "<<cmd<<endl;}
};
struct ProcDummyCmd{
  void operator()(DummyCmd const&cmd){cout<<"executing ProcDummyCmd: "<<cmd<<endl;}
};


// --- (*) test main program
int main(int argc,char*argv[]){
  try{
    // setup cmd objects
    using CmdTypes=Typelist<NopCmd,DummyCmd>;                  // types of cmd objects
    auto fprocs=overloaded{ProcNopCmd{},ProcDummyCmd{}};       // cmd processing function objects
    string version="v1.001";
    CmdLine<decltype(fprocs),CmdTypes>cmdline(argc,argv,fprocs,version);

    // parse cmd line and execute command
    cmdline.executeCmd();
  }
  catch(std::exception const&exc){
    cerr<<"cought exception: "<<exc.what()<<endl;
    exit(1);
  }
}
