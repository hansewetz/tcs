#pragma once
#include <string>
#include <variant>
#include <iosfwd>
namespace tcs{

// class reprsenting what can be stored in a memory
// (data memory, stack etc.)
class MemElement{
public:
  // debug print operator
  friend std::ostream&operator<<(std::ostream&os,MemElement const&me);

  // types for each type that can be stored
  enum data_t{INT=0,STRING=1,CHAR=2,DOUBLE=3};

  // ctors, dtor, assign
  template<typename T>
  explicit MemElement(T t):type_(typeAux(t)),data_(t){}

  MemElement(MemElement const&me)=default;
  ~MemElement()=default;
  MemElement(MemElement&&me)=default;
  [[nodiscard]]MemElement&operator=(MemElement const&me)=default;
  [[nodiscard]]MemElement&operator=(MemElement&&me)=default;

  // getters
  [[nodiscard]]data_t type()const noexcept;

/*
  [[nodiscard]]int getInt()const;
  [[nodiscard]]std::string getString()const;
  [[nodiscard]]char getChar()const;
  [[nodiscard]]double getDouble()const;
*/

private:
  // helpers
  data_t typeAux(int){return INT;}
  data_t typeAux(std::string const&){return STRING;}
  data_t typeAux(char){return CHAR;}
  data_t typeAux(double){return DOUBLE;}

  // state
  data_t type_;
  std::variant<int,std::string,char,double>data_;
};
}
