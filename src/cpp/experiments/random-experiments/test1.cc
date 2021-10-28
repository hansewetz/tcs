#include <concepts>
#include <vector>
#include <numeric>
#include <array>
#include <sstream>
#include <list>
#include <set>
#include <type_traits>
#include <iostream>
#include <cxxabi.h>
using namespace std;


// (=== sysutils.h)
// NOTE! not yet done





// (=== dsutils.h)
// NOTE!! data structure utilities




// --- function for converting a list of values to a container
// NOTE! convert true,false --> container
// NOTE! not yet done

// test
struct Junk{
  Junk(int j):i(j){}
  int i;
};
ostream&operator<<(ostream&os,Junk const&junk){
  return os<<junk.i;
}


// test main program
int main(){
}
