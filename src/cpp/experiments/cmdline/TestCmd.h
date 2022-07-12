#pragma once
#include "general/cmdline/CmdBase.h"
#include <boost/program_options.hpp>
#include <string>
#include <iosfwd>
namespace po=boost::program_options;
namespace tcs{

// command that is a nop
class TestCmd:public CmdBase{
public:
  static const std::string CMDNAME;

  TestCmd(std::string const&progn,int argc,char*argv[],bool exitOnHelp,std::function<void(std::string const&)>cmderr,bool printOnPrint);
  [[nodiscard]]static bool match(std::string const&cmd);
  [[nodiscard]]static std::string cmdname();
  [[nodiscard]]static std::string cmddescr();

  [[nodiscard]]std::string const&testFile()const noexcept;
  [[nodiscard]]std::string const&testDir()const noexcept;
private:
  virtual void printAux(std::ostream&os)const override;
  virtual void addCmdlineOptionsAux(po::options_description&desc,po::positional_options_description&posdesc)override;
  virtual void parseCmdlineAux(po::variables_map const&vm)override;
  void twmnbmAux(std::set<std::string>&baseset,std::optional<std::string>const&lstcmd,std::optional<std::string>const&lstopt)const override;
private:
  std::string testFile_;
  std::string testDir_;
};
}
