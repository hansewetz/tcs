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
Interpolator::Interpolator(optional<flookup_t>const&fenv,optional<flookup_t>const&fmem_,
                           optional<flookup_t>const&fcmd,optional<flookup_t>const&fcfg):
    fenv_(fenv),fmem_(fmem_),
    fcmd_(fcmd),fcfg_(fcfg){
}
// interpolate a string - throw an exception if failure
string Interpolator::interpolate(string const&rawstr)const{
  static set<char>escchars={'$','%','`','*'};
  stringstream ret;
  int ind=0;
  int n=rawstr.size();
  while(ind<n){
    char c=rawstr[ind++];

    // check if we have an escaped char
    if(c=='\\'){
      if(ind==n)THROW_TEXCEPT("escape character '\\' found at end of string: ");
      c=rawstr[ind++];
      if(!escchars.count(c))THROW_TEXCEPT("invalid escape sequence '\\"s+c+"' - can only escape characters: [\\\"$%*]");
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
      if(ind==n)THROW_TEXCEPT("found ` at end of string");
      stringstream scmd;
      c=rawstr[ind++];
      while(c!='`'){
        scmd<<c;
        if(ind==n)THROW_TEXCEPT("no matching '`' for command");
        c=rawstr[ind++];
      }
      // check if we got a cmd and get it as a string
      if(c!='`')THROW_TEXCEPT("no matching '`' for command");
      string cmd=scmd.str();

      // execute cmd
      ret<<optionalcall(fcmd_,cmd);
      continue;
    }
    // we either have an env variable or a normal program variable or a config variable
    // remember type of variable
    bool isenv=c=='$'?true:false;
    bool iscfg=c=='*'?true:false;

    // we have an environment or memory variable (either $xxx, ${xxx}, %xxx or %xxx, *xxx or *{xxx})
    if(ind==n)THROW_TEXCEPT("found '"s+c+"' at end of string");
    stringstream sname;
    c=rawstr[ind++];
    if(c=='{'){
      // search for a matching '}' - we can only have chars in [a-zA-Z0-9_]
      bool allowdot=false;
      while(ind<n&&(c=rawstr[ind++])!='}'){
        if(!valididc(allowdot,c))THROW_TEXCEPT("invalid character: "s+c+" inside interpolated variable name");
        allowdot=!isenv;
        sname<<c;
      }
      // check if we got a name
      if(c!='}')THROW_TEXCEPT("no matching '}' for '{'");
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
    if(name.length()==0)THROW_TEXCEPT("found empty name (environment or variable)");

    // depending on if we have an env var or memory variable do something
    if(isenv){
      ret<<optionalcall(fenv_,name);
    }else
    if(iscfg){
      ret<<optionalcall(fcfg_,name);
    }else{
      ret<<optionalcall(fmem_,name);
    }
  }
  return ret.str();
}
string Interpolator::optionalcall(optional<flookup_t>const&f,string const&s)const{
  if(!f)return s;
  return f.value()(s);
}
// check if a character can be part of an identifier
bool Interpolator::valididc(bool allowdot,char c)const{
  if(!allowdot)return (c>='a'&&c<'z')||(c>='A'&&c<='Z')||(c>='0'&&c<='9')||c=='_';
  return (c>='a'&&c<='z')||(c>='A'&&c<='Z')||(c>='0'&&c<='9')||c=='_'||c=='.';
}
}
