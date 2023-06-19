/**
 * @file class_crtp.cpp
 * misc demo of cpp-fstring
 *
 * @ingroup examples
 *
 * @author Sandeep M
 * @copyright Copyright 2023 Sandeep M<deep@tensorfield.ag>
   @license MIT License
*/
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "fstr.h"

//
// from:
// https://stackoverflow.com/questions/42795408/can-libclang-parse-the-crtp-pattern
// https://stackoverflow.com/questions/41333185/python-clang-getting-template-arguments
//
//


namespace A {

template<class T>
class TBase {
  public:
  int tbase = 0;
};
class X1 : public TBase<X1> {
  public:
  int x1=0;
};
class CBase {
  public:
  int cbase=0;
};
class X2 : public CBase {
  public:
  int x2=0;
};

} // end namespace A

namespace B {

template<class T>
class TBase {
  int tbase = 0;
};
class Y1 : public TBase<Y1> {
  int y1=0;
};
class CBase {
  int cbase=0;
};
class Y2 : public CBase {
  int y2=0;
};

} // end namespace B

int main() {
  using std::cout;
  cout << "file: {__FILE_NAME__}\ntime: {__TIMESTAMP__}\n";

  auto x1 = A::X1();
  auto x2 = A::X2();

  cout << " {A::CBase()=} ";
  cout << " {A::TBase<A::CBase>()=} ";
  cout << " base {x1=} ";
  cout << " derived {x2=} ";

  auto y1 = B::Y1();
  auto y2 = B::Y2();

  // see https://cplusplus.com/doc/tutorial/inheritance/
  cout << " base {y1=} ";
  cout << " derived {y2=} ";


  return 0;
}
