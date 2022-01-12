#pragma once
#include "utils.h"
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <filesystem>
#include <optional>
#include <sstream>
#include <vector>
#include <algorithm>
namespace tcs{

// --- strcat(...) functions: merge values from collectino, tuple, cmd line parameters and insert separator between them
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
requires requires(S s,C<T>c,std::ostream&os){
  begin(c);
  os<<s;
  os<<*begin(c);
}

[[nodiscard]]std::string strcat(S const&sep,C<T>const&c){
  std::stringstream ret;
  ret<<"[";
  for(auto it=begin(c);it!=end(c);++it){
    ret<<*it;
    if(next(it)!=end(c))ret<<sep;
  }
  ret<<"]";
  return ret.str();
}
// merge streamable values from a pack into a string with separator between values
template<Streamable S,Streamable T,Streamable...Ts>
[[nodiscard]]std::string strcat(S const&sep,T const&t,Ts const&...ts){
  std::stringstream ret;
  ret<<"[";
  ret<<t,((ret<<sep<<ts),...);
  ret<<"]";
  return ret.str();
}
// merge streamable tuple values into a string with separator between values
template<Streamable S,Streamable...Ts>
[[nodiscard]]std::string strcat(S const&sep,std::tuple<Ts...>const&tu){
  constexpr std::size_t LEN=sizeof...(Ts);               // get length of tuple
  std::stringstream ret;                                 // write tuple to a string stream
  std::string ssep=boost::lexical_cast<std::string>(sep);// convert sep to string
  std::string emptysep;                                  // get empty separator as a string
  ret<<"[";                                              // wrap output in brackets
  [&]<std::size_t...Is>(std::index_sequence<Is...>){     // lambda template writing tuple elements
    ((ret<<std::get<Is>(tu)<<(Is<(LEN-1)?ssep:emptysep)),...); // don't write separator after last element
  }(std::make_index_sequence<LEN>());                    // call lambda and pass an index sequence as parameter so we can walk through tuple elements
  ret<<"]";                                              // (wrap in bracket)
  return ret.str();                                      // get string from string stream
}
// merge streamable pair values into a string with separator between values
template<Streamable S,Streamable T1,Streamable T2>
[[nodiscard]]std::string strcat(S const&sep,std::pair<T1,T2>const&p){
  std::stringstream ret;                                 // write tuple to a string stream
  std::string ssep=boost::lexical_cast<std::string>(sep);// convert sep to string
  ret<<"["<<p.first<<ssep<<p.second<<"]";
  return ret.str();                                      // get string from string stream
}

// --- string split functions
// (by default, tokens are store in a vector<string>)
// (however, tokens can also be specified to be stored in a set<string> for example)

// split string on characters part of a 'split string'
template<typename C=std::vector<std::string>>
[[nodiscard]]auto splitstr(std::string const&str,std::string const&splitchars)->C{
  C ret;
  boost::split(ret,str,boost::is_any_of(splitchars));
  return ret;
}
// split string on a single character
template<typename C=std::vector<std::string>>
[[nodiscard]]auto splitstr(std::string const&str,char splitchar)->C{
  return splitstr<C>(str,std::string(1,splitchar));
}
// split string on blank space (0x20)
template<typename C=std::vector<std::string>>
[[nodiscard]]auto splitstr(std::string const&str)->C{
  return splitstr<C>(str,std::string(1,' '));
}

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
