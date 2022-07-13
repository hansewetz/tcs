#include "apputils/cmdline/ProcNopCmd.h"
#include "apputils/cmdline/NopCmd.h"
#include <iostream>
using namespace std;
namespace tcs{
void ProcNopCmd::operator()(NopCmd const&cmd)const{
  cout<<"executing ProcNopCmd: "<<cmd<<endl;
}
}
