#pragma once
#include <boost/program_options.hpp>
#include <string>
#include <iosfwd>
#include <functional>
namespace po=boost::program_options;

namespace tcs{
class CmdBase{
public:
  // debug print operator
  friend std::ostream&operator<<(std::ostream&os,CmdBase const&p);

  // ctor
  CmdBase(std::string const&progn,int argc,char*argv[],bool exitOnHelp,std::function<void(std::string const&)>cmderr);
 
  // getters
  [[nodiscard]]int debug()const noexcept;
  [[nodiscard]]bool help()const noexcept;
  [[nodiscard]]bool noexec()const noexcept;
  [[nodiscard]]bool print()const noexcept;
  [[nodiscard]]std::string progn()const noexcept;
  [[nodiscard]]std::string cmd()const noexcept;
protected:
  // main alg parsing and evaluating cmd line parameters
  void parseCmdline();

  // pure virtual functions for cmd line parameters to be implemented in derived classes
  virtual void addCmdlineOptionsAux(po::options_description&desc,po::positional_options_description&posdesc)=0;
  virtual void parseCmdlineAux(po::variables_map const&vm)=0;

  // print usage info for this command
  // (uses 'desc_' and 'posdesc_' to extract usage info)
  void cmdusage(bool exitwhendone,std::string const&msg)const;
private:
  virtual void print(std::ostream&os)const=0;

  // program option related parameters
  std::string progn_;
  std::string cmd_;
  int argc_=0;
  char**argv_=nullptr;
  bool exitOnHelp_;
protected:
  std::function<void(std::string const&)>cmderr_;
private:
  // boost program option related parameters
  po::options_description desc_;
  po::positional_options_description posdesc_;

  // cmd line parameter supported by base class
  int debug_=2;
  bool help_=false;
  bool print_=false;
  bool noexec_=false;
};
}
