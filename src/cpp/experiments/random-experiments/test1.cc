#include <iostream>
#include <optional>

template<typename T>
struct Monad {
  std::optional<T> value;
  Monad() {}
  Monad(const T& val) : value(val) {}
  template<typename F>
  auto operator|(F f) {
    if (value) {
      return Monad<decltype(f(value.value()))>(f(value.value()));
    } else {
      return Monad<decltype(f(value.value()))>();
    }
  }
};
template<typename T>
Monad<T> unit(const T& val) {
  return Monad<T>(val);
}
int main() {
  // Example usage
  auto m1 = unit(5);
  auto m2 = unit(10);
  auto m3 = m1 | [](int x) { return unit(x + 2); };
  auto m4 = m1 | [&m2](int x) { return m2 | [x](int y) { return unit(x * y); }; };
  auto m5 = Monad<int>();
  std::cout << "m1: " << m1.value.value_or(-1) << std::endl;
}
