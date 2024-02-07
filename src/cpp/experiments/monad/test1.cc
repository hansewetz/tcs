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

// ----------------------------------------------------
template <typename>struct is_optional:std::false_type {};
template <typename T>struct is_optional<std::optional<T>>:std::true_type{};
template<typename T>inline constexpr bool is_optional_v=is_optional<T>::value;
template<typename T>concept Optional=is_optional_v<std::decay_t<T>>;

// helper to create an optional
template<typename T>
std::optional<T>make_opt(T&&t){
  return std::optional<T>(std::forward<T>(t));
}
// ----------------------------------------------------

/*
// -------------------------------------------------------------------
// (1) where mbind function is called via a helper wrapper
// monad for calculating with optional<>
// (optional<T1>, F: T1-->optional<T2>) --> optional<T2>
template<Optional T,typename F>
auto mbind_opt(T&&t,F f)->decltype(f(std::forward<T>(t).value())){
  if(!t)return {};
  return f(std::forward<T>(t).value());
}
namespace detail{
template <typename F>
struct mbind_opt_helper{
  F f;
};
}
// we need the helper to select the correct overloaded 'operator|(...)' function - there might be many of them
// (for example, we might have another monad that is not optional but possibly a log wrapper)
template <typename F>
detail::mbind_opt_helper<F>mbind_opt(F&&f){
  return {std::forward<F>(f)};
}
// we will not interfere with other pipe operators since we enforce second argument to be a 'detail::opt_helper<F>
template <Optional T, typename F>
auto operator|(T&&xs,const detail::mbind_opt_helper<F>&helper) {
  return mbind_opt(std::forward<T>(xs),helper.f);
}
template<typename T,typename F>
auto operator|(T&&t,const detail::mbind_opt_helper<F>&helper){
  return make_opt(std::forward<T>(t))|helper;
}
// -------------------------------------------------------------------
*/

// -------------------------------------------------------------------
// (2) monad where mbind function is inside a struct
// monad for calculating with optional
// (this implementation combines mbind function + helper struct + helper-create-function into one structure)
template<typename F>
struct mbind_opts{
  // call operator
  template<Optional T>
  auto operator()(T&&t)->decltype(F()(std::forward<T>(t).value())){
    if(!t)return {};
    return f_(std::forward<T>(t).value());
  }
  F f_;
};
// pipe operator for 'mbind_opts'
template<Optional T,typename F>
auto operator|(T&&t,mbind_opts<F>m){
  return m(std::forward<T>(t));
}
template<typename T,typename F>
auto operator|(T&&t,mbind_opts<F>m){
  return make_opt(std::forward<T>(t))|m;
}
// -------------------------------------------------------------------

// -------------------------------------------------------------------

static const int x1=0;
static const optional<int>x2=optional<int>{0};

// test main program
int main(){
  auto times2=[](int val)->optional<int>{return 2*val;};
  auto mul2=[](int val)->optional<int>{return val*val;};
  auto tostring=[](int val)->optional<string>{ostringstream os; os<<val;return os.str();};
  auto ident=[]<typename T>(T val)->optional<T>{return val;};
  auto str2int=[](string&&val)->optional<int>{return stoi(val);};

  // (2)
  auto res1=5|mbind_opts(mul2)|mbind_opts(times2)|mbind_opts(tostring)|mbind_opts(ident);
  if(res1)cout<<res1.value()<<endl;
  else cout<<"FAILURE"<<endl;
  

/*
  // (1)
  auto res2=make_opt(5)|mbind_opt(mul2)|mbind_opt(tostring)|mbind_opt(ident);
  cout<<"res2: "<<tcs::type2string(res2)<<endl;
  if(res2)cout<<res2.value()<<endl;
  else cout<<"FAILURE"<<endl;

  auto res3=5|mbind_opt(mul2)|mbind_opt(tostring)|mbind_opt(ident);
  cout<<"res3: "<<tcs::type2string(res3)<<endl;
  if(res3)cout<<res3.value()<<endl;
  else cout<<"FAILURE"<<endl;
*/
}
