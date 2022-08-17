#include "apputils/logtrace/LogHeader.h"
#include "apputils/logtrace/Logger.h"

#include <boost/log/trivial.hpp>

#include <boost/stacktrace.hpp>
#include <boost/exception/all.hpp>
#include <iostream>
#include <stdexcept>
using namespace std;
using namespace tcs;

//typedef boost::error_info<struct tag_stacktrace, boost::stacktrace::stacktrace>traced;
//typedef boost::error_info<struct tag_my_info,string> my_info; //(1)
//struct str_error:virtual boost::exception,virtual std::exception { }; //(2)

int main(){
  // setup log header string + define a few loggers
  LogHeader::instance().setlogheader("test-app");

  // setup logging on file + stdxxx
  Logger logger1(Logger::AppLogLevel::TRACE,"junk1.stdout","junk1.stderr");
  //Logger logger2(Logger::AppLogLevel::TRACE,Logger::STDLOG);
 
  BOOST_LOG_TRIVIAL(error)<<"logger1: error message outside scope ...";
  BOOST_LOG_TRIVIAL(info)<<"logger1: info message  outside scope...";
  BOOST_LOG_TRIVIAL(debug)<<"logger1: Debug log message  outside scope...";
  try{
    //throw str_error()<<my_info("Hello");
    //cerr<<boost::stacktrace::stacktrace();
  }
  catch(std::exception const&e){
    //cerr<<"err: "<<*boost::get_error_info<my_info>(e)<<endl;
  }
}
