#pragma once
#include "utils.h"
#include <filesystem>
#include <optional>
#include <sstream>
namespace tcs{

// --- strcat(...) functinos: merge values into a string where values are separated by a separator
// for example: 
//
//     auto tup=make_tuple("Hello"s,1,4.7,"Again");
//     cout<<strcat(", ",tup)<<endl;
//
//  The result is:
//
//     [Hello, 1, 4.7, Again]
// ---

// merge streamable values from a collection with separator in between elements
// (we don't want to use this function if C<T> is a string, instead we want the 'strcat(S, T, Ts ...)
template<Streamable S,Streamable T,template<class>class C>
[[nodiscard]]std::string strcat(S const&sep,C<T>const&c)requires(!std::is_same_v<C<T>,std::basic_string<T>>){
  std::stringstream ret;
  ret<<"[";
  for(auto it=begin(c);it!=end(c);++it){
    ret<<*it;
    if(next(it)!=end(c))ret<<sep;
  }
  ret<<"]";
  return ret.str();
}
// merge an empty list
template<Streamable S>
[[nodiscard]]std::string strcat(S const&sep){
  return "";
}
// merge streamable values into a string with separator between values
template<Streamable S,Streamable T,Streamable...Ts>
[[nodiscard]]std::string strcat(S const&sep,T const&t,Ts const&...ts){
  std::stringstream ret;
  ret<<t,((ret<<sep<<ts),...);
  return ret.str();
}
// merge streamable tuple values into a string with separator between values
template<Streamable S, typename TU,std::size_t...Is>
void strcattupaux(S const&sep,std::stringstream&str,TU const&tu,std::index_sequence<Is...>){
  str<<strcat(sep,get<Is>(tu)...);
}
template<Streamable S,Streamable...Ts>
[[nodiscard]]std::string strcat(S const&sep,std::tuple<Ts...>const&tu){
  std::stringstream ret;
  ret<<"[";
  strcattupaux(sep,ret,tu,std::make_index_sequence<sizeof...(Ts)>());
  ret<<"]";
  return ret.str();
}

// --- split functions
// split a string and return elements as a collection
// TODO
// split on a character
// split on space
// split on a string
// ---

// -- string conversions functions
// string to upper/lower
[[nodiscard]]std::string toupper(std::string const&s);
[[nodiscard]]std::string tolower(std::string const&s);
// TODO
[[nodiscard]]std::string toupperutf8(std::string const&rawstr);
[[nodiscard]]std::string tolowerutf8(std::string const&rawstr);
// convert a string to a number
// TODO
[[nodiscard]]std::optional<long>string2long(std::string const&s);
[[nodiscard]]std::optional<unsigned long>string2ulong(std::string const&s);
// ---

// --- string/file related functions
// read a file into a string
// TODO
[[nodiscard]]std::string file2string(std::filesystem::path const&path);
// ---


}
