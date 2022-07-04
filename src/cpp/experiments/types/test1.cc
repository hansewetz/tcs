#include "Var2ValueCategoryTemplate.h"
#include <iostream>
#include <stdexcept>
using namespace std;
using namespace tcs;

struct Foo{
};

// test program
int main(){
  try{
    Foo u; 
    Foo&tl=u;   
    Foo&&tr=std::move(u);

    {
    // move since we pass an r-value
    Var2ValueCategoryTemplate vc1(std::move(tr));
    cout<<"tr (rvalue ref) (move):      "<<vc1<<endl;
    }
    {
    // we have an l-value - no ned to move
    Var2ValueCategoryTemplate vc1(u);
    cout<<"u (lvalue) (non-move): "<<vc1<<endl;
    }
    {
    // we have an l-value ref - no ned to move
    Var2ValueCategoryTemplate vc1(tl);
    cout<<"tl (lvalue ref) (non-move): "<<vc1<<endl;
    }
    {
    // we pass a pr-value which will be materizlied (via temporary materialization)  into an x-value
    Var2ValueCategoryTemplate vc1(2);
    cout<<"value (rvalue value) (non-move): "<<vc1<<endl;
    }
  }
  catch(std::exception const&e){
    cerr<<"cought exception: "<<e.what()<<endl;
    exit(1);
  }
  return 0;
}

