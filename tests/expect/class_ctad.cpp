/**
 * @file class_ctad.cpp
 * misc demo of cpp-fstring
 *
 * @ingroup examples
 *
 * @author Sandeep M
 * @copyright Copyright 2023 Sandeep M<deep@tensorfield.ag>
 *  @license MIT License
 */
#include <array>
#include <iostream>

#include "fstr.h"
#include "utils.h"

// from
// https://stackoverflow.com/questions/65781641/im-trying-to-format-a-template-using-fmt
template <typename T, T Min, T Max>
class LimitedInt {
  T mValue{Min};

 public:
  explicit LimitedInt(const T value) { setValue(value); }
  void setValue(const T value) { mValue = value; }
  T getValue() const { return mValue; }
  // Generated to_string() for PUBLIC CLASS_TEMPLATE LimitedInt<T, Min, Max>
  public:
  auto to_string() const {
    const std::string fmt_string = "LimitedInt<T:={}, Min:={}, Max:={}>: T mValue={}";
    return fstr::format(fmt_string, fstr::get_type_name<T>(), Min, Max, mValue);
  }
};

template <auto n>
struct Auto {
  int v = 1;
  // Generated to_string() for PUBLIC CLASS_TEMPLATE Auto<n>
  public:
  auto to_string() const {
    const std::string fmt_string = "Auto<n:={}>: int v={}";
    return fstr::format(fmt_string, n, v);
  }
};


//
// from https://en.cppreference.com/w/cpp/language/class_template_argument_deduction
//
template <class T>
struct A {
  T t;

  struct {
    long a, b;
    // Generated to_string() for PUBLIC STRUCT_DECL A<T>::(unnamed struct)
  public:
  auto to_string() const {
    const std::string fmt_string = "A<T>::(unnamed struct): long a={}, b={}";
    return fstr::format(fmt_string, a, b);
  }
} u;
  // Generated to_string() for PUBLIC CLASS_TEMPLATE A<T>
  public:
  auto to_string() const {
    const std::string fmt_string = "A<T:={}>: T t={}, struct (unnamed struct) u={}";
    return fstr::format(fmt_string, fstr::get_type_name<T>(), t, u);
  }
};

template <class T>
struct B {
  T t;
  A<T> a;
  // Generated to_string() for PUBLIC CLASS_TEMPLATE B<T>
  public:
  auto to_string() const {
    const std::string fmt_string = "B<T:={}>: T t={}, A<T> a={}";
    return fstr::format(fmt_string, fstr::get_type_name<T>(), t, a);
  }
};


template <auto... Values>
struct ValueList {
    static constexpr auto values = std::array{Values...};
  // Generated to_string() for PUBLIC CLASS_TEMPLATE ValueList<Values>
  public:
  auto to_string() const {
    const std::string fmt_string = "ValueList<Values...>: const auto values={}";
    return fstr::format(fmt_string, values);
  }
};

int main()
{
  using std::cout;
  print_info(__FILE__, __TIMESTAMP__);

  A<int> a{1, {2, 3}};
  auto b = B<int>{1, {2, {3, 4}}};
  cout << fmt::format("a={}\nb={}\n", a, b);
  cout << fmt::format("ValueList<1, 2, 3>()={}\n", ValueList<1, 2, 3>());
  cout << fmt::format("ValueList<'a', 'b', 'c'>()={}\n", ValueList<'a', 'b', 'c'>());
  cout << fmt::format("ValueList<true, false, false>()={}\n", ValueList<true, false, false>());
}



