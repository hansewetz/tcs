#include "general/utils/strutils.h"
//#include "general/utils/utils.h"
#include <vector>
#include <list>
#include <set>
using namespace std;
using namespace tcs;

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
  auto p1=pair("Hello"s,17);
  cout<<p1<<endl;

  char c='a';
  int i=17;

  using mytype=std::decay_t<decltype(true?char():int())>;
  cout<<"mytype: "<<type2string(mytype())<<endl;

  auto maxres=tmax(c,i);
  cout<<"max: "<<maxres<<", type: "<<type2string(maxres)<<endl;

  cout<<strcat(", "s,list<int>{1,2,3,4})<<endl;
  cout<<strcat(", "s,set<int>{1,2,3,4})<<endl;
  cout<<strcat(", "s,vector<int>{1,2,3,4})<<endl;
  cout<<strcat(", "s,Junk{1},"2",3,4)<<endl;
}
