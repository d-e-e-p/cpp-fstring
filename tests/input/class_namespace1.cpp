/**
 * @file class_namespace1.cpp
 * misc demo of cpp-fstring
 *
 * @ingroup examples
 *
 * @author Sandeep M
 * @copyright Copyright 2023 Sandeep M<deep@tensorfield.ag>
 *   @license MIT License
 */
#include <iostream>

#include "fstr.h"
#include "utils.h"

// Anonymous Class : Class not having a name
struct {
  int i = 3;
  struct {
    int j = 2;
    struct {
      int k = 1;
      struct {
        int l = 0;
      } obj0 ;
    } obj1 ;
  } obj2 ;
} obj3;

// test of namespace and inheritance
struct Base {
  int i;
};

namespace a::b::c {
struct enclose {
  struct outer {
    struct inner {
      static const int x;
      int y;
      Base b;
    } foo;
  } bar;
} obj1;

}  // end namespace a::b::c
const int a::b::c::enclose::outer::inner::x = 42;

int main()
{
  using std::cout;
  print_info(__FILE__, __TIMESTAMP__);

  // (unnamed struct) !
  cout << "{obj3=}\n";

  a::b::c::obj1.bar.foo.y = 24;
  cout << " {a::b::c::obj1=}\n";
  return 0;
}
