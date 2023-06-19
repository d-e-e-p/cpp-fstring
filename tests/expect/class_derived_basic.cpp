/**
 * @file class_derived_basic.cpp
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


struct Base {
  std::string bname = "base";
  int a = 4;
// Generated to_string for PUBLIC STRUCT_DECL Base
  public:
  auto to_string() const {
    return fstr::format(R"( Base: int bname={}, a={}
)", bname, a);
  }
};

struct Foo {
  char name[50] = "foo" ;
  Base b;
// Generated to_string for PUBLIC STRUCT_DECL Foo
  public:
  auto to_string() const {
    return fstr::format(R"( Foo: char[50] name={}, Base b={}
)", name, b);
  }
};

struct Bar: Base {
  char name[50] = "bar" ;
// Generated to_string for PUBLIC STRUCT_DECL Bar
  public:
  auto to_string() const {
    return fstr::format(R"( Bar: char[50] name={}, int bname={}, a={}
)", name, this->bname, this->a);
  }
};


int main()
{
  using std::cout;
  cout << fmt::format("file: {}\ntime: {}\n", __FILE_NAME__, __TIMESTAMP__);
  cout << fmt::format(" Foo()={} Bar()={} \n", Foo(), Bar());
}


