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
class my_view : public std::ranges::view_interface<my_view<View,Pred>>{
public:
  // ctors
  my_view()=default;
  my_view(View v,Pred p):subview_(std::move(v)),pred_(std::move(p)){ }

  // nested iterator forward decl
  // (need templated iterator since 'end()' might be a different type than begin - i.e., a sentinel)
  // (it won't work with views like std::view::iota(...) unless we template on the end() type.
  // (the end9) type might be a sintinel std::unreachable_sentinel_t like for view::iota for example))
  template<typename End>class iterator;

  // get begin/end iterators
  auto begin() const {return iterator{subview_.begin(),subview_.end(),&pred_};}
  auto end() const {return iterator{subview_.end(),&pred_};}
private:
  View subview_ = View();
  Pred pred_;
};
// iterators for view
template <typename View, typename Pred>
template<typename End>
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
  auto operator *() { return *me_.value(); }
  auto operator *() const { return *me_.value(); }

  // increment iterator
  // (if pred is true, we skip the next item)
  iterator &operator ++() {
    if(!me_)return *this;
    if(me_.value()==end_)return *this;
    ++me_.value();
    return *this;       
  }
  // increment iterator
  void operator ++(int) {
      ++(*this);
  }
  // compare iterators
  template<typename End1,typename End2>
  friend bool operator==(iterator<End1> const &a, iterator<End2> const &b) {
    if(a.me_&&b.me_)return a.me_.value()== b.me_.value();    // we have 2 non-end iterators
    if(!a.me_&&!b.me_)return true;                           // we have 2 end iterators
    if(!a.me_)return b.me_.value()==a.end_;                  // b is end iterator
    return a.me_.value()==b.end_;                            // a is end iterator
  }
private:
  optional<parent_t>me_;           // if set, then we have an iterator that is not fixed at end()
  End end_;                        // end of sequence - could be a sentinel
  Pred const *pred_=nullptr;

  // ctors (one for begin() (non-end) and one for end() iterator
  iterator(parent_t const &me, End  end, Pred const *pred):me_(me), end_(std::move(end)), pred_(pred){}
  iterator(End  end, Pred const *pred):end_(std::move(end)), pred_(pred){}
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
/* note: do we need this ...?
  // call operator
  template <typename Range, typename Pred>
  auto operator()(Range &&range, Pred &&pred) const {
    return my_view(std::forward(range), std::forward(pred));
  }
*/
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
  auto res=views::iota(1)|my([](auto c)->bool{return true;})|views::take(3)|views::transform([](auto&&val){return val*val;});

  for(auto c:res)cout<<c;
  cout<<endl;
}
