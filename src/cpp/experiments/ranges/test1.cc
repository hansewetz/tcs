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
class xx_view : public std::ranges::view_interface<xx_view<View,Pred>>{
public:
  // ctors
  xx_view()=default;
  xx_view(View&&v,Pred pred):subview_(std::forward<View>(v)),pred_(pred){ }

  // nested iterator forward decl
  // (need templated iterator since 'end()' might be a different type than begin - i.e., a sentinel)
  // (it won't work with views like std::view::iota(...) unless we template on the end() type.
  // (the end) type might be a sintinel std::unreachable_sentinel_t like for view::iota for example))
  template<typename End>class iterator;

  // get begin/end iterators
  auto begin()const{return iterator{subview_.begin(),subview_.end(),&pred_};}
  auto end()const{return iterator{subview_.end()};}
private:
  View subview_=View();
  Pred pred_;
};
// iterators for view
template <typename View, typename Pred>
template<typename End>
class xx_view<View, Pred>::iterator{
private:
  // get some type info from parent
  using parent_t=std::ranges::iterator_t<View>;
  using parent_traits=std::iterator_traits<parent_t>;

  // this class is the only one that is allowed to create begin()/end() iterator
  friend class xx_view<View, Pred>;
public:
  // some typedefs
  using value_type=parent_traits::value_type;
  using reference=parent_traits::reference;
  using pointer=parent_traits::pointer;
  using difference_type=std::ptrdiff_t;
  using iterator_category=std::input_iterator_tag;

  // ctor
  constexpr iterator()=default;

  // deref operator
  auto operator*(){
    step(false);
    return *me_.value();
  }
  auto operator*()const{
    step(false);
    return *me_.value();
  }
  // prefix increment iterator
  // (if pred is true, we skip the next item)
  iterator&operator++(){
    step(true);
    return*this;
  }
  // postfix increment iterator
  void operator ++(int){
    ++(*this);
  }
  // compare iterators
  friend bool operator==(iterator<End>const&a,iterator<End>const&b){
    if(a.me_&&b.me_)return a.me_.value()== b.me_.value();    // we have 2 non-end iterators
    if(!a.me_&&!b.me_)return true;                           // we have 2 end iterators
    if(!a.me_)return b.me_.value()==a.end_;                  // a is only end iterator
    return a.me_.value()==b.end_;                            // b is end only iterator
  }
private:
  // private data
  mutable optional<parent_t>me_;   // if set, then we have an iterator that is not fixed at end()
  End end_;                        // end of sequence - could be a sentinel
  Pred const*pred_=nullptr;

  // ctors (one for begin() (non-end) and one for end() iterator
  iterator(parent_t const&me, End end,Pred const*pred):me_(me), end_(end), pred_(pred){}
  iterator(End end):end_(std::move(end)){}

// NOTE!
//  iterator(iterator<End>const&it):me_(it.me_),end_(it.end_),pred_(it.pred_){}
/*
  iterator&operator=(iterator<End>const&it){
    me_,it=me_;
    end_,it=end_;
    pred_,it=pred_;
    return *this;
  }
*/

  // step iterator until we have a pred that is true or we reach end
  // (when leaving we either point to end() or predicate is true)
  // (parameter specifies if we should step at a minimum one step)
  void step(bool alwaysStep)const{
    // if we are end iterator, then return
    if(!me_)return;

    // loop until we either have a value or we reach end()
    while(me_.value()!=end_){
      if(alwaysStep){
        ++me_.value();
        if(me_.value()==end_)break;
        alwaysStep=false;
      }
      // evaluate predicate to see if we stop here
      if((*pred_)(*me_.value()))return;
      ++me_.value();
    }
  }
};
// template deduction
template <std::ranges::range Range,typename Pred>
xx_view(Range&&,Pred&&) -> xx_view<std::ranges::views::all_t<Range>,Pred>;

// adaptor (supplies 'operator|' and 'operator()')
// (this is the class we use in pipelines of ranges)
struct xx_adaptor{
  // closure class 
  // (holds predicate and cal be called with a range)
  template <typename Pred>
  class closure{
  public:
    friend class xx_adaptor;

    // call operator - used when we create view by passing just predicate (range is piped in)
    template <typename Range>
    auto operator()(Range&&range) {
      return xx_view(std::forward<Range>(range),std::forward<Pred>(pred_));
    }
    // call operator - used when we create view by passing both range and predicate
    template <typename Range>
    auto operator()(Range&&range, Pred pred)const{
      return xx_view(std::forward<Range>(range), std::forward<Pred>(pred));
    }
  private:
    // ctor
    explicit closure(Pred pred):pred_(pred){}

    // predicate
    Pred pred_;
  };
  // call operator
  template <typename Pred>
  auto operator()(Pred pred) const {
    return closure<Pred>(std::forward<Pred>(pred));
  }
  // pipe operator friend function
  // (creates closure which holds predicate and then view)
  template <typename Range, typename Pred>
  friend auto operator|(Range&&rng,closure<Pred>pred) {
    return std::forward<closure<Pred>>(pred)(std::forward<Range>(rng));
  }
};
// declare adaptor variable to use
constexpr auto my=xx_adaptor{};


// test main program
int main(){
  // predicate
  auto pred=[](auto val)->bool{return val%2==0;};

  // pipe an infinite stream of integers into our view, take the last 3 ones and transform them
  //auto res=views::iota(0)|my([](auto c)->bool{return c%2==1;})|views::take(5)|views::transform([](auto&&val){return val;});
  auto res=views::iota(0)|my(pred)|views::take(5)|views::transform([](auto&&val){return val;});

  for(auto c:res)cout<<c<<" ";
  cout<<endl;
}
