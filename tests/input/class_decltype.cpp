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
};
struct D {
  int d = 24;
  void echo_dumb(D const &) {}
};

int main()
{
  fmt::print("file: {__FILE_NAME__}\ntime: {__TIMESTAMP__}\n");

  fmt::print("{Bar<A, B>()=}");
  fmt::print("{Bar<C, D>()=}");

  return 0;
}
