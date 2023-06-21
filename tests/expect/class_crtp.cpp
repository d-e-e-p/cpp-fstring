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
#include <string>

#include "fstr.h"

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
  // Generated to_string() for PUBLIC CLASS_TEMPLATE A::TBase<T>
  public:
  auto to_string() const {
    return fstr::format("A::TBase<T:={}>: int tbase={}\n", fstr::get_type_name<T>(), tbase);
  }
};
class X1 : public TBase<X1> {
 public:
  int x1 = 0;
  // Generated to_string() for PUBLIC CLASS_DECL A::X1
  public:
  auto to_string() const {
    return fstr::format(": int x1={}\n", x1);
  }
};
class CBase {
 public:
  int cbase = 0;
  // Generated to_string() for PUBLIC CLASS_DECL A::CBase
  public:
  auto to_string() const {
    return fstr::format(": int cbase={}\n", cbase);
  }
};
class X2 : public CBase {
 public:
  int x2 = 0;
  // Generated to_string() for PUBLIC CLASS_DECL A::X2
  public:
  auto to_string() const {
    return fstr::format(": int x2={}, cbase={}\n", x2, this->cbase);
  }
};

}  // end namespace A

namespace B {

template <class T>
class TBase {
  int tbase = 0;
  // Generated to_string() for PUBLIC CLASS_TEMPLATE B::TBase<T>
  public:
  auto to_string() const {
    return fstr::format("B::TBase<T:={}>: int tbase={}\n", fstr::get_type_name<T>(), tbase);
  }
};
class Y1 : public TBase<Y1> {
  int y1 = 0;
  // Generated to_string() for PUBLIC CLASS_DECL B::Y1
  public:
  auto to_string() const {
    return fstr::format(": int y1={}\n", y1);
  }
};
class CBase {
  int cbase = 0;

  friend class Y2;
  // Generated to_string() for PUBLIC CLASS_DECL B::CBase
  public:
  auto to_string() const {
    return fstr::format(": int cbase={}\n", cbase);
  }
};
class Y2 : public CBase {
  int y2 = 0;
  // Generated to_string() for PUBLIC CLASS_DECL B::Y2
  public:
  auto to_string() const {
    return fstr::format(": int y2={}, cbase={}\n", y2, this->cbase);
  }
};

}  // end namespace B

int main()
{
  using std::cout;
  cout << fmt::format("file: {}\ntime: {}\n", __FILE_NAME__, __TIMESTAMP__);

  auto x1 = A::X1();
  auto x2 = A::X2();

  cout << fmt::format(" A::CBase()={} ", A::CBase());
  cout << fmt::format(" A::TBase<A::CBase>()={} ", A::TBase<A::CBase>());
  cout << fmt::format(" base x1={} ", x1);
  cout << fmt::format(" derived x2={} ", x2);

  auto y1 = B::Y1();
  auto y2 = B::Y2();

  // see https://cplusplus.com/doc/tutorial/inheritance/
  cout << fmt::format(" base y1={} ", y1);
  cout << fmt::format(" derived y2={} ", y2);

  return 0;
}



