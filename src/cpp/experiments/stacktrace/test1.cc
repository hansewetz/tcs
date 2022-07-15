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
  Logger logger1;
  Logger logger2("test-app",Logger::AppLogLevel::TRACE,"junk.stdout","junk.stderr");

  BOOST_LOG_TRIVIAL(error)<<"Error log message ...";
  BOOST_LOG_TRIVIAL(info)<<"Info log message ...";
  try{
    //throw str_error()<<my_info("Hello");
    //cerr<<boost::stacktrace::stacktrace();
  }
  catch(std::exception const&e){
    //cerr<<"err: "<<*boost::get_error_info<my_info>(e)<<endl;
  }
}
