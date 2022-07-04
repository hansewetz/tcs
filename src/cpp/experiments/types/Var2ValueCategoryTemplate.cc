#include "Var2ValueCategoryTemplate.h"
#include <iostream>
using namespace std;
namespace tcs{

const map<Var2ValueCategoryTemplate::valcat_t,string>Var2ValueCategoryTemplate::valcat2string_{
  {lvalue,"lvalue"},
  {xvalue,"xvalue"},
  {rvalue,"rvalue"},
  {prvalue,"prvalue"}
};
ostream&operator<<(ostream&os,Var2ValueCategoryTemplate const&vc){
  return os<<"param-value-category: "<<vc.valcat2string()<<", template-type: "<<vc.templateType()<<", param-type: "<<vc.paramType();
}
Var2ValueCategoryTemplate::valcat_t Var2ValueCategoryTemplate::operator()()const noexcept{
  return valcat_;
}
string const&Var2ValueCategoryTemplate::valcat2string()const{
  return valcat2string_.find(valcat_)->second;
}
string const&Var2ValueCategoryTemplate::templateType()const noexcept{
  return templateType_;
}
string const&Var2ValueCategoryTemplate::paramType()const noexcept{
  return paramType_;
}
}
