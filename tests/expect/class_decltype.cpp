/**
 * @file class_decltype.cpp
 * misc demo of cpp-fstring
 *
 * @ingroup examples
 *
 * @author Sandeep M
 * @copyright Copyright 2023 Sandeep M<deep@tensorfield.ag>
 *   @license MIT License
 */
// from
// https://stackoverflow.com/questions/48456042/base-class-c14-template-functions-are-not-visible-in-clang-on-mac-os-recursi
//
#include <type_traits>
#include <utility>

#include "fstr.h"

// TODO(deep): Base class template functions don't get detected correctly in
// libclang

template <class U>
struct Foo {
  template <class V>
  decltype(std::declval<U>().echo_dumb(std::declval<V>())) dumb(V const &v)
  {
    u.echo_dumb(v);
  }
  U u;
  // Generated to_string() for PUBLIC CLASS_TEMPLATE Foo<U> 
  public:
  auto to_string() const {
    return fstr::format("Foo<U:={}>: U u={}\n", fstr::get_type_name<U>(), u);
  }
};

template <class T, class U>
struct Bar : public Foo<U> {
  using Foo<U>::dumb;

  template <class V>
  decltype(std::declval<T>().echo_dumb(std::declval<V>())) dumb(V const &v)
  {
    t.echo_dumb(v);
  }
  T t;
  // Generated to_string() for PUBLIC CLASS_TEMPLATE Bar<T, U> 
  public:
  auto to_string() const {
    return fstr::format("Bar<T:={}, U:={}>: T t={}, U u={}\n", fstr::get_type_name<T>(), fstr::get_type_name<U>(), t, this->u);
  }
};

struct A {
  void echo_dumb(A const &) {}
};
struct B {
  void echo_dumb(B const &) {}
};
struct C {
  int c = 42;
  void echo_dumb(C const &) {}
  // Generated to_string() for PUBLIC STRUCT_DECL C 
  public:
  auto to_string() const {
    return fstr::format("C: int c={}\n", c);
  }
};
struct D {
  int d = 24;
  void echo_dumb(D const &) {}
  // Generated to_string() for PUBLIC STRUCT_DECL D 
  public:
  auto to_string() const {
    return fstr::format("D: int d={}\n", d);
  }
};

int main()
{
  fmt::print(fmt::format("file: {}\ntime: {}\n", __FILE_NAME__, __TIMESTAMP__));

  fmt::print(fmt::format("Bar<A, B>()={}", Bar<A, B>()));
  fmt::print(fmt::format("Bar<C, D>()={}", Bar<C, D>()));

  return 0;
}



