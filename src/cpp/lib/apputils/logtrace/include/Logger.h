#pragma once
#include <string>
#include <filesystem>
#include <optional>
#include <iostream>
#include <fstream>
namespace tcs{

// class used when initializing logging
class Logger{
public:
  // log levels
  enum class AppLogLevel{TRACE=0,DEBUG=1,NORMAL=2};

  // basic ctors
  // (errors always goes to error stream, other log messages goes to non-error stream)
  Logger();
  Logger(std::string const&logheader,AppLogLevel level);
  Logger(std::string const&logheader,AppLogLevel level,std::filesystem::path const&outpath,std::filesystem::path const&errpath);
  Logger(std::string const&logheader,AppLogLevel level,std::ostream&osout,std::ostream&oserr);

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
private:
  // some private attr
  std::string logheader_;
  AppLogLevel level_;

  // logfiles if we log to files
  std::optional<std::filesystem::path>outpath_;
  std::optional<std::filesystem::path>errpath_;
  
  // streams if we log to user supplied streams
  std::ostream*osout_;
  std::ostream*oserr_;

  // bool if looging to stdout/stderr
  bool log2std_;

  // streams for user supplied paths
  std::optional<std::ofstream>osoutpath_;
  std::optional<std::ofstream>oserrpath_;
};
}
