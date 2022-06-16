#pragma once
#include "CmdBase.h"
#include <boost/program_options.hpp>
#include <string>
#include <iosfwd>
namespace po=boost::program_options;
namespace tcs{

// command that is a nop
class DummyCmd:public CmdBase{
public:
  DummyCmd(std::string const&progn,int argc,char*argv[],bool exitOnHelp,std::function<void(std::string const&)>cmderr);
  [[nodiscard]]static bool match(std::string const&cmd);
private:
  virtual void print(std::ostream&os)const override;
  virtual void addCmdlineOptionsAux(po::options_description&desc,po::positional_options_description&posdesc)override;
  virtual void parseCmdlineAux(po::variables_map const&vm)override;
  void twmnbmAux(std::set<std::string>&baseset,std::optional<std::string>const&lstcmd,std::optional<std::string>const&lstopt)const override;
private:
  std::string dummy_;
};
}
