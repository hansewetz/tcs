#include "NopCmd.h"
#include "DummyCmd.h"
#include "InvalidCmd.h"
#include "CmdHelper.h"
#include "general/utils/utils.h"

using namespace std;
using namespace tcs;
namespace po=boost::program_options;


// --- (*) command processors
struct ProcInvalidCmd{
  void operator()(InvalidCmd const&cmd){cout<<"ProcInvalidCmd: "<<cmd<<endl;}
};
struct ProcNopCmd{
  void operator()(NopCmd const&cmd){cout<<"ProcNopCmd: "<<cmd<<endl;}
};
struct ProcDummyCmd{
  void operator()(DummyCmd const&cmd){cout<<"ProcDummyCmd: "<<cmd<<endl;}
};



// --- (*) test main program
int main(int argc,char*argv[]){
  try{
    // checks and adjust cmd line parameters
    if(argc<3)throw runtime_error("invalid #of cmd line parameters");
    string progname=argv[0];

    // error function for cmd line parsing
    auto cmderr=[](string const&msg){
      cerr<<"cmdlineError called: '"<<msg<<"' ... exiting ..."<<endl;
      exit(1);
    };
    // setup cmd objects
    using CmdTypes=Typelist<InvalidCmd,NopCmd,DummyCmd>;                        // types of cmd objects
    auto fprocs=overloaded{ProcInvalidCmd{},ProcNopCmd{},ProcDummyCmd{}};       // cmd processing function objects

    // get variant that includes the matching sub-cmd and process it
    auto cmdobj=getMatchedCmdObject(CmdTypes(),progname,argc-1,argv+1,true,cmderr);
    visit(fprocs,cmdobj);
  }
  catch(std::exception const&exc){
    cerr<<"cought exception: "<<exc.what()<<endl;
    exit(1);
  }
}
