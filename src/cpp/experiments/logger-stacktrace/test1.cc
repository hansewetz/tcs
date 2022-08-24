#include "apputils/logtrace/LogHeader.h"
#include "apputils/logtrace/Logger.h"
#include "apputils/logtrace/throw.h"

#include <boost/log/trivial.hpp>

#include <boost/stacktrace.hpp>
#include <boost/exception/all.hpp>

#include <iostream>
#include <stdexcept>
using namespace std;
using namespace tcs;

// test throw function
void foo(){
  //throw_with_trace(runtime_error("thrown from foo() ..."));
  THROW_TEXCEPT("Hello: "<<17);
}

int main(){
  try{
    // setup log header string + define a few loggers
    LogHeader::instance().setlogheader("test-app");
  
    // setup logging on file + stdxxx
    Logger logger1(Logger::AppLogLevel::TRACE,"junk1.stdout","junk1.stderr");
    Logger logger2(Logger::AppLogLevel::TRACE,Logger::STDLOG);
    BOOST_LOG_TRIVIAL(debug)<<"will throw here ...";

    // call function that throws
    foo();
  }
// NOTE! should catch std::exception() and then having what() overriden virtually
  catch(exception const&e){
    //cerr<<e.what()<<endl;
    cerr<<e<<endl;
/*
    cerr<<e.what()<<endl;
    const boost::stacktrace::stacktrace*st=boost::get_error_info<traced>(e);
    if(st)cerr<<*st<<endl;
*/
  }
}
