#include "general/utils/strutils.h"
#include <iterator>
#include <sstream>
#include <cctype>
using namespace std;
namespace tcs{

// utility function for transforming a string into another
string xformstr(string const&s,int(*fx)(int)){
  string ret;
  ret.resize(s.length());
  transform(begin(s),end(s),begin(ret),fx);
  return ret;
}
// create an uppercase string from a string
string toupper(string const&s){
  return xformstr(s,::toupper);
}
// create a lowercase string from a string
string tolower(string const&s){
  return xformstr(s,::tolower);
}
}
