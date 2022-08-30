#include "general/logtrace/logtrace.h"

#include <boost/log/trivial.hpp>
#include <boost/stacktrace.hpp>
#include <boost/exception/all.hpp>

#include <iostream>
#include <stdexcept>
using namespace std;
using namespace tcs;

// test throw function
void foo(){
  BOOST_LOG_TRIVIAL(debug)<<"foo: throwing ...";
  THROW_TEXCEPT("Hello: "<<17);
}

// test program
int main(){
  try{
    // setup log header string + define a few loggers
    LogHeader::instance().setlogheader("test-app");
    Logger logger1(Logger::AppLogLevel::TRACE,"junk1.stdout","junk1.stderr");
    Logger logger2(Logger::AppLogLevel::TRACE,Logger::STDLOG);

    // call function that throws
    foo();
  }
  catch(exception const&e){
    cerr<<"caught exception: "<<e<<endl;
  }
}
