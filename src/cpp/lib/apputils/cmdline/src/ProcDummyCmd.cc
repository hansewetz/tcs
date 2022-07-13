#include "apputils/cmdline/ProcDummyCmd.h"
#include "apputils/cmdline/DummyCmd.h"
#include <iostream>
using namespace std;
namespace tcs{
void ProcDummyCmd::operator()(DummyCmd const&cmd)const{
  cout<<"executing ProcDummyCmd: "<<cmd<<endl;
}
}

