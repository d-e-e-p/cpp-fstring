/**
 * @file class_namespace1.cpp
 * misc demo of cpp-fstring
 *
 * @ingroup examples
 *
 * @author Sandeep M
 * @copyright Copyright 2023 Sandeep M<deep@tensorfield.ag>
   @license MIT License
*/
#include <iostream>

#include "fstr.h"

// Anonymous Class : Class is not having any name
struct {
  int i;
} obj0;

struct Base {
  int i;
};

namespace a::b::c {
struct enclose {
  struct outer {
    struct inner {
      static int x;
      int y;
      Base b;
    } foo;
  } bar;
} obj1;

}  // end namespace a::b::c
int a::b::c::enclose::outer::inner::x = 42;

int main()
{
  using std::cout;
  cout << "file: {__FILE_NAME__}\ntime: {__TIMESTAMP__}\n";
  a::b::c::obj1.bar.foo.y = 24;
  cout << " {a::b::c::obj1=} \n";
  return 0;
}
