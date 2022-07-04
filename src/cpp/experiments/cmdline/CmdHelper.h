#pragma once
#include "general/utils/typeutils.h"
#include <boost/program_options.hpp>
#include <optional>
#include <string>
#include <functional>
#include <variant>
#include <iostream>
#include <optional>
#include <type_traits>
namespace po=boost::program_options;

namespace tcs{
// print usage info from boost::program_options::positional_options_description
// (and exit)
void cmdusage(bool exitwhendone,boost::program_options::options_description const&desc,boost::program_options::positional_options_description const&posdesc,
           std::string const&progname);
void cmdusage(bool exitwhendone,boost::program_options::options_description const&desc,boost::program_options::positional_options_description const&posdesc,
           std::string const&progname,std::optional<std::string>const&subcmd,std::optional<std::string>const&manpage);

// get sub command from a type list of commands
template<int I,typename ...Ts>
requires requires(Ts...ts,std::string const&cmd){
  (Ts::match(cmd),...);
}
std::optional<std::variant<Ts...>>getMatchedCmdObjectAux(std::string const&progname,int argc,char**argv,bool exitOnHelp,std::function<void(std::string const&)>cmderr){
  // check out of bounds - if so, we did not find sub command
  if constexpr(I>=sizeof...(Ts)){
    return std::nullopt;
  }else{
    using cmd_t=std::decay_t<NthTypeOf<I,Ts...>>;

    // check if currenty object in type list matches subcmd
    // (done by calling static function in command class)
    if(cmd_t::match(argv[0])){
      return cmd_t(progname,argc,argv,exitOnHelp,cmderr);
    }else{
      // increase index to check next element in type list
      return getMatchedCmdObjectAux<I+1,Ts...>(progname,argc,argv,exitOnHelp,cmderr);
    }
  }
}
template<typename ...Ts>
std::optional<std::variant<Ts...>>getMatchedCmdObject(Typelist<Ts...>types,std::string const&progname,int argc,char**argv,
                                  bool exitOnHelp,std::function<void(std::string const&)>cmderr){
  return getMatchedCmdObjectAux<0,Ts...>(progname,argc,argv,exitOnHelp,cmderr);
}
}
