#include "InvalidCmd.h"
#include <iostream>
using namespace std;
namespace tcs{

// debug print cmd
ostream&operator<<(ostream&os,InvalidCmd const&c){
  // NOTE! dump more info here
  return os<<"InvalidCmd";
}
InvalidCmd::InvalidCmd(string const&progn):progn_(progn),argc_(1),argv_(nullptr){
}
InvalidCmd::InvalidCmd(string const&progn,int argc,char*argv[]):progn_(progn),argc_(argc),argv_(argv){
}
// dummy implementations - should never be called
bool InvalidCmd::match(string const&cmd){return false;}
string InvalidCmd::cmdname(){return "invalid";}
}
