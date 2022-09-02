#include "tcsvm/tcsvm-utils/Interpolator.h"
#include "general/logtrace/logtrace.h"
#include "general/sysutils/envutils.h"
#include "general/sysutils/executils.h"
#include "general/miscutils/StopWatch.h"

#include <boost/log/trivial.hpp>
#include <boost/stacktrace.hpp>
#include <boost/exception/all.hpp>

#include <iostream>
#include <stdexcept>
using namespace std;
using namespace tcs;

// interpolator functionsDget environment variable
// (envvar - $xxx or ${xxx})
string fenv(string const&s){
  auto ret=getenvvar(s);
  return ret;
}
// get memory variable
// (memvar - %xxx or %{xxx})
string fmem(string const&s){
  static map<string,string>mem{
    {"foo","src"},
    {"bar","include"}
  };
  if(decltype(begin(mem))it;(it=mem.find(s))!=end(mem)){
    return it->second;
  }
  THROW_TEXCEPT("no such memory variable: "<<s);
}
// get result of executing a cmd
// (cmdvar - `xxx`)
string fcmd(string const&s){
  return shellcmd(s);
}
// get a config variable
// (cfgvar - *xxx or *{xxx})
// NOTE! not yet done

// test program
int main(){
  // setup logger
  LogHeader::instance().setlogheader("test-app");
  Logger logger1(Logger::AppLogLevel::TRACE);
  logger1.activateStdlog();
  logger1.activatePathlog("junk.stdout","junk.stderr");
  try{
    // test stopwatch
    SteadyStopWatch sw;
    sw.click();

    // create interpolator
    vm::Interpolator ip(fenv,fmem,fcmd,nullopt);

    // test interpolator
    string stest="a=%foo";
    string itest=ip.interpolate(stest);
    BOOST_LOG_TRIVIAL(info)<<"\""<<stest<<"\" --> \""<<itest<<"\"";

    // get time from stopwatch
    sw.click();
    BOOST_LOG_TRIVIAL(info)<<"execution time: "<<sw.getElapsedTimeMs()<<"ms";
  }
  catch(exception const&e){
    BOOST_LOG_TRIVIAL(error)<<"caught exception: "<<e;
  }
}
