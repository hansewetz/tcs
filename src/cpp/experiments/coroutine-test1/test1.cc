#include <iostream>
#include <vector>
#include <optional>
#include <queue>
#include <functional>
#include <coroutine>
using namespace std;

// --- (begin) promise object
// promise used when we need to store a single value
template<typename CRET>
class val_promise_type{
public:
  // we require 'return_object' to have a 'value_t' typedef
  using value_t=typename CRET::value_t;

  CRET get_return_object(){return CRET(coroutine_handle<val_promise_type>::from_promise(*this));}
  void return_void(){}
  std::suspend_always yield_value(value_t val){val_=val;return {};}
  std::suspend_always initial_suspend(){return {};}
  std::suspend_always final_suspend(){return {};}
  value_t value(){return val_;}
  void unhandled_exception(){}
private:
  value_t val_;
};
// --- (end) promise object

// -- (begin) couroutine return object for generator
template<typename T>
struct Gen{
  // some typedefs
  using value_t=T;
  using promise_type=val_promise_type<Gen>;

  // keep handle so we can get to promise
  coroutine_handle<promise_type>handle_;

  // ctor
  // (called from 'promise_type::get_return_object()')
  Gen(coroutine_handle<promise_type>handle):handle_(handle){}
  ~Gen(){handle_.destroy();}

  // get current value
  value_t value()const{
    return handle_.promise().value();
  }
  // get next item
  optional<value_t>next(){
    if(handle_.done())return std::nullopt;
    handle_.resume();
    return handle_.done()?optional<value_t>{}:value();
  }
};
// -- (end) coroutine object

// --- (begin) generators

// simple integer sequence
Gen<int>genint(int n){
  for(int i=0;i<n;++i)co_yield i;
}
// simple BFS traversal of graph
using graph_t=vector<vector<int>>;
enum color_t{white=0,grey=1,black=2};

Gen<int>genbfs(graph_t const&g,int ustart){
  if(g.size()==0)co_return;
  vector<color_t>color(g.size(),white);
  queue<int>q;
  q.push(ustart);
  color[ustart]=grey;
  while(!q.empty()){
    int u=q.front();
    q.pop();
    co_yield(u);
    for(int v:g[u]){
      if(color[v]!=white)continue;
      q.push(v);
      color[v]=grey;
    }
    color[u]=black;
  }
}
// character reader + tokenizer
Gen<char>genreadchar(string const&str){
  for(char c:str)co_yield c;
}
Gen<string>gentok(Gen<char>&creader){
  string currtok;
  while(creader.next()){
    char c=creader.value();
    if(c==' '){
      if(currtok.length()){
        co_yield currtok;
        currtok.clear();
      }
    }else{
      currtok.push_back(c);
    }
  }
  if(currtok.length())co_yield currtok;
}
// generate permutations in lexicographical order
Gen<string>genperm(string s){
  int n=s.length();

  // sort in ascending order and return the result as the first permutation
  sort(begin(s),end(s));
  co_yield(s);

  if(n==1)co_return;
  while(true){
    // find largest k such that s[k]<s[k+1]
    int k=n-2;
    for(;k>=0;--k){
      if(s[k]<s[k+1])break;
    }
    if(k<0)co_return;

    // find largest l>k such that s[k]<s[l]
    int l=n-1;
    for(;l>k;--l){
      if(s[k]<s[l])break;
    }
    swap(s[k],s[l]);
    reverse(begin(s)+k+1,end(s));

    // we now have the next permutation
    co_yield s;
  }
}
// --- (end) generators

// run generator and print result
void genprint(auto&gen){
  while(auto val=gen.next()){
    cout<<val.value()<<endl;
  }
}

// test program
int main(){
  cout<<"----- int sequence"<<endl;
  auto intgen=genint(5);
  genprint(intgen);

  cout<<"----- bfs graph"<<endl;
  graph_t g{
    {1, 2},
    {4},
    {3},
    {4},
    {5},
    {}
  };
  auto bfsgen=genbfs(g,0);
  genprint(bfsgen);

  cout<<"----- tokenizer graph"<<endl;
  string inpstring="Hello again where is the cheese";
  auto creader=genreadchar(inpstring);
  auto tokreader=gentok(creader);
  genprint(tokreader);

  // generate permutation of string in lexographical order
  string str="hello";
  auto permgen=genperm(str);
  genprint(permgen);
}

