#include "general/utils/strutils.h"
#include <ranges>
#include <concepts>
#include <vector>
#include <optional>
#include <iostream>
#include <functional>
using namespace std;

// view class
template <std::ranges::view View,typename Pred>
requires std::ranges::input_range<View> &&
         std::ranges::common_range<View> &&
         std::is_object_v<Pred> &&
         std::indirect_unary_predicate<const Pred,std::ranges::iterator_t<View>>
class my_view : public std::ranges::view_interface<my_view<View,Pred>>{
public:
  // ctors
  my_view() = default;
  my_view(View v,Pred p):subview_(std::move(v)),pred_(std::move(p)){ }

  // iterator forward decl
  class iterator;
  friend class iterator;

  // get begin/end iterators
  auto begin() const {return iterator{subview_.begin(),subview_.end(),&pred_};}
  auto end() const {return iterator{subview_.end(),subview_.end(),&pred_};}
private:
  View subview_ = View();
  Pred pred_;
};
// iterators for view
template <typename View, typename Pred>
class my_view<View, Pred>::iterator{

  using parent_t = std::ranges::iterator_t<View>;
  using parent_traits = std::iterator_traits<parent_t>;

  friend class my_view<View, Pred>;
public:
  // some typedefs
  using value_type = parent_traits::value_type;
  using reference = parent_traits::reference;
  using pointer = parent_traits::pointer;
  using difference_type = std::ptrdiff_t;
  using iterator_category = std::input_iterator_tag;

  // ctor
  constexpr iterator() = default;

  // deref operator
  auto operator *() { return *me_; }
  auto operator *() const { return *me_; }

  // increment iterator
  // (if pred is true, we skip the next item)
  iterator &operator ++() {
    if(me_!=end_)++me_;
    return *this;       
  }
  // increment iterator
  void operator ++(int) {
      ++(*this);
  }
  // compare iterators
  friend bool operator ==(iterator const &a, iterator const &b) {
      return a.me_ == b.me_;
  }
private:
  parent_t me_;
  parent_t end_;
  Pred const *pred_ = nullptr;

  // ctor
  iterator(parent_t const &me, parent_t end, Pred const *pred):me_(me), end_(std::move(end)), pred_(pred){
  }
};
// template deduction
template <std::ranges::range Range, typename Pred>
my_view(Range&&,Pred&&) -> my_view<std::ranges::views::all_t<Range>, Pred>;

// adaptor (supplies 'operator|' and 'operator()')
struct my_adaptor {
  // closure class
  template <typename Pred>
  class closure {
    friend class my_adaptor;
    Pred pred_;

    // ctor
    explicit closure(Pred &&p) : pred_(std::move(p)) {}
  public:
    // call operator
    template <typename Range>
    auto operator ()(Range &&range) {
      return my_view(std::forward<Range>(range),std::move(pred_));
    }
  };
  // call operator
  template <typename Pred>
  auto operator ()(Pred pred) const {
    return closure<Pred>(std::move(pred));
  }
  // call operator
  template <typename Range, typename Pred>
  auto operator()(Range &&range, Pred &&pred) const {
    return my_view(std::forward(range), std::forward(pred));
  }
  // pipe operator friend function
  template <typename Range, typename Pred>
  friend auto operator|(Range&& rng, closure<Pred> &&fun) {
    return fun(std::forward<Range>(rng));
  }
};
// declare adaptror variable to use
constexpr auto my = my_adaptor{};



// test main program
int main(){
  //constexpr string_view str{"George Orwell"};
  string str{"George Orwell"};
  //vector<char>str{'a','b','c'};
  auto res=str|my([](char c){return c<'f';})|views::take(20);

  for(auto c:res)cout<<c;
  cout<<endl;
}
