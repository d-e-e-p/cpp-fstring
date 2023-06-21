/**
 * @file class_ctad.cpp
 * misc demo of cpp-fstring
 *
 * @ingroup examples
 *
 * @author Sandeep M
 * @copyright Copyright 2023 Sandeep M<deep@tensorfield.ag>
   @license MIT License
*/
#include <array>
#include <iostream>

#include "fstr.h"

// from
// https://stackoverflow.com/questions/65781641/im-trying-to-format-a-template-using-fmt
template <typename T, T Min, T Max>
class LimitedInt {
  T mValue{Min};

 public:
  explicit LimitedInt(const T value) { setValue(value); }
  void setValue(const T value) { mValue = value; }
  T getValue() const { return mValue; }
};

template <auto n>
struct Auto {
  int v = 1;
};


// from https://en.cppreference.com/w/cpp/language/class_template_argument_deduction
//
//
template <class T>
struct A {
  T t;

  struct {
    long a, b;
  } u;
};

template <class T>
struct B {
  T t;
  A<T> a;
};

template <auto... Values>
struct ValueList {
    static constexpr auto values = std::array{Values...};
};

int main()
{
  using std::cout;
  cout << "file: {__FILE_NAME__}\ntime: {__TIMESTAMP__}\n";

  A<int> a{1, {2, 3}};
  auto b = B<int>{1, {2, {3, 4}}};
  cout << "{a=}{b=}";
  cout << "{ValueList<1, 2, 3>()=}";
  cout << "{ValueList<'a', 'b', 'c'>()=}";
  cout << "{ValueList<true, false, false>()=}";
}
