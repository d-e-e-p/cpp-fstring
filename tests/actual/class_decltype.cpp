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
#include <vector>
#include <variant>

#include "fstr.h"
#include "utils.h"

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
    const std::string fmt_string = "Foo<U:={}>: U u={}";
    return fstr::format(fmt_string, fstr::get_type_name<U>(), u);
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
    const std::string fmt_string = "Bar<T:={}, U:={}>: T t={}, U u={}";
    return fstr::format(fmt_string, fstr::get_type_name<T>(), fstr::get_type_name<U>(), t, this->u);
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
    const std::string fmt_string = "C: int c={}";
    return fstr::format(fmt_string, c);
  }
};
struct D {
  int d = 24;
  void echo_dumb(D const &) {}
  // Generated to_string() for PUBLIC STRUCT_DECL D
  public:
  auto to_string() const {
    const std::string fmt_string = "D: int d={}";
    return fstr::format(fmt_string, d);
  }
};

//  variant example
struct Circle {
    double radius;
  // Generated to_string() for PUBLIC STRUCT_DECL Circle
  public:
  auto to_string() const {
    const std::string fmt_string = "Circle: double radius={}";
    return fstr::format(fmt_string, radius);
  }
};

struct Rectangle {
    double width;
    double height;
  // Generated to_string() for PUBLIC STRUCT_DECL Rectangle
  public:
  auto to_string() const {
    const std::string fmt_string = "Rectangle: double width={}, height={}";
    return fstr::format(fmt_string, width, height);
  }
};

struct Triangle {
    double base;
    double height;
  // Generated to_string() for PUBLIC STRUCT_DECL Triangle
  public:
  auto to_string() const {
    const std::string fmt_string = "Triangle: double base={}, height={}";
    return fstr::format(fmt_string, base, height);
  }
};


int main()
{
  print_info(__FILE__, __TIMESTAMP__);

  fmt::print(fmt::format("Bar<A, B>()={}\n", Bar<A, B>()));
  fmt::print(fmt::format("Bar<C, D>()={}\n", Bar<C, D>()));

  using Shape = std::variant<Circle, Rectangle, Triangle>;
  std::vector<Shape> shapes;
  shapes.push_back(Circle{2.5});
  shapes.push_back(Rectangle{3.0, 4.0});
  shapes.push_back(Triangle{5.0, 6.0});
  fmt::print(fmt::format("shapes={}\n", shapes));

  return 0;
}



