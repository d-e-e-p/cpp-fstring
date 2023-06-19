/**
 * @file class_basic.cpp
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


struct Foo {
  int a = 32;
  int b[10] = {};
// Generated to_string for PUBLIC STRUCT_DECL Foo
  public:
  auto to_string() const {
    return fstr::format(R"( Foo: int a={}, int[10] b={}
)", a, b);
  }
};

struct Bar {
    char name[50] = "foo" ;
    int i = 10;
    double f = 3.14;
    Foo foo;
// Generated to_string for PUBLIC STRUCT_DECL Bar
  public:
  auto to_string() const {
    return fstr::format(R"( Bar: char[50] name={}, int i={}, double f={}, Foo foo={}
)", name, i, f, foo);
  }
};

class Rectangle {
    int width, height;
  public:
    void set_values (int,int);
    int area (void);
    Bar bar;
// Generated to_string for PUBLIC CLASS_DECL Rectangle
  public:
  auto to_string() const {
    return fstr::format(R"( Rectangle: int width={}, height={}, Bar bar={}
)", width, height, bar);
  }
} rect;

class Outer {
  struct {
    int a = 12;
    int b = 24;
    Rectangle r;
  // Generated to_string for PRIVATE STRUCT_DECL Outer::(unnamed struct at input/class_basic.cpp:40:3)
  public:
  auto to_string() const {
    return fstr::format(R"( Outer::(unnamed struct at input/class_basic.cpp:40:3): int a={}, b={}, Rectangle r={}
)", a, b, r);
  }
} anon;
// Generated to_string for PUBLIC CLASS_DECL Outer
  public:
  auto to_string() const {
    return fstr::format(R"( Outer: int anon.a={}, anon.b={}, Rectangle anon.r={}
)", this->anon.a, this->anon.b, this->anon.r);
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
