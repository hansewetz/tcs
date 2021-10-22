#pragma once
#include <concepts>
#include <type_traits>
#include <iostream>
#include <utility>
#include <tuple>
#include <cxxabi.h>
namespace tcs{

// --- concepts
// check if types in a type pack are all streamable
template <typename...Ts>
concept Streamable=requires(std::ostream&os,Ts...ts){
  {((os<<ts),...)};
};
// concept for iterable with begin()/end() methods
template<typename T>
concept IterableBE=requires(T t){
  // just make sure the following expression is valid (simple requirement)
  begin(t)!=end(t);
};

// --- make types readable
// get a type as a string
template<typename T>
[[nodiscard]]std::string type2string(T t){
  int status;
  return std::string("(")+abi::__cxa_demangle(typeid(T).name(),0,0,&status)+")";
}

// --- min/max functions with variable return type
template<typename T1,typename T2>
[[nodiscard]]auto tmax(T1 t1,T2 t2)->std::common_type_t<T1,T2>{
  return t1>t2?t1:t2;
}
template<typename T1,typename T2>
[[nodiscard]]auto tmin(T1 t1,T2 t2)->std::common_type_t<T1,T2>{
  return t1<t2?t1:t2;
}

// --- add elements to collections
// push elements on a collection
template<typename C,typename...Ts>
void push_vals(C&c,Ts&&...ts){
  (c.push_back(std::forward<Ts>(ts)),...);
}
}
