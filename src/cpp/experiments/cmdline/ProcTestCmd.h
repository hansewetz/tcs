#pragma once
#include "TestCmd.h"
#include <iostream>
namespace tcs{

// command processor for Test command
struct ProcTestCmd{
  void operator()(TestCmd const&cmd)const{
    std::cerr<<"processing TestCmd: "<<cmd<<std::endl;
  }
};
}
