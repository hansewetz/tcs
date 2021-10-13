#include "utils.h"
#include <filesystem>
#include <optional>
#include <sstream>
namespace tcs{

// -- merge strings from a collection with separator in between
template<Streamable S,template<class>class C,Streamable T>
[[nodiscard]]std::string strcat(S const&sep,C<T>const&c){
  std::stringstream ret;
  for(auto it=begin(c);it!=end(c);++it){
    ret<<*it<<(next(it)!=end(c)?sep:S{});
  }
  return ret.str();
}
// -- merge streamable values into a string with separator
template<Streamable S,Streamable T,Streamable...Ts>
[[nodiscard]]std::string strcat(S const&s,T const&t,Ts const&...ts){
  std::stringstream ret;
  ret<<t,((ret<<s<<ts),...);
  return ret.str();
}
// --- split a string and return elements as a collection
// TODO
// split on a character
// split on space
// split on a string

// --- string to upper/lower
// TODO
[[nodiscard]]std::string toupper(std::string const&s);
[[nodiscard]]std::string tolower(std::string const&s);
[[nodiscard]]std::string toupperutf8(std::string const&rawstr);
[[nodiscard]]std::string tolowerutf8(std::string const&rawstr);

// read a file into a string
[[nodiscard]]std::string file2string(std::filesystem::path const&path);

// -- convert a string to a number
// TODO
[[nodiscard]]std::optional<long>string2long(std::string const&s);
[[nodiscard]]std::optional<unsigned long>string2ulong(std::string const&s);

}
