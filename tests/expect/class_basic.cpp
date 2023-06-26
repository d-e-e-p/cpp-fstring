/**
 * @file class_basic.cpp
 * misc demo of cpp-fstring
 *
 * @ingroup examples
 *
 * @author Sandeep M
 * @copyright Copyright 2023 Sandeep M<deep@tensorfield.ag>
 *  @license MIT License
 */

#include <iostream>

#include "fstr.h"

struct Foo {
  int a = 32;
  int b[10] = {};
  // Generated to_string() for PUBLIC STRUCT_DECL Foo
  public:
  auto to_string() const {
    const std::string fmt_string = "Foo: int a={}, int[10] b={}";
    return fstr::format(fmt_string, a, b);
  }
};

struct Bar {
  char name[50] = "foo";
  int i = 10;
  double f = 3.14;
  Foo foo;
  // Generated to_string() for PUBLIC STRUCT_DECL Bar
  public:
  auto to_string() const {
    const std::string fmt_string = "Bar: char[50] name={}, int i={}, double f={}, Foo foo={}";
    return fstr::format(fmt_string, name, i, f, foo);
  }
};

class Rectangle {
  int width, height;

  public:
  void set_values(int, int);
  int area(void);
  Bar bar;
  // Generated to_string() for PUBLIC CLASS_DECL Rectangle
  public:
  auto to_string() const {
    const std::string fmt_string = "Rectangle: int width={}, height={}, Bar bar={}";
    return fstr::format(fmt_string, width, height, bar);
  }
} rect;

class Outer {
  struct {
    int a = 12;
    int b = 24;
    Rectangle r;
    // Generated to_string() for PRIVATE STRUCT_DECL Outer::(unnamed struct)
  public:
  auto to_string() const {
    const std::string fmt_string = "Outer::(unnamed struct): int a={}, b={}, Rectangle r={}";
    return fstr::format(fmt_string, a, b, r);
  }
} anon;
  // Generated to_string() for PUBLIC CLASS_DECL Outer
  public:
  auto to_string() const {
    const std::string fmt_string = "Outer: struct (unnamed struct) anon={}";
    return fstr::format(fmt_string, anon);
  }
} out;

int main()
{
  using std::cout;
  cout << fmt::format("file: {}\ntime: {}\n", __FILE_NAME__, __TIMESTAMP__);
  // can't print loc
  struct Local {
    int x = 0;
  } loc;

  cout << fmt::format("Outer()={}", Outer());
}



