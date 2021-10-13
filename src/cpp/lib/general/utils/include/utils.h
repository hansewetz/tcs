#include <concepts>
#include <type_traits>
#include <iostream>
#include <cxxabi.h>
namespace tcs{

// --- concept: check if a number of types are all streamable
template <typename ...Ts>
concept Streamable=requires(std::ostream& os,Ts ... ts){
  {((os<<ts),...)};
};
// --- get a type as a string
template<typename T>
[[nodiscard]]std::string type2string(T t){
  int status;
  return std::string("(")+abi::__cxa_demangle(typeid(T).name(),0,0,&status)+")";
}
// --- min/max functions with variable return type
template<typename T1,typename T2>
[[nodiscard]]auto tmax(T1 t1,T2 t2)->std::common_type_t<T1,T2>{
  return t1>t2?t1:t2;
}
template<typename T1,typename T2>
[[nodiscard]]auto tmin(T1 t1,T2 t2)->std::common_type_t<T1,T2>{
  return t1<t2?t1:t2;
}
// --- stream operator for pair
template<typename T1,typename T2>
[[nodiscard]]std::ostream&operator<<(std::ostream&os,std::pair<T1,T2>const&p)requires Streamable<T1,T2>{
  return os<<"["<<p.first<<", "<<p.second<<"]";
}
}
