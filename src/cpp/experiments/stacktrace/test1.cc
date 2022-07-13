#include <boost/stacktrace.hpp>
#include <boost/exception/all.hpp>
#include <iostream>
#include <stdexcept>
using namespace std;

//typedef boost::error_info<struct tag_stacktrace, boost::stacktrace::stacktrace>traced;
//typedef boost::error_info<struct tag_my_info,string> my_info; //(1)
//struct str_error:virtual boost::exception,virtual std::exception { }; //(2)

int main(){
  try{
    //throw str_error()<<my_info("Hello");
    cerr<<boost::stacktrace::stacktrace();
  }
  catch(std::exception const&e){
    //cerr<<"err: "<<*boost::get_error_info<my_info>(e)<<endl;
  }
}
