#pragma once
#include <boost/log/core.hpp>
#include <string>
#include <filesystem>
#include <optional>
#include <iostream>
#include <fstream>
namespace tcs{

// class used when initializing logging
class Logger{
public:
  // debug print of logger
  friend std::ostream&operator<<(std::ostream&os,Logger const&l);

  // log levels
  enum class AppLogLevel{TRACE=0,DEBUG=1,NORMAL=2};

  // tag used to indicate stdlog
  struct STDLOG{};

  // basic ctors
  // (errors always goes to error stream, other log messages goes to non-error stream)
  Logger();                                                      // no logging
  Logger(STDLOG);                                                // stdlog
  Logger(std::string const&logheader,AppLogLevel level);         // no logging
  Logger(std::string const&logheader,AppLogLevel level,STDLOG);  // stdlog
  Logger(std::string const&logheader,AppLogLevel level,          // path log
         std::filesystem::path const&outpath,std::filesystem::path const&errpath);
  Logger(std::string const&logheader,AppLogLevel level,          // stream log
         std::ostream&osout,std::ostream&oserr);

  Logger(Logger const&)=delete;
  Logger(Logger&&)=delete;
  Logger&operator=(Logger const&)=delete;
  Logger&operator=(Logger&&)=delete;

  // most generic ctor
  Logger(std::string const&logheader,AppLogLevel level,
         bool log2std,
         std::ostream*osout,std::ostream*oserr,                    // points to something or are nullptr
         std::optional<std::filesystem::path>const&outpath,        // nulleable
         std::optional<std::filesystem::path>const&errpath);       // ...

  // modify logging
  // (will add logging options unless the type of logging is already active)
  bool activateStdlog();
  bool activatePathlog(std::filesystem::path const&outpath,std::filesystem::path const&errpath);
  bool activateStreamlog(std::ostream&osout,std::ostream&oserr);
private:
  // private helpers
  bool activateStdlogAux();
  bool activatePathlogAux(std::optional<std::filesystem::path>const&outpath,std::optional<std::filesystem::path>const&errpath);
  bool activateStreamlogAux(std::ostream*osout,std::ostream*oserr);

  // some private attr
  std::string logheader_="";
  AppLogLevel level_=AppLogLevel::NORMAL;

  // logfiles if we log to files
  std::optional<std::filesystem::path>outpath_=std::nullopt;
  std::optional<std::filesystem::path>errpath_=std::nullopt;
  
  // streams if we log to user supplied streams
  std::ostream*osout_=nullptr;
  std::ostream*oserr_=nullptr;

  // bool if looging to stdout/stderr
  bool log2std_=false;

  // streams for user supplied paths
  std::optional<std::ofstream>osoutpath_=std::nullopt;
  std::optional<std::ofstream>oserrpath_=std::nullopt;

  // boost log core
  boost::shared_ptr<boost::log::core>core_;
};
}
