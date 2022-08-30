#include "general/sysutils/envutils.h"
#include "general/logtrace/throw.h"
#include <stdlib.h>
using namespace std;
namespace tcs{

// get value of env variable - if error, return nullopt and set an error in 'err'
optional<string>getenvvar(string const&env,string&err){
  auto envvar=getenv(env.c_str());
  if(envvar==0){
    err="getenvvar_throw: env variable: "s+env+" is not set in environment";
    return nullopt;
  }
  return envvar;
}
// get value of env variable - if error, throw exception
string getenvvar(string const&env){
  string err;
  auto ret=getenvvar(env,err);
  if(ret)return ret.value();
  THROW_TEXCEPT(err);
}
}
