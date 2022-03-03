#include <iostream>
#include <vector>
#include <optional>
#include <coroutine>
#include <stdexcept>
using namespace std;

// future that does not know the promise_type
// (promise_type is looked up by the compiler through the std::coroutine_traits<Gen<T>>)
template<typename T>
class Gen{
public:
  // ctor, dtor
  Gen(coroutine_handle<void>handle,T const&valref):handle_(handle),valref_(valref){}
  ~Gen(){handle_.destroy();}

  // get current value
  // (we have reference referencing the last yield value in the promise object)(
  T value()const{return valref_;}

  // get next item
  optional<T>next(){
    if(handle_.done())return std::nullopt;
    handle_();
    return handle_.done()?optional<T>{}:value();
  }
private:
  coroutine_handle<void>handle_; // keep handle so we can control coroutine
  T const&valref_;                 // ref to value we store inside promise
};

// traits for looking up the promise object from a Gen<T> type
template <typename T>
struct std::coroutine_traits<Gen<T>,T>{
  // promise used when we need to store a single yield value
  class promise_type{
  public:
    Gen<T>get_return_object(){return Gen<T>(coroutine_handle<promise_type>::from_promise(*this),val_);}
    void return_void(){}
    std::suspend_always yield_value(T val){val_=val;return {};}
    std::suspend_always initial_suspend(){return {};}
    std::suspend_always final_suspend(){return {};}
    void unhandled_exception(){std::terminate();}
  private:
    T val_;
  };
};

// simple integer sequence
Gen<int>genint(int n){
  for(int i=0;i<n;++i)co_yield i;
}

// test program
int main(){
  auto gen=genint(5);
  while(auto val=gen.next())cout<<val.value()<<endl;
}
