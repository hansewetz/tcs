#include "tcsvm/tcsvm-utils/Interpolator.h"
#include "general/logtrace/logtrace.h"
#include "general/sysutils/envutils.h"

#include <boost/log/trivial.hpp>
#include <boost/stacktrace.hpp>
#include <boost/exception/all.hpp>

#include <iostream>
#include <stdexcept>
using namespace std;
using namespace tcs;

// interpolator functions
optional<string>fenv(string const&s,string&err){
  auto ret=getenvvar(s,err);
  return ret;
}

// test program
int main(){
  // setup logger
  LogHeader::instance().setlogheader("test-app");
  Logger logger1(Logger::AppLogLevel::TRACE);
  logger1.activateStdlog();
  logger1.activatePathlog("junk.stdout","junk.stderr");
  try{

    // create interpolator
    vm::Interpolator ip(fenv,nullopt,nullopt,nullopt);

    // test interpolator
    string stest="a=${HELLO}";
    string itest=ip.interpolate(stest);
    cout<<"\""<<stest<<"\" --> \""<<itest<<"\""<<endl;
  }
  catch(exception const&e){
    BOOST_LOG_TRIVIAL(error)<<"caught exception: "<<e;
  }
}

/*std::optional<flookup_t>const&fenv,std::optional<flookup_t>const&fvar,
               std::optional<flookup_t>const&fcmd,std::optional<flookup_t>const&fcfg);
*/
