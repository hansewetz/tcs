#pragma once
#include "general/utils/typeutils.h"
#include <boost/program_options.hpp>
#include <boost/lexical_cast.hpp>
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
std::optional<std::variant<Ts...>>getMatchedCmdObjectAux(std::string const&progname,int argc,char**argv,bool exitOnHelp,std::function<void(std::string const&)>cmderr,bool printOnPrint){
  // check out of bounds - if so, we did not find sub command
  if constexpr(I>=sizeof...(Ts)){
    return std::nullopt;
  }else{
    using cmd_t=std::decay_t<NthTypeOf<I,Ts...>>;

    // check if currenty object in type list matches subcmd
    // (done by calling static function in command class)
    if(cmd_t::match(argv[0])){
      return cmd_t(progname,argc,argv,exitOnHelp,cmderr,printOnPrint);
    }else{
      // increase index to check next element in type list
      return getMatchedCmdObjectAux<I+1,Ts...>(progname,argc,argv,exitOnHelp,cmderr,printOnPrint);
    }
  }
}
template<typename ...Ts>
std::optional<std::variant<Ts...>>getMatchedCmdObject(Typelist<Ts...>types,std::string const&progname,int argc,char**argv,
                                  bool exitOnHelp,std::function<void(std::string const&)>cmderr,
                                  bool printOnPrint){
  return getMatchedCmdObjectAux<0,Ts...>(progname,argc,argv,exitOnHelp,cmderr,printOnPrint);
}
// check and get a '--cmd <val>' from a command line where type of 'val' is T
// (if the cmd line parameter is not part of the cmd line, a std::nullopt is returned)
// (if it is part of the cmd line, it is checked that there is a value after the command that can be converted to a T type object)
// (if an error occurs, the function 'cmderr' is called)
// (typically the 'cmderr' function would print a messsage and exit.)
// (if it doe snot exit, a std::nullopt is returned)
template<typename T>
std::optional<T>checkAndGetCmdlineParam(int argc,char**argv,std::string const&cmd,std::function<void(std::string const&)>cmderr){
  auto it=std::find_if(&argv[1],&argv[argc],[cmd](char const*s){return strcmp(s,cmd.data())==0;});
  // check that we have cmd + val as part of the cmd line
  if(it!=&argv[argc]){
    if(it==&argv[argc-1])cmderr(cmd+" requires a parameter");

    // try to convert the parameter to 'cmd' from a string to a T
    T val;
    if(boost::conversion::try_lexical_convert(std::string(it[1]),val)){
      return val;
    }else{
      cmderr(cmd+" requires a parameter of type: "+type2string<T>());
      return std::nullopt;
    }
  }
  return std::nullopt;
}
}
