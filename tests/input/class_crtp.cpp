/**
 * @file class_crtp.cpp
 * misc demo of cpp-fstring
 *
 * @ingroup examples
 *
 * @author Sandeep M
 * @copyright Copyright 2023 Sandeep M<deep@tensorfield.ag>
 * @license MIT License
 */
#include <iostream>
#include <string>

#include "fstr.h"
#include "utils.h"

//
// from:
// https://stackoverflow.com/questions/42795408/can-libclang-parse-the-crtp-pattern
// https://stackoverflow.com/questions/41333185/python-clang-getting-template-arguments
//
//

namespace A {

template <class T>
class TBase {
 public:
  int tbase = 0;
};
class X1 : public TBase<X1> {
 public:
  int x1 = 0;
};
class CBase {
 public:
  int cbase = 0;
};
class X2 : public CBase {
 public:
  int x2 = 0;
};

}  // end namespace A

namespace B {

template <class T>
class TBase {
  int tbase = 0;
};
class Y1 : public TBase<Y1> {
  int y1 = 0;
};
class CBase {
  int cbase = 0;
};
class Y2 : public CBase {
  int y2 = 0;
};

}  // end namespace B

int main()
{
  using std::cout;
  print_info(__FILE__, __TIMESTAMP__);

  auto x1 = A::X1();
  auto x2 = A::X2();

  cout << " {A::CBase()=}\n";
  cout << " {A::TBase<A::CBase>()=}\n";
  cout << " base {x1=}\n";
  cout << " derived {x2=}\n";

  auto y1 = B::Y1();
  auto y2 = B::Y2();

  // see https://cplusplus.com/doc/tutorial/inheritance/
  cout << " base {y1=}\n";
  cout << " derived {y2=}\n";

  return 0;
}
