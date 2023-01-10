#include "tcsvm/tcsvm-mem/MemElement.h"
#include <iostream>

using namespace std;
namespace tcs{

// debug print operator
ostream&operator<<(ostream&os,MemElement const&me){
  // NOTE! not yet done
  return os;
}
// ctors, dtor, assign
/*
MemElement::MemElement(string const&data):type_(STRING),data_(data){
}
MemElement::MemElement(int data):type_(INT),data_(data){
}
*/
// get type of data stored in element
MemElement::data_t MemElement::type()const noexcept{
  return type_;
}
/*
// get int (throws if INT is not stored)
int MemElement::getInt()const{
  // NOTE! not yet done
  return 0;
}
// get string (throws if STRING is not stored)
string MemElement::getString()const{
  // NOTE! not yet done
  return "empty";
}
*/
// NOTE! not yet done
// ...
}
