#include "apputils/logtrace/throw.h"
#include <sstream>
#include <iostream>

using namespace std;


// install print operator for styd::exception in std namespace
namespace std{
// print operator
ostream&operator<<(ostream&os,exception const&e){
  tcs::texcept const*te=tcs::texcept::e2texcept(e);
  if(te)return os<<*te;
  return os<<e.what();
}
}

namespace tcs{

// print operator - will print stacktrace also
ostream&operator<<(ostream&os,texcept const&te){
  os<<te.whatstr()<<endl;
  os<<te.stacktrace();
  return os;
}
// ctor
texcept::texcept(string const&msg,stacktrace_t const&trace,string const&file,int line):
    runtime_error(msg),
    msg_(msg),trace_(trace),file_(file),line_(line),
    whatstr_(file+"#"+to_string(line_)+": "+msg_){
}
// getters
string const&texcept::msg()const noexcept{return msg_;}
stacktrace_t const&texcept::stacktrace()const{return trace_;}
size_t texcept::line()const noexcept{return line_;}
string const&texcept::file()const noexcept{return file_;}
string const&texcept::whatstr()const noexcept{return whatstr_;}

// what() function
char const*texcept::what()const noexcept{return whatstr_.c_str();}

// get a texcept pointer (possibly nullptr) from an exception
texcept const*texcept::e2texcept(exception const&e){return dynamic_cast<texcept const*>(&e);}
}
