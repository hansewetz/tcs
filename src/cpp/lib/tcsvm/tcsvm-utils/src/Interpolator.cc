#include "tcsvm/tcsvm-utils/Interpolator.h"
#include "general/logtrace/throw.h"
#include <string>
#include <set>
#include <sstream>
#include <iterator>
#include <iostream>
#include <map>
using namespace std;
namespace tcs::vm{

// ctor
Interpolator::Interpolator(optional<flookup_t>const&fenv,optional<flookup_t>const&fvar,
                           optional<flookup_t>const&fcmd,optional<flookup_t>const&fcfg):
    fenv_(fenv),fvar_(fvar),
    fcmd_(fcmd),fcfg_(fcfg){
}
// interpolate a string - throw an exception if failure
string Interpolator::interpolate(string const&rawstr)const{
  string err;
  auto ret=interpolate(rawstr,err);
  if(ret)return ret.value();
  THROW_TEXCEPT("error while interpolating string: '"<<rawstr+"': "<<err);
}
// interpolate a string
optional<string>Interpolator::interpolate(string const&rawstr,string&err)const{
  static set<char>escchars={'$','%','`','*'};
  stringstream ret;
  int ind=0;
  int n=rawstr.size();
  while(ind<n){
    char c=rawstr[ind++];

    // check if we have an escaped char
    if(c=='\\'){
      if(ind==n)return seterr(err,"escape character '\\' found at end of string: ");
      c=rawstr[ind++];
      if(!escchars.count(c))return seterr(err,"invalid escape sequence '\\"s+c+"' - can only escape characters: [\\\"$%*]");
      ret<<c;
      continue;
    }
    // check if we have a non-escaped character with no special meaning
    if(escchars.count(c)==0){
      ret<<c;
      continue;
    }
    // check if we have an embedded command
    if(c=='`'){
      if(ind==n)return seterr(err,"found ` at end of string");
      stringstream scmd;
      c=rawstr[ind++];
      while(c!='`'){
        scmd<<c;
        if(ind==n)return seterr(err,"no matching '`' for command");
        c=rawstr[ind++];
      }
      // check if we got a cmd and get it as a string
      if(c!='`')return seterr(err,"no matching '`' for command");
      string cmd=scmd.str();

      // execute cmd
      string lerr;
      auto cmdres=optionalcall(fcmd_,cmd,lerr);
      if(!cmdres)return seterr(err,lerr);
      ret<<cmdres.value();
      continue;
    }
    // we either have an env variable or a normal program variable or a config variable
    // remember type of variable
    bool isenv=c=='$'?true:false;
    bool iscfg=c=='*'?true:false;

    // we have an environment or memory variable (either $xxx, ${xxx}, %xxx or %xxx, *xxx or *{xxx})
    if(ind==n)return seterr(err,"found '"s+c+"' at end of string");
    stringstream sname;
    c=rawstr[ind++];
    if(c=='{'){
      // search for a matching '}' - we can only have chars in [a-zA-Z0-9_]
      bool allowdot=false;
      while(ind<n&&(c=rawstr[ind++])!='}'){
        if(!valididc(allowdot,c))return seterr(err,"invalid character: "s+c+" inside interpolated variable name");
        allowdot=!isenv;
        sname<<c;
      }
      // check if we got a name
      if(c!='}')return seterr(err,"no matching '}' for '{'");
    }else{
      // search for a character that is not a valid ident char
      bool allowdot=false;
      for(valididc(allowdot,c);;){
        allowdot=!isenv;
        sname<<c;
       if(ind==n)break;
       c=rawstr[ind];
       if(!valididc(allowdot,c))break;
       ++ind;
      }
    }
    // we now have a name for variable (env or var)
    string name=sname.str();
    if(name.length()==0)return seterr(err,"found empty name (environment or variable)");

    // depending on if we have an env var or memory variable do something
    if(isenv){
      string lerr;
      auto envres=optionalcall(fenv_,name,lerr);
      if(!envres)return seterr(err,lerr);
      ret<<envres.value();
    }else
    if(iscfg){
      string lerr;
      auto cfgres=optionalcall(fcfg_,name,lerr);
      if(!cfgres)return seterr(err,lerr);
      ret<<cfgres.value();
    }else{
      string lerr;
      auto varres=optionalcall(fvar_,name,lerr);
      if(!varres)return seterr(err,lerr);
      ret<<varres.value();
    }
  }
  return ret.str();
}
// set value of a variable 'err' to 'errstr' and return a std::nullopt
optional<string>Interpolator::seterr(string&err,string const&errstr)const{
  err=errstr;
  return std::nullopt;
}
optional<string>Interpolator::optionalcall(optional<flookup_t>const&f,string const&s,string&err)const{
  if(!f)return s;
  return f.value()(s,err);
}
// check if a character can be part of an identifier
bool Interpolator::valididc(bool allowdot,char c)const{
  if(!allowdot)return (c>='a'&&c<'z')||(c>='A'&&c<='Z')||(c>='0'&&c<='9')||c=='_';
  return (c>='a'&&c<='z')||(c>='A'&&c<='Z')||(c>='0'&&c<='9')||c=='_'||c=='.';
}
}
