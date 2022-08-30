#pragma once
#include <string>
#include <functional>
#include <optional>
namespace tcs::vm{

// class used to interpolate a string
//   (envvar - $xxx or ${xxx})
//   (memvar - %xxx or %{xxx})
//   (cmdvar - `xxx`)
//   (cfgvar - *xxx or *{xxx})
class Interpolator{
public:
  using flookup_t=std::function<std::optional<std::string>(std::string const&,std::string&err)>;

  // ctor
  Interpolator(std::optional<flookup_t>const&fenv,std::optional<flookup_t>const&fvar,
               std::optional<flookup_t>const&fcmd,std::optional<flookup_t>const&fcfg);

  // interpolate a string - return std::nullopt if failure
  // (if strd::nullopt is returned, an error message is stored in 'err')
  [[nodiscard]]std::optional<std::string>interpolate(std::string const&rawstr,std::string&err)const;

  // interpolate a string - throw an exception if failure
  [[nodiscard]]std::string interpolate(std::string const&rawstr)const;
private:
  std::optional<flookup_t>fenv_;    // $xxx  - environment variable
  std::optional<flookup_t>fvar_;    // %xxx  - memory variable
  std::optional<flookup_t>fcmd_;    // `xxx` - execute command
  std::optional<flookup_t>fcfg_;    // *xxx  - configuration variable

  // helpers
  std::optional<std::string>seterr(std::string&err,std::string const&errstr)const;                   // set an error string and return std::nullopt
  std::optional<std::string>optionalcall(std::optional<flookup_t>const&f,std::string const&s,std::string&err)const;  // call an optional function 
  bool valididc(bool allowdot,char c)const;                                                          // can 'c' be part of an identifier
};
}
