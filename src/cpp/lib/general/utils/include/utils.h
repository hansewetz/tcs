#pragma once
#include "utils-concepts.h"
#include <concepts>
#include <type_traits>
#include <iostream>
#include <utility>
#include <tuple>
#include <cxxabi.h>
namespace tcs{

// --- convert type to readable string
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



// --- add (push) elements to collection
// 'push_vals' on collection by doing 'insert' if push_back not supported
template<typename C,typename ...Ts>
requires HasPushback<C,Ts...>
void push_vals(C&c,Ts const&...ts){
  (c.push_back(ts),...);
}
template<typename C,typename ...Ts>
requires HasInsert<C,Ts...>
void push_vals(C&c,Ts const&...ts){
  (c.insert(ts),...);
}
// push elements from one collection to another
template<typename C1,typename T,template<class>typename C2>
requires SupportsPushvals<C1,T>&&IsIterable2Type<C2<T>,T>
void push_vals(C1&c1,C2<T>const&c2){
  for(auto&&t:c2)push_vals(c1,t);
}
// push elements of a tuple on a collection
template<typename C,typename ...Ts>
requires SupportsPushvals<C,Ts...>
void push_vals(C&c,std::tuple<Ts...>const&tu){
  constexpr std::size_t LEN=sizeof...(Ts);
  auto fp=[&]<std::size_t...Is>(std::index_sequence<Is...>){
    (push_vals(c,std::get<Is>(tu)),...);
  };
  fp(std::make_index_sequence<LEN>());
}
}
