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
  auto tup1=make_tuple("one"s,"two"s,"three"s);
  vector<string>vtup1;
  push_vals(vtup1,tup1);
  cout<<"vtup1: "<<strcat(", ",vtup1)<<endl;

  vector<int>s1;
  push_vals(s1,1,2,3,4,1,2,3,4);
  cout<<"s1: "<<strcat(", ",s1)<<endl;

  set<int>v1;
  push_vals(v1,s1);
  cout<<"v1: "<<strcat(", ",v1)<<endl;

  string str1="Hello again where are you again";
  cout<<"v: "<<strcat("|",splitstr<set<string>>(str1," "))<<endl;

  vector<string>v{"Hello","1","2"};
  cout<<strcat(", ",v)<<endl;

  string str2{"Hello"};
  cout<<strcat(", ",str2)<<endl;

  auto tu1=make_tuple();
  cout<<strcat(", ",tu1)<<endl;

  auto tup=make_tuple("Hello"s,1,4.7,"Again");
  cout<<strcat(", ",tup)<<endl;
  cout<<"("<<strcat(", ",tup)<<")"<<endl;

  cout<<"typ: "<<type2string(tup)<<endl;

/*
  auto even=[](int i){return i%2;};
  auto square=[](int i){return i*i;};
  auto ints={0,1,2,3,4};
  for (int i:ints|std::views::filter(even)|std::views::transform(square)){
    std::cout<<i<<' ';
  }
  cout<<endl;

  //pair<int,string>p(17,"Hello"s);
  //cout<<p<<endl;
*/
}
