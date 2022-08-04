#include "apputils/logtrace/Logger.h"
#include "apputils/logtrace/LogHeader.h"
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

using namespace std;
namespace tcs{

// debug print of logger
ostream&operator<<(ostream&os,Logger const&l){
  os<<"outpath: "<<(l.outpath_?l.outpath_.value().string():"<nullopt>"s)<<", ";
  os<<"errpath: "<<(l.errpath_?l.errpath_.value().string():"<nullopt>"s)<<", ";
  os<<"osout: "<<(l.osout_?"<stream>"s:"<null>"s)<<", ";
  os<<"oserr: "<<(l.oserr_?"<stream>"s:"<null>"s)<<", ";
  os<<"log2std: "<<boolalpha<<l.log2std_;
  return os;
}
// helper methods
namespace{

// setup a boost sink and attach an output stream based on log level
[[nodiscard]]auto setupSink(bool nulllog,boost::shared_ptr<boost::log::core>core,ostream&os,bool iserror,Logger::AppLogLevel level){
  // boost log sink
  Logger::sink_ptr_t ret_sink;

  // create sink and setup filter on sink
  boost::shared_ptr<boost::log::sinks::text_ostream_backend>backend=boost::make_shared<boost::log::sinks::text_ostream_backend>();
  if(!nulllog){
    backend->add_stream(boost::shared_ptr<ostream>(&os,boost::null_deleter()));
  }
  backend->auto_flush(true);                        // enable auto-flushing after each log record written
  ret_sink=boost::make_shared<Logger::sink_t>(backend);     // (the backend requires synchronization in the frontend)

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
  auto const&logheader=LogHeader::instance().getlogheader();
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
    Logger(AppLogLevel::NORMAL,false,nullptr,nullptr,std::nullopt,std::nullopt){
}
// stdlog
Logger::Logger(Logger::STDLOG_):
    Logger(AppLogLevel::NORMAL,true,nullptr,nullptr,std::nullopt,std::nullopt){
}
// no logging
Logger::Logger(AppLogLevel level):
    Logger(level,false,nullptr,nullptr,std::nullopt,std::nullopt){
}
// initialize boost logging with errors and non-errors going stderr
Logger::Logger(Logger::AppLogLevel level,Logger::STDLOG_):
    Logger(level,true,nullptr,nullptr,std::nullopt,std::nullopt){
}
// use specific ostreams
Logger::Logger(Logger::AppLogLevel level,ostream&osout,ostream&oserr):
    Logger(level,false,&osout,&oserr,std::nullopt,std::nullopt){
}
// use filenames for err and non-err messages
Logger::Logger(Logger::AppLogLevel level,filesystem::path const&outpath,filesystem::path const&errpath):
    Logger(level,false,nullptr,nullptr,outpath,errpath){
}
Logger::Logger(AppLogLevel level,
         bool log2std,
         ostream*osout,ostream*oserr,
         optional<filesystem::path>const&outpath,
         optional<filesystem::path>const&errpath):
    level_(level),
    log2std_(log2std),
    osout_(osout),oserr_(oserr),
    outpath_(outpath),errpath_(errpath),
    core_(boost::log::core::get()){

  // check if we should have no logging
  if(!log2std_&&!outpath_&&!errpath_&&!osout&&!oserr){
    activateNoStdlogAux();
  }
  // check if we log to std streams
  if(log2std_){
    activateStdlogAux();
  }
  // check if we have files names
  activatePathlogAux(outpath_,errpath_);

  // check if we have user specific output streams
  activateStreamlogAux(osout,oserr);
}
// dtor
Logger::~Logger(){
  deactivateStdlog();
  // NOTE!
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
  // even if atd sinks are not active, we need to have a sink without ouput or, the output will be generated by default)
  if(!log2std_){
    if(stdoutsink_){
      core_->remove_sink(stdoutsink_);
      stdoutsink_.reset();
    }
    if(stderrsink_){
      core_->remove_sink(stderrsink_);
      stderrsink_.reset();
    }
  }
  if(!stdoutsink_)core_->add_sink(stdoutsink_=setupSink(false,core_,cout,false,level_));
  if(!stderrsink_)core_->add_sink(stderrsink_=setupSink(false,core_,cerr,true,level_));
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
    if(pathoutsink_)core_->add_sink(pathoutsink_=setupSink(false,core_,osoutpath_.value(),false,level_));
    ret=true;
  }
  // errors
  if(errpath){
    errpath_=errpath;
    oserrpath_=ofstream(errpath_.value().string());
    if(!oserrpath_.value())throw runtime_error("initLogging: failed opening file: "s+errpath_.value().string());
    if(!patherrsink_)core_->add_sink(patherrsink_=setupSink(false,core_,oserrpath_.value(),true,level_));
    ret=true;
  }
  return ret;
}
bool Logger::activateStreamlogAux(ostream*osout,ostream*oserr){
  bool ret=false;
  if(osout){
    if(streamoutsink_)core_->add_sink(streamoutsink_=setupSink(false,core_,*osout,false,level_));
    osout_=osout;
    ret=true;
  }
  if(oserr){
    if(!streamerrsink_)core_->add_sink(streamerrsink_=setupSink(false,core_,*oserr,true,level_));
    oserr_=oserr;
    ret=true;
  }
  return ret;
}
bool Logger::activateNoStdlogAux(){
  // even if atd sinks are not active, we need to have a sink without ouput or, the output will be generated by default)
  if(!stdoutsink_)core_->add_sink(stdoutsink_=setupSink(true,core_,cout,false,level_));
  if(!stderrsink_)core_->add_sink(stderrsink_=setupSink(true,core_,cerr,true,level_));
  return true;
}
// deactivate std logging on a logger
void Logger::deactivateStdlog(){
  if(log2std_){
    core_->remove_sink(stdoutsink_);
    stdoutsink_.reset();
    core_->remove_sink(stderrsink_);
    stderrsink_.reset();
  }
  // dactivate std logger (not eneough to remove sinks)
  activateNoStdlogAux();
}
// convert level to a string
string Logger::level2string(AppLogLevel level){
  switch(level){
    case AppLogLevel::TRACE:
      return "TRACE";
    case AppLogLevel::DEBUG:
      return "DEBUG";
    case AppLogLevel::NORMAL:
      return "NORMAL";
    default:
      throw runtime_error("Logger::level2string: invalid level in program: "s+to_string(static_cast<int>(level)));
  }
}
string Logger::level2string()const{
  return Logger::level2string(level_);
}
}
