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
};

struct Foo {
  char name[50] = "foo";
  Base b;
};

struct Bar : Base {
  char name[50] = "bar";
};

int main()
{
  using std::cout;
  cout << "file: {__FILE_NAME__}\ntime: {__TIMESTAMP__}\n";
  cout << " {Foo()=} {Bar()=} \n";
}
