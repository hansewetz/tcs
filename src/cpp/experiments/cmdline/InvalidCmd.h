#pragma once
#include "CmdBase.h"
#include <boost/program_options.hpp>
#include <string>
#include <iosfwd>
namespace po=boost::program_options;
namespace tcs{

// command that is a nop
class InvalidCmd{
public:
  friend std::ostream&operator<<(std::ostream&os,InvalidCmd const&c);

  InvalidCmd(std::string const&progn,int argc,char*argv[]);
  [[nodiscard]]static bool match(std::string const&cmd);
private:
  std::string progn_;
  int argc_;
  char**argv_;
};
}
