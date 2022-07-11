#pragma once
#include <boost/program_options.hpp>
#include <string>
#include <vector>
#include <set>
#include <optional>
#include <iosfwd>
#include <functional>
#include <memory>
namespace po=boost::program_options;

namespace tcs{
class CmdBase{
public:
  // debug print operator
  friend std::ostream&operator<<(std::ostream&os,CmdBase const&p);

  // ctor
  CmdBase(std::string const&progn,int argc,char*argv[],bool exitOnHelp,std::function<void(std::string const&)>cmderr,bool printOnPrint);
 
  // getters
  [[nodiscard]]int debug()const noexcept;
  [[nodiscard]]bool help()const noexcept;
  [[nodiscard]]bool noexec()const noexcept;
  [[nodiscard]]bool print()const noexcept;
  [[nodiscard]]bool istwmnbm()const noexcept;
  [[nodiscard]]std::string const&progn()const noexcept;
  [[nodiscard]]std::string const&cmd()const noexcept;
  [[nodiscard]]std::vector<std::string>twmnbm()const;
protected:
  // main alg parsing and evaluating cmd line parameters
  void parseCmdline();

  // pure virtual functions for cmd line parameters to be implemented in derived classes
  virtual void addCmdlineOptionsAux(po::options_description&desc,po::positional_options_description&posdesc)=0;
  virtual void parseCmdlineAux(po::variables_map const&vm)=0;

  // print usage info for this command
  // (uses 'desc_' and 'posdesc_' to extract usage info)
  void cmdusage(bool exitwhendone,std::string const&msg)const;
protected:
  virtual void printAux(std::ostream&os)const=0;
  virtual void twmnbmAux(std::set<std::string>&baseset,std::optional<std::string>const&lstcmd,std::optional<std::string>const&lstopt)const=0;

  // helper method used when calculating sets for twmnbm operations
  std::set<std::string>subfromargv(std::set<std::string>const&baseset1,std::set<std::string>const&baseset2)const;
  std::optional<std::string>lastcmd()const;
  std::optional<std::string>lastopt()const;
  [[nodiscard]]bool twmnbmCheckCmdParam(std::string const&cmdparam,std::string const&defparam,std::set<std::string>&baseset,
                           std::optional<std::string>const&lstcmd,std::optional<std::string>const&lstopt)const;
  [[nodiscard]]bool twmnbmCheckCmdParamDirFile(bool isfile,std::string const&cmdparam,std::set<std::string>&baseset,std::optional<std::string>const&lstcmd,std::optional<std::string>const&lstopt)const;
private:
  // program option related parameters
  std::string progn_;
  std::string cmd_;
  int argc_=0;
  char**argv_=nullptr;
  bool exitOnHelp_;
  bool printOnPrint_;
protected:
  std::function<void(std::string const&)>cmderr_;
private:
  // boost program option related parameters
  std::shared_ptr<po::options_description>desc_;  // shared_ptr<...> so that object can be assigned to
  po::positional_options_description posdesc_;

  // cmd line parameter supported by base class
  int debug_=2;
  bool help_=false;
  bool print_=false;
  bool noexec_=false;
  bool istwmnbm_=false;
  int twmnbmSpacesAtEnd_=-1;
};
}
