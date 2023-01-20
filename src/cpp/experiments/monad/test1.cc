#include "general/utils/strutils.h"
#include "general/utils/typeutils.h"
#include <optional>
#include <iostream>
#include <ranges>
#include <type_traits>
using namespace std;


// NOTE! TODO:
//
// - try to simplify
// - more 'option<>' functions
// - add 'compose()' function that composes multiple option functions
// - extend composition example in main to any number of functions
// - add gunit tests in DF monad unit test (efutils1)

namespace tcs{

// ----------------------------------------------------
template <typename>struct is_optional:std::false_type {};
template <typename T>struct is_optional<std::optional<T>>:std::true_type{};
template<typename T>inline constexpr bool is_optional_v=is_optional<T>::value;
template<typename T>concept Optional=is_optional_v<std::remove_reference_t<T>>;
// ----------------------------------------------------

// -------------------------------------------------------------------
// monad for calculating with optional<>
// (optional<T1>, F: T1-->optional<T2>) --> optional<T2>
template<Optional T,typename F>
auto mbind(T&&t,F f)->decltype(f(std::forward<T>(t).value())){
  if(!std::forward<T>(t))return {};
  return f(std::forward<T>(t).value());
}
// adaptor for mbind
// (need an object to call pipe operator on)
struct mbind_opt_adaptor{
  // holder of function to call
  // (used so that we can constrain pipe operator functions for this monad)
  // (we could otherwise just skip the closure and have a call operator in parent class that returns a lambda)
  template<typename F>struct closure{
    closure(F f):f_(f){}                        // ctor
    auto operator()(auto&&t){return f_(std::forward<decltype(t)>(t));}      // call function
  private:
    F f_;                                       // function to call
  };
  mbind_opt_adaptor()=default;                     // ctor
  auto operator()(auto f)const{return closure(f);} // function operator creating a closure holding an F
};
// global mbind variable for optional monad
constexpr auto mbind_opt=mbind_opt_adaptor{};

// pipe operator taking an optional as parameter
template<typename F,Optional T>
auto operator|(T&&t,mbind_opt_adaptor::closure<F>c){
  return mbind(std::forward<T>(t),c);
}
// pipe operator where we convert passed parameter to optional
// (make it simpler to say: '5|mbind_opt(f)' than to say: 'optional<int>(5)|mbind(f)')
template<typename F,typename T>
auto operator|(T&&t,mbind_opt_adaptor::closure<F>c){
  return std::optional<T>(std::forward<T>(t))|mbind_opt(c);
}
// -------------------------------------------------------------------

// compose functions into a single function that is returned
/*
template<typename F>
auto compose(F f){
  return [&](auto t){returnf(t);};
}
template<typename F,typename...Fs>
auto compose(F f,Fs...fs){
  auto ret1=[&](auto t){return f(t);};
  auto ret2=compose(fs... NOTE!
}
*/
}

// test main program
int main(){
  auto mul2=[](int val)->optional<int>{return val*val;};
  auto tostring=[](int val)->optional<string>{ostringstream os; os<<val;return os.str();};
  auto ident=[]<typename T>(T val)->optional<T>{return val;};

  // testing
  //auto res=tcs::compose(mul2,tostring);

  // compose two mbind functions
  auto fres1=[](auto f1,auto f2){
    return [f1,f2]<typename T>(optional<T>v){return tcs::mbind(tcs::mbind(v,f1),f2);};
  };
  auto fres2=fres1(mul2,tostring);
  cout<<"res2: "<<fres2(optional<int>(5)).value()<<endl;

  // call monad within a range pipeline
  vector<int>v{1,2,3};
  auto res=v|views::transform([](int v){return optional<int>(v);})
            |views::transform([mul2]<typename T>(T v){return tcs::mbind(v,mul2);})
            |views::transform([tostring]<typename T>(T v){return tcs::mbind(v,tostring);});
  for(auto r:res)if(res)cout<<"r: "<<r.value()<<endl;

  // call monad within a non-range pipeline
  auto res1=5|tcs::mbind_opt(mul2)|tcs::mbind_opt(tostring)|tcs::mbind_opt(ident);
  cout<<"res1_t: "<<tcs::type2string(res1)<<endl;
  if(res1)cout<<res1.value()<<endl;
  else cout<<"FAILURE"<<endl;
}
