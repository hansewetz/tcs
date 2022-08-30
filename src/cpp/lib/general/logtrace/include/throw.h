#pragma once
#include "general/logtrace/LogHeader.h"
#include "boost/stacktrace.hpp"
#include <string>
#include <stdexcept>
#include <iosfwd>
#include <sstream>


// install print operator for styd::exception in std namespace
// (we cannot have the operator in 'ef' namespace since we get clashes with boost print operators then)
namespace std{
// print operator
std::ostream&operator<<(std::ostream&os,std::exception const&e);
}

// open tcs namepace
namespace tcs{

// type of stack trace
using stacktrace_t=boost::stacktrace::stacktrace;

class texcept:public std::runtime_error{
public:
  // print operator - will print stacktrace also
  friend std::ostream&operator<<(std::ostream&os,texcept const&te);
  
  // ctor
  texcept(std::string const&msg,stacktrace_t const&trace,std::string const&file,int line);
  
  // getters
  [[nodiscard]]std::string const&msg()const noexcept;
  [[nodiscard]]stacktrace_t const&stacktrace()const;
  [[nodiscard]]std::size_t line()const noexcept;
  [[nodiscard]]std::string const&file()const noexcept;
  [[nodiscard]]std::string const&whatstr()const noexcept;

  // what() function
  [[nodiscard]]virtual char const*what()const noexcept;

  // get a texcept pointer (possibly nullptr) from an std::exception
  [[nodiscard]]static texcept const*e2texcept(exception const&e);
private:
  std::string msg_;
  stacktrace_t trace_;
  std::string file_;
  std::size_t line_;
  std::string whatstr_;
};
// throw 'texcept'
#define THROW_TEXCEPT(x){\
  std::stringstream strm;\
  strm<<"["<<LogHeader::instance().getlogheader()<<"]"<<": "<<x;\
  throw tcs::texcept(strm.str(),tcs::stacktrace_t(),__FILE__,__LINE__);\
}
// throw 'runtime_error' 
#define THROW(x){\
  std::stringstream strm;\
  strm<<"["<<LogHeader::instance().getlogheader()<<"]"<<__FILE__":#"<<__LINE__<<": "<<x;\
    std::string s{strm.str()};\
    throw std::runtime_error(s);\
}
}
