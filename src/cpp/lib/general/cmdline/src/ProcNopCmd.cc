#include "general/cmdline/ProcNopCmd.h"
#include "general/cmdline/NopCmd.h"
#include <iostream>
using namespace std;
namespace tcs{
void ProcNopCmd::operator()(NopCmd const&cmd)const{
  cout<<"executing ProcNopCmd: "<<cmd<<endl;
}
}
