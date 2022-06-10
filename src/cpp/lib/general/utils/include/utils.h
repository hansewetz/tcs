#pragma once
#include "utils-concepts.h"
#include <concepts>
#include <type_traits>
#include <iostream>
#include <utility>
#include <tuple>
namespace tcs{

// --- min/max functions with variable return type
// (also handling pointer values)
[[nodiscard]]auto tmax(auto t1,auto t2){
  return t1>t2?t1:t2;
}
// (constrain for pointer type)
[[nodiscard]]auto tmax(IsPointer auto t1,IsPointer auto t2)
requires std::three_way_comparable_with<decltype(*t1), decltype(*t2)>{
  return tmax(*t1,*t2);
}
[[nodiscard]]auto tmin(auto t1,auto t2){
  return t1<t2?t1:t2;
}
// (constrain for pointer type)
[[nodiscard]]auto tmin(IsPointer auto t1,IsPointer auto t2)
requires std::three_way_comparable_with<decltype(*t1), decltype(*t2)>{
  return tmin(*t1,*t2);
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
