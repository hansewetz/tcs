#pragma once
#include <concepts>
#include <type_traits>
#include <utility>
#include <iostream>
namespace tcs{

// --- concepts
// check if types in a type pack are all streamable
template <typename...Ts>
concept Streamable=requires(std::ostream&os,Ts...ts){
  ((os<<ts),...);
};
// concept for iterable with begin()/end() methods
template<typename T>
concept IterableBE=requires(T t){
  // just make sure the following expression is valid (simple requirement)
  begin(t)!=end(t);
};
// check if class supports 'push_vals' for types Ts...
template<typename C,typename ...Ts>
concept HasPushback=requires(C&c,Ts const&...ts){
  (c.push_back(ts),...);
};
// check if class supports 'insert' for type T
template<typename C,typename ...Ts>
concept HasInsert=requires(C&c,Ts const&...ts){
  (c.insert(ts),...);
};
// check that we can push value(s) on a collection
template<typename C,typename...Ts>
concept SupportsPushvals=HasPushback<C,Ts...>||HasInsert<C,Ts...>;

// check that collectikon can be iterated through
template<typename C,typename T>
concept IsIterable2Type=requires(C c){
  std::convertible_to <decltype(*c.begin()),T>;
};
}
