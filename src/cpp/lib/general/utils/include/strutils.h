#pragma once
#include "utils.h"
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <filesystem>
#include <optional>
#include <sstream>
#include <vector>
#include <algorithm>
#include <ranges>
namespace tcs{

// --- make some types streamable 
// (so that we can do 'strcat' of data structures containing those types)

// make 'pair' streamable
template<Streamable S1,Streamable S2>
std::ostream&operator<<(std::ostream&os,std::pair<S1,S2>const&p);

// --- strcat(...) functions: merge values from collection, tuple, cmd line parameters and insert separator between them
//     the values merged into the string must be streamable - i.e. concept: Streamable
// for example: 
//
//     auto tup=make_tuple("Hello"s,1,4.7,"Again");
//     cout<<strcat(", ",tup)<<endl;
//
//  The result is:
//
//     [Hello, 1, 4.7, Again]
// ---

// merge a single (or no)_ value from an optional parameter into a string
// (note: the 'sep' parameter is never used since an 'optional' (collection) has at most one element)
template<Streamable S,Streamable T>
[[nodiscard]]std::string strcat(S const&sep,std::optional<T>const&p){
  std::stringstream ret;
  if(p)ret<<p.value();
  return ret.str();
}
// merge streamable values from start iterator to end iterator separator in between elements
template<Streamable S,typename ITSTART,typename ITEND>
requires requires(S s,ITSTART itstart,ITEND itend,std::ostream&os){
  os<<s;
  os<<*itstart;
  itstart!=itend;
}
[[nodiscard]]std::string strcat(S const&sep,ITSTART itstart,ITEND itend){ 
  std::stringstream ret;
  for(auto it=itstart;it!=itend;){
    ret<<*it;
    if(++it!=itend)ret<<sep;
  }
  return ret.str();
}
// merge streamable values from a collection with separator in between elements
// (we don't want to use this function if C<T> is a string, instead we want the 'strcat(S, T, Ts ...)
template<Streamable S,Streamable T,template<class>class C>
requires requires(S s,C<T>c,std::ostream&os){
  begin(c);
  os<<s;
  os<<*begin(c);
  begin(c)!=end(c);
}
[[nodiscard]]std::string strcat(S const&sep,C<T>const&c){
  return strcat(sep,begin(c),end(c));
}
// merge streamable values from a ranges view with separator in between elements
template<Streamable S,std::ranges::view V>
requires requires(V v,std::ostream&os){
  os<<*v.begin();
}
[[nodiscard]]std::string strcat(S const&sep,V const&v){
  return strcat(sep,v.begin(),v.end());
}
// merge streamable values from a pack into a string with separator between values
template<Streamable S,Streamable T,Streamable...Ts>
[[nodiscard]]std::string strcat(S const&sep,T const&t,Ts const&...ts){
  std::stringstream ret;
  ret<<t,((ret<<sep<<ts),...);
  return ret.str();
}
// merge streamable tuple values into a string with separator between values
template<Streamable S,Streamable...Ts>
[[nodiscard]]std::string strcat(S const&sep,std::tuple<Ts...>const&tu){
  constexpr std::size_t LEN=sizeof...(Ts);               // get length of tuple
  std::stringstream ret;                                 // write tuple to a string stream
  std::string ssep=boost::lexical_cast<std::string>(sep);// convert sep to string
  std::string emptysep;                                  // get empty separator as a string
  [&]<std::size_t...Is>(std::index_sequence<Is...>){     // lambda template writing tuple elements
    ((ret<<std::get<Is>(tu)<<(Is<(LEN-1)?ssep:emptysep)),...); // don't write separator after last element
  }(std::make_index_sequence<LEN>());                    // call lambda and pass an index sequence as parameter so we can walk through tuple elements
  return ret.str();                                      // get string from string stream
}
// merge streamable pair values into a string with separator between values
template<Streamable S,Streamable T1,Streamable T2>
[[nodiscard]]std::string strcat(S const&sep,std::pair<T1,T2>const&p){
  std::stringstream ret;                                 // write tuple to a string stream
  std::string ssep=boost::lexical_cast<std::string>(sep);// convert sep to string
  ret<<p.first<<ssep<<p.second;
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
//[[nodiscard]]std::string toupperutf8(std::string const&rawstr);
//[[nodiscard]]std::string tolowerutf8(std::string const&rawstr);

// convert a string to a number
// (return nullopt oif conversion cannot be done)
[[nodiscard]]std::optional<long>string2long(std::string const&s);
[[nodiscard]]std::optional<unsigned long>string2ulong(std::string const&s);

// --- string/file related functions
// read a file into a string
// TODO
//[[nodiscard]]std::string file2string(std::filesystem::path const&path);
// ---





// --- implementation: make some types streamable 
// (so that we can do 'strcat' of data structures containing those types)

// make 'pair' streamable
template<Streamable S1,Streamable S2>
std::ostream&operator<<(std::ostream&os,std::pair<S1,S2>const&p){
  // NOTE! this is a hack since we hardcode separator and brackets
  return os<<"["<<strcat(",",p)<<"]";
}
}
