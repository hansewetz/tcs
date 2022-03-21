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

template<Streamable U>
friend ostream&operator<<(ostream&os,Junk<U>const&junk){
  return os<<junk.i;
}

private:
  T i;
public:
  Junk(T j):i(j){}

  // add comparison operator(s)
  auto operator<=>(Junk<T>const&other)const{
    return i<=>other.i;
  }
};

template<typename T>
requires requires(Junk<T> t){
  {cout<<t.i}->std::same_as<std::ostream&>;
}
ostream&operator<<(ostream&os,Junk<T>const&junk){
  return os<<junk.i;
}

// test main program
int main(){

  auto t1=make_tuple(1,"hello", 1.7,"again"s);
  auto t2=vector<string>{"1","2"};
  auto t3=list<string>{"goodmorning","goodbuy"};
  cout<<strcat("|",t1)<<endl;
  cout<<strcat("|",t1)<<endl;
  cout<<strcat("|",t3)<<endl;

  Junk junk1(8);
  Junk junk2(10);
  auto res=junk1<=>junk2;
  cout<<"tmax(junk1, junk2): "<<boolalpha<<tmax(junk1,junk2)<<endl;

  int i1{8};
  int i2{10};
  cout<<tmax(i1,i2)<<endl;
  cout<<tmax(&i1,&i2)<<endl;
  cout<<tmin(i1,i2)<<endl;
  cout<<tmin(&i1,&i2)<<endl;

  Junk<int>j1(2);
  cout<<j1<<endl;


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

  auto even=[](int i){return i%2;};
  auto square=[](int i){return i*i;};
  auto ints={0,1,2,3,4};
  for (int i:ints|std::views::filter(even)|std::views::transform(square)){
    std::cout<<i<<' ';
  }
  cout<<endl;

  pair<int,string>p(17,"Hello"s);
  cout<<strcat(", ",p)<<endl;
}
