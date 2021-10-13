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

// (=== typeutils.h)
// --- check if a number of types are all streamable
template <typename ...Ts>
concept Streamable=requires(std::ostream& os,Ts ... ts){
  {((os<<ts),...)};
};
// --- get a type as a string
template<typename T>
std::string type2string(T t){
  int status;
  return std::string("(")+abi::__cxa_demangle(typeid(T).name(),0,0,&status)+")";
}


// (=== strutils.h)
// --- create string of container elements separated by a separator.
template<Streamable S,template<class>class C,Streamable T>
string strcat(S const&sep,C<T>const&c){
  stringstream ret;
  for(auto it=begin(c);it!=end(c);++it){
    ret<<*it<<(next(it)!=end(c)?sep:S{});
  }
  return ret.str();
}
// --- create a string of a list of variables (possibly of different types) separated by a separator
template<Streamable S,Streamable T,Streamable...Ts>
string strcat1(S const&s,T const&t,Ts const&...ts){
  stringstream ret;
  ret<<t,((ret<<s<<ts),...);  // the 'comma' operator is one of the operators supported by fold operators
  return ret.str();
}


// (=== utils.h)
// min/max functions
template<typename T1,typename T2>
auto tmax(T1 t1,T2 t2)->std::common_type_t<T1,T2>{
  return t1>t2?t1:t2;
}
template<typename T1,typename T2>
auto tmin(T1 t1,T2 t2)->std::common_type_t<T1,T2>{
  return t1<t2?t1:t2;
}
// operator<<(...) or pairs
template<typename T1,typename T2>
std::ostream&operator<<(std::ostream&os,std::pair<T1,T2>const&p)requires Streamable<T1,T2>{
  return os<<"["<<p.first<<", "<<p.second<<"]";
}


// (=== sysutils.h)
// NOTE! not yet done


// (=== dsutils.h)
// NOTRE! data structure utilities


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
  cout<<strcat1(", "s,Junk{1},"2",3,4)<<endl;
}
