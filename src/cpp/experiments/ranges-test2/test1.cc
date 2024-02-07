#include "general/utils/strutils.h"
#include "general/utils/typeutils.h"
#include <ranges>
#include <concepts>
#include <vector>
#include <optional>
#include <iostream>
#include <functional>

using namespace std;

// ------------------------------------------------

// view class
template <std::ranges::view View,typename Pred>
class xx_view : public std::ranges::view_interface<xx_view<View,Pred>>{
public:
  // ctors
  xx_view()=default;
  //xx_view(View&&v,Pred pred):subview_(std::forward<View>(v)),pred_(pred){}
  xx_view(View&&v,Pred pred):subview_(std::forward<View>(v)){}

  // nested iterator forward decl
  class iterator;

  // get begin/end iterators
  // (note: cast away constness from view since the resuilt from begin()/end() depends on the constness of the view)
  auto begin()const{return iterator(const_cast<View*>(&subview_)->begin(),const_cast<View*>(&subview_)->end(),&pred_);}
  auto end()const{return iterator(const_cast<View*>(&subview_)->end());}
private:
  View subview_=View();
  Pred pred_;
};
// iterators for view
// (declared inside 'xx_view' class)
template <typename View, typename Pred>
class xx_view<View, Pred>::iterator{// NOTE! :ranges::__detail::__filter_view_iter_cat<View>{
private:
  // get iterator type and sentinel type from parent view
  using Begin=std::ranges::iterator_t<View>;
  using End=std::ranges::sentinel_t<View>;

  // this class is the only one that is allowed to create begin()/end() iterator
  friend class xx_view<View, Pred>;
public:
  // some typedefs
  using value_type=std::ranges::range_value_t <View>;
  using reference=std::ranges::range_reference_t<View>;
  using difference_type=int;
  using iterator_category=std::input_iterator_tag;

  // default ctor
  constexpr iterator()=default;

  // deref operator
  auto operator*()const{return value();}
  auto operator*(){return value();}

  // pre-incremenet iterator
  // (if pred is true, we skip the next item)
  iterator&operator++(){
    step(true);
    return*this;
  }
  // post-increment iterator
  iterator operator++(int){
    auto ret=*this;
    step(true);
    return ret;
  }
  // compare iterators for equality
  bool operator==(iterator const&other)const{
    if(me_&&other.me_)return me_.value()== other.me_.value();  // we have 2 non-sentinels
    if(!me_&&!other.me_)return true;                           // we have 2 sentinels
    if(!me_)return other.me_.value()==end_;                    // a is only sentinel
    return me_.value()==other.end_;                            // b is end only sentinel
  }
private:
  // private data
  mutable optional<Begin>me_;   // if set, then we have an iterator that is not fixed at end()
  End end_;                     // end of sequence - could be a sentinel
  Pred const*pred_=nullptr;

  // ctors (one for begin() and one for end() iterator
  iterator(Begin me, End end,Pred const*pred):me_(std::move(me)),end_(std::move(end)), pred_(pred){}
  iterator(End end):end_(std::move(end)){}

  // get value of iterator
  auto value()const{
    step(false);
    return *me_.value();
  }
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
xx_view(Range&&,Pred&&) -> xx_view<std::ranges::views::all_t<Range>,std::decay_t<Pred>>;

// ---- NOTE! ADAPTOR ----------------------
// (from: https://stackoverflow.com/questions/69465335/c20-ranges-custom-view-error-when-trying-to-pipe-into-another-view)
//
namespace detail{

// creates a view
template<typename Pred>
struct xx_range_adaptor_closure{
    Pred pred_;

    // ctor
    constexpr xx_range_adaptor_closure(Pred pred):pred_(std::move(pred)){
    }
    // call operator: create a view
    // (used in pipe operator)
    template<std::ranges::viewable_range R>
    constexpr decltype(auto) operator() (R&& r) const{   
        return xx_view{std::forward<R>(r), pred_};
    }
};

// creates a view or a closure
// - when on the left side of a pipe we neec to create a view by passing in a range and a pred
// - when on the right side of a pipe, we need to create a closure by passing in a pred only
struct xx_range_adaptor{
    // call operator: return a view - can be used as: xx(range,pred) --> xx_view
    template<std::ranges::viewable_range R>
    constexpr decltype(auto) operator() (R&& r, auto pred)const{
        return xx_view{std::forward<R>(r), std::move(pred)};
    }   
    // call operator: return a closure
    constexpr decltype(auto) operator() (auto pred)const{   
        return xx_range_adaptor_closure{std::move(pred)};
    }
};

// pipe operator: used when xx is on right side of pipe
// (   xx(pred) returns a closure which is passed into pipe operator   )
template<std::ranges::viewable_range R>
constexpr decltype(auto) operator| (R&& r,xx_range_adaptor_closure<auto>&&closure){
    return std::move(closure)(std::forward<R>((r)));
}
}

// NOTE! correct way of doing this ...?
constexpr detail::xx_range_adaptor xx;
// ----------------------


// test main program
int main(){
  // predicate
  auto pred=[](auto val)->bool{return val%2==0;};

  // NOTE! 'piping' xx_view into 'take_view' does not work ... find out what has to be done
  // (see: https://stackoverflow.com/questions/69465335/c20-ranges-custom-view-error-when-trying-to-pipe-into-another-view)
  vector<int>v={0,1,2,3,4,5};
  auto xxv1=xx_view(v,pred);
  auto xxv2=v|xx(pred)|std::views::take(2);
  auto xxv3=xx(v,pred)|std::views::take(2);

  for(auto&&val:xxv3){
    cout<<val<<' ';
  }
  cout<<endl;
}
