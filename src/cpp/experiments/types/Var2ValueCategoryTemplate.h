#pragma once
#include "general/utils/typeutils.h"
#include <map>
#include <string>
#include <iosfwd>
#include <stdexcept>
#include <type_traits>

namespace tcs{

// value category classifier for checking what functions template see
class Var2ValueCategoryTemplate{
public:
  friend std::ostream&operator<<(std::ostream&os,Var2ValueCategoryTemplate const&vc);
  
  enum valcat_t{lvalue=0,xvalue=1,rvalue=2,prvalue=3};

  // ctod
  template<typename T>
  Var2ValueCategoryTemplate(T&&t):
      templateType_(tcs::type2string<T>()),
      paramType_(tcs::type2string(std::forward<decltype(t)>(t))){

    if(!std::is_reference_v<decltype((std::forward<T>(t)))>){
      valcat_=prvalue;
    }else
    if(std::is_lvalue_reference_v<decltype((std::forward<T>(t)))>){
      valcat_=lvalue;
    }else
    if(std::is_rvalue_reference_v<decltype((std::forward<T>(t)))>){
      valcat_=xvalue;
    }else
    if(!std::is_lvalue_reference_v<decltype((std::forward<T>(t)))>){
      valcat_=rvalue;
    }else{
      throw std::runtime_error("Var2ValueCategoryTemplate::Var2ValueCategoryTemplate: should never happend  ... invalid calue category");
    }
  }
  // get value category stored in this object
  [[nodiscard]]valcat_t operator()()const noexcept;
  
  // get attr as strings
  [[nodiscard]]std::string const&valcat2string()const;
  [[nodiscard]]std::string const&templateType()const noexcept;
  [[nodiscard]]std::string const&paramType()const noexcept;
private:
  valcat_t valcat_;             // value category for 't' to ctor
  std::string templateType_;    // type of T in template decl
  std::string paramType_;       // type of 't' to ctor

  // conversion from category to string
  static const std::map<valcat_t,std::string>valcat2string_;
};
}
