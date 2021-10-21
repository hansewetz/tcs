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

// --- stream operators
// stream a pair
/*
template<Streamable T1,Streamable T2>
std::ostream&operator<<(std::ostream&os,std::pair<T1,T2>const&p){
  return os<<"["<<p.first<<", "<<p.second<<"]";
}
*/
// stream a tuple
template<typename TU,std::size_t...Is>
void printtupaux(std::ostream&os,TU const&tu,std::index_sequence<Is...>){
  (...,(os<<(Is==0?"":", ")<<std::get<Is>(tu)));
}
template<Streamable...Ts>
std::ostream&operator<<(std::ostream&os,std::tuple<Ts...>const&tu){
  os<<"[";
  printtupaux(os,tu,std::make_index_sequence<sizeof...(Ts)>());
  os<<"]";
  return os;
}
/*
// stream a collection
template<Streamable T,template<class>class C>requires IterableBE<C<T>>
std::ostream&operator<<(std::ostream&os,C<T>const&c){
  os<<"[";
  for(auto it=begin(c);it!=end(c);++it){
    os<<*it;
    if(next(it)!=end(c))os<<", ";
  }
  return os<<"]";
}
*/
// stream operator for argument pack
// TODO

// --- add elements to collections
// push elements on a collection
template<typename C,typename...Ts>
void push_vals(C&c,Ts&&...ts){
  (c.push_back(std::forward<Ts>(ts)),...);
}
}
