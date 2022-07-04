#pragma once
#include <boost/type_index.hpp>
#include <string>
#include <type_traits>
#include <tuple>
#include <cxxabi.h>
namespace tcs{

// --- convert type to readable string
// get a type as a string
template<typename T>
[[nodiscard]]std::string type2string(){
  return boost::typeindex::type_id_with_cvr<T>().pretty_name();
}
// get a type of a parameter as a string
template<typename T>
[[nodiscard]]std::string type2string(T&&t){
  return type2string<decltype(t)>();
}
// template used to pack a number of function objects as base classes
template<typename...Ts>
struct overloaded:Ts...{using Ts::operator()...;};

// get Nth type from a type pack
template<int N, typename... Ts>
using NthTypeOf=typename std::tuple_element<N,std::tuple<Ts...>>::type;

// struct carrying a list of types
// (usefull when we want to pass type packs as parameters without constructing any objects which we would have to do if we used tuples)
template<typename...Ts>struct Typelist{};
}
