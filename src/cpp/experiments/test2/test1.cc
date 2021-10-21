#include "general/utils/strutils.h"
#include "general/utils/utils.h"
#include <ranges>
#include <vector>
#include <list>
#include <set>
using namespace std;
using namespace tcs;

// test
template<typename T>
struct Junk{
  Junk(T j):i(j){}
  T i;
};
template<typename T>
ostream&operator<<(ostream&os,Junk<T>const&junk){
  return os<<junk.i;
}

// test main program
int main(){
/*
  vector<int>v{1,2,3,4,5};
  cout<<v<<endl;
*/
/*
  auto even=[](int i){return i%2;};
  auto square=[](int i){return i*i;};
  auto ints={0,1,2,3,4};
  for (int i:ints|std::views::filter(even)|std::views::transform(square)){
    std::cout<<i<<' ';
  }
  cout<<endl;
*/

  //pair<int,string>p(17,"Hello"s);
  //cout<<p<<endl;
}
