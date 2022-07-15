#include "apputils/logtrace/Logger.h"
#include <boost/log/trivial.hpp>
#include <boost/log/core.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/attributes/current_thread_id.hpp>
#include <boost/log/common.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/attributes.hpp>
#include <boost/core/null_deleter.hpp>
#include <iostream>
#include <fstream>

// NOTE!
// we should make Logger is a singleton ... or that we never declare more than one object of this type

using namespace std;
namespace tcs{

// debug print of logger
ostream&operator<<(ostream&os,Logger const&l){
  // NOTE!
  return os;
}

// helper methods
namespace{

// setup a boost sink and attach an output stream based on log level
auto setupSink(boost::shared_ptr<boost::log::core>core,ostream&os,bool iserror,string const&logheader,Logger::AppLogLevel level){
  // sink to return
  typedef boost::log::sinks::synchronous_sink<boost::log::sinks::text_ostream_backend> sink_t;
  boost::shared_ptr<sink_t>ret_sink;

  // create sink and setup filter on sink
  boost::shared_ptr<boost::log::sinks::text_ostream_backend>backend=boost::make_shared<boost::log::sinks::text_ostream_backend>();
  backend->add_stream(boost::shared_ptr<ostream>(&os,boost::null_deleter()));
  backend->auto_flush(true);                        // enable auto-flushing after each log record written
  ret_sink=boost::make_shared<sink_t>(backend);     // (the backend requires synchronization in the frontend)

  // only display error type messages on this sink
  if(iserror){
    ret_sink->set_filter(boost::log::trivial::severity>boost::log::trivial::warning);
  }else{
    if(level==Logger::AppLogLevel::TRACE){
      ret_sink->set_filter(
          boost::log::trivial::severity>=boost::log::trivial::trace&&
          boost::log::trivial::severity<=boost::log::trivial::warning);
    }else
    if(level==Logger::AppLogLevel::DEBUG){
      ret_sink->set_filter(
          boost::log::trivial::severity>=boost::log::trivial::debug&&
          boost::log::trivial::severity<=boost::log::trivial::warning);
    }else{
      ret_sink->set_filter(
        boost::log::trivial::severity>=boost::log::trivial::info&&
        boost::log::trivial::severity<=boost::log::trivial::warning);
    }
  }
  // add some formatting stuff to core
  core->add_global_attribute("TimeStamp",boost::log::attributes::local_clock());
  core->add_global_attribute("ThreadID",boost::log::attributes::current_thread_id());

  // setup log format
  ret_sink->set_formatter(
      boost::log::expressions::format("["s+logheader+"]"+"[%1%] [%2%] [%3%] %4%")%
      boost::log::expressions::attr<boost::posix_time::ptime>("TimeStamp")%
      boost::log::expressions::attr<boost::log::attributes::current_thread_id::value_type>("ThreadID")%
      boost::log::expressions::attr<boost::log::trivial::severity_level>("Severity")%boost::log::expressions::smessage
  );
  // return sink
  return ret_sink;
}
}
// no logging
Logger::Logger():
    Logger(""s,AppLogLevel::NORMAL,false,nullptr,nullptr,std::nullopt,std::nullopt){
}
// stdlog
Logger::Logger(Logger::STDLOG):
    Logger(""s,AppLogLevel::NORMAL,true,nullptr,nullptr,std::nullopt,std::nullopt){
}
// no logging
Logger::Logger(string const&logheader,AppLogLevel level):
    Logger(logheader,level,false,nullptr,nullptr,std::nullopt,std::nullopt){
}
// initialize boost logging with errors and non-errors going stderr
Logger::Logger(string const&logheader,Logger::AppLogLevel level,Logger::STDLOG):
    Logger(logheader,level,true,nullptr,nullptr,std::nullopt,std::nullopt){
}
// use specific ostreams
Logger::Logger(string const&logheader,Logger::AppLogLevel level,ostream&osout,ostream&oserr):
    Logger(logheader,level,false,&osout,&oserr,std::nullopt,std::nullopt){
}
// use filenames for err and non-err messages
Logger::Logger(string const&logheader,Logger::AppLogLevel level,filesystem::path const&outpath,filesystem::path const&errpath):
    Logger(logheader,level,false,nullptr,nullptr,outpath,errpath){
}
Logger::Logger(string const&logheader,AppLogLevel level,
         bool log2std,
         ostream*osout,ostream*oserr,
         optional<filesystem::path>const&outpath,
         optional<filesystem::path>const&errpath):
    logheader_(logheader),level_(level),
    log2std_(log2std),
    osout_(osout),oserr_(oserr),
    outpath_(outpath),errpath_(errpath),
    core_(boost::log::core::get()){

  // check if we log to std streams
  if(log2std_){
    activateStdlogAux();
  }
  // check if we have files names
  activatePathlogAux(outpath_,errpath_);

  // check if we have user specific output streams
  activateStreamlogAux(osout,oserr);
}
// modify logging
// (will add logging options unless the type of logging is already active)
bool Logger::activateStdlog(){
  if(log2std_)return false;
  return activateStdlogAux();
}
bool Logger::activatePathlog(std::filesystem::path const&outpath,std::filesystem::path const&errpath){
  if(outpath_||errpath_)return false;
  return activatePathlogAux(outpath,errpath);
}
bool Logger::activateStreamlog(std::ostream&osout,std::ostream&oserr){
  if(osout_||oserr_)return false;
  return activateStreamlogAux(&osout,&oserr);
}
// Aux functions blindly actvates if parameters allow it - it's up to caller to make sure activation should occur
bool Logger::activateStdlogAux(){
  core_->add_sink(setupSink(core_,cout,false,logheader_,level_));
  core_->add_sink(setupSink(core_,cerr,true,logheader_,level_));
  log2std_=true;
  return true;
}
bool Logger::activatePathlogAux(optional<filesystem::path>const&outpath,optional<filesystem::path>const&errpath){
  bool ret=false;

  // non-errors
  if(outpath){
    outpath_=outpath;
    osoutpath_=ofstream(outpath_.value().string());
    if(!osoutpath_.value())throw runtime_error("initLogging: failed opening file: "s+outpath_.value().string());
    core_->add_sink(setupSink(core_,osoutpath_.value(),false,logheader_,level_));
    ret=true;
  }
  // errors
  if(errpath){
    errpath_=errpath;
    oserrpath_=ofstream(errpath_.value().string());
    if(!oserrpath_.value())throw runtime_error("initLogging: failed opening file: "s+errpath_.value().string());
    core_->add_sink(setupSink(core_,oserrpath_.value(),true,logheader_,level_));
    ret=true;
  }
  return ret;
}
bool Logger::activateStreamlogAux(ostream*osout,ostream*oserr){
  bool ret=false;
  if(osout){
    core_->add_sink(setupSink(core_,*osout,false,logheader_,level_));
    osout_=osout;
    ret=true;
  }
  if(oserr){
    core_->add_sink(setupSink(core_,*oserr,true,logheader_,level_));
    oserr_=oserr;
    ret=true;
  }
  return ret;
}
}
