#pragma once
#include <boost/log/core.hpp>
#include <boost/log/sinks.hpp>
#include <string>
#include <filesystem>
#include <optional>
#include <iostream>
#include <fstream>
namespace tcs{

// class used when initializing logging
class Logger{
public:
  // typedefs for sinks
  typedef boost::log::sinks::synchronous_sink<boost::log::sinks::text_ostream_backend> sink_t;
  using sink_ptr_t=boost::shared_ptr<Logger::sink_t>;

  // debug print of logger
  friend std::ostream&operator<<(std::ostream&os,Logger const&l);

  // log levels
  enum class AppLogLevel{TRACE=0,DEBUG=1,NORMAL=2};

  // tag used to indicate stdlog
  static constexpr struct STDLOG_{}STDLOG{};

  // basic ctors
  // (errors always goes to error stream, other log messages goes to non-error stream)
  Logger();                          // no logging
  Logger(STDLOG_);                   // stdlog
  Logger(AppLogLevel level);         // no logging
  Logger(AppLogLevel level,STDLOG_); // stdlog
  Logger(AppLogLevel level,          // path log
         std::filesystem::path const&outpath,std::filesystem::path const&errpath);
  Logger(AppLogLevel level,          // stream log
         std::ostream&osout,std::ostream&oserr);

  Logger(Logger const&)=delete;
  Logger(Logger&&)=delete;
  Logger&operator=(Logger const&)=delete;
  Logger&operator=(Logger&&)=delete;
  ~Logger();

  // activate logging parameters
  // (will add logging options unless the type of logging is already active)
  bool activateStdlog();
  bool activatePathlog(std::filesystem::path const&outpath,std::filesystem::path const&errpath);
  bool activateStreamlog(std::ostream&osout,std::ostream&oserr);

  // deactivate logging parameter
  void deactivateStdlog();
  // NOTE! not yet done

  // utility methods
  static std::string level2string(AppLogLevel level);
  std::string level2string()const;
private:

  // private ctor used by other ctors
  Logger(AppLogLevel level,
         bool log2std,
         std::ostream*osout,std::ostream*oserr,                    // points to something or are nullptr
         std::optional<std::filesystem::path>const&outpath,        // nulleable
         std::optional<std::filesystem::path>const&errpath);       // ...

  // private helpers
  bool activateStdlogAux();
  bool activatePathlogAux(std::optional<std::filesystem::path>const&outpath,std::optional<std::filesystem::path>const&errpath);
  bool activateStreamlogAux(std::ostream*osout,std::ostream*oserr);
  bool activateNoStdlogAux();

  // some private attr
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

  // active sinks
  // (we keep them so that we can deactivate when needed)
  sink_ptr_t stdoutsink_;
  sink_ptr_t stderrsink_;

  sink_ptr_t pathoutsink_;
  sink_ptr_t patherrsink_;

  sink_ptr_t streamoutsink_;
  sink_ptr_t streamerrsink_;
};
}
