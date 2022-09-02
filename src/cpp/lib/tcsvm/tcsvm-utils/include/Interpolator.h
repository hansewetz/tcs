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
  using flookup_t=std::function<std::string(std::string const&)>;

  // ctor
  Interpolator(std::optional<flookup_t>const&fenv,std::optional<flookup_t>const&fmem,
               std::optional<flookup_t>const&fcmd,std::optional<flookup_t>const&fcfg);

  // interpolate a string - throw an exception if failure
  [[nodiscard]]std::string interpolate(std::string const&rawstr)const;
private:
  // the 'fxxx' functions must throw if an error occurs
  std::optional<flookup_t>fenv_;    // $xxx  - environment variable
  std::optional<flookup_t>fmem_;    // %xxx  - memory variable
  std::optional<flookup_t>fcmd_;    // `xxx` - execute command
  std::optional<flookup_t>fcfg_;    // *xxx  - configuration variable

  // helpers
  std::string optionalcall(std::optional<flookup_t>const&f,std::string const&s)const;  // call an optional function 
  bool valididc(bool allowdot,char c)const;                                            // can 'c' be part of an identifier
};
}
