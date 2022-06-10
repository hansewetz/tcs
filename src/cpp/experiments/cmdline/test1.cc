#include "CmdBase.h"
#include "NopCmd.h"
#include "DummyCmd.h"

#include "general/utils/strutils.h"
#include "general/utils/utils.h"
#include "general/utils/typeutils.h"

#include <boost/program_options.hpp>

#include <ranges>
#include <variant>
#include <type_traits>

using namespace std;
using namespace tcs;
namespace po=boost::program_options;



// --- (*) derived classes: cmd line parameters
class InvalidCmd:public CmdBase{
public:
  InvalidCmd(std::string const&progn,int argc,char*argv[],bool exitOnHelp,function<void(string const&)>cmderr):
      CmdBase(progn,argc,argv,exitOnHelp,cmderr){
  }
  [[nodiscard]]static bool match(std::string const&cmd){return false;}
private:
  virtual void print(ostream&os)const override{os<<"InvalidCmd";}
  virtual void addCmdlineOptionsAux(po::options_description&desc,po::positional_options_description&posdesc)override{
  }
  virtual void parseCmdlineAux(po::variables_map const&vm)override{
  }
};



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





// --- (*) get sub command from a type list of commands
template<int I,typename ...Ts>
requires requires(Ts...ts,string const&cmd){
  (Ts::match(cmd),...);
}
variant<Ts...>getMatchedCmdObjectAux(string const&progname,int argc,char**argv,bool exitOnHelp,function<void(string const&)>cmderr){
  // check out of bounds - if so, we did not find sub command
  if constexpr(I>=sizeof...(Ts)){
    return InvalidCmd(progname,argc,argv,exitOnHelp,cmderr);
  }else{
    using cmd_t=decay_t<NthTypeOf<I,Ts...>>;

    // check if currenty object in type list matches subcmd
    // (done by calling static function in command class)
    if(cmd_t::match(argv[0])){
      return cmd_t(progname,argc,argv,exitOnHelp,cmderr);
    }else{
      // increase index to check next element in type list
      return getMatchedCmdObjectAux<I+1,Ts...>(progname,argc,argv,exitOnHelp,cmderr);
    }
  }
}
template<typename ...Ts>
variant<Ts...>getMatchedCmdObject(Typelist<Ts...>types,string const&progname,int argc,char**argv,
                                  bool exitOnHelp,function<void(string const&)>cmderr){
  return getMatchedCmdObjectAux<0,Ts...>(progname,argc,argv,exitOnHelp,cmderr);
}






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
    using CmdTypes=Typelist<InvalidCmd,NopCmd,DummyCmd>;                        // types only
    auto fprocs=overloaded{ProcInvalidCmd{},ProcNopCmd{},ProcDummyCmd{}};       // function objects

    // get variant that includes the matching sub-cmd and process it
    bool exitOnHelp=true;
    auto cmdobj=getMatchedCmdObject(CmdTypes(),progname,argc-1,argv+1,exitOnHelp,cmderr);
    visit(fprocs,cmdobj);
  }
  catch(std::exception const&exc){
    cerr<<"cought exception: "<<exc.what()<<endl;
    exit(1);
  }
}
