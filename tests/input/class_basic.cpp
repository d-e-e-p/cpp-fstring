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
#include "utils.h"

struct Foo {
  int a = 32;
  int b[10] = {};
};

struct Bar {
  char name[50] = "foo";
  int i = 10;
  double f = 3.14;
  Foo foo;
};

class Rectangle {
  int width, height;

  public:
  void set_values(int, int);
  int area(void);
  Bar bar;
} rect;

class Outer {
  struct {
    int a = 12;
    int b = 24;
    Rectangle r;
  } anon;
} out;


int main()
{
  using std::cout;
  print_info(__FILE__, __TIMESTAMP__);

  // can't print loc
  struct Local {
    int x = 0;
  } loc;

  cout << "{Outer()=}\n";
}
