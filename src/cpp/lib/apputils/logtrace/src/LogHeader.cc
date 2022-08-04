#include "apputils/logtrace/LogHeader.h"
#include <iostream>
#include <stdexcept>

using namespace std;
namespace tcs{

// get singleton instance
LogHeader&LogHeader::instance(){
  static LogHeader inst;
  return inst;
}
// set logheader
string const&LogHeader::getlogheader()const{
  if(!logheader_)throw runtime_error("LogHeader::getlogheader: logheader string not set");
  return logheader_.value();
}
// set log header
void LogHeader::setlogheader(std::string const&logheader){
  logheader_=logheader;
}
}
