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

// Anonymous Class : Class not having a name
struct {
  int i = 3;
  struct {
    int j = 2;
    struct {
      int k = 1;
      struct {
        int l = 0;
        // Generated to_string() for PUBLIC STRUCT_DECL (unnamed struct) 
  public:
  auto to_string() const {
    return fstr::format("(unnamed struct): int l={}\n", l);
  }
} obj0 ;
      // Generated to_string() for PUBLIC STRUCT_DECL (unnamed struct) 
  public:
  auto to_string() const {
    return fstr::format("(unnamed struct): int k={}, struct (unnamed struct) obj0={}\n", k, obj0);
  }
} obj1 ;
    // Generated to_string() for PUBLIC STRUCT_DECL (unnamed struct) 
  public:
  auto to_string() const {
    return fstr::format("(unnamed struct): int j={}, struct (unnamed struct) obj1={}\n", j, obj1);
  }
} obj2 ;
  // Generated to_string() for PUBLIC STRUCT_DECL (unnamed struct) 
  public:
  auto to_string() const {
    return fstr::format("(unnamed struct): int i={}, struct (unnamed struct) obj2={}\n", i, obj2);
  }
} obj3;

// test of namespace and inheritance
struct Base {
  int i;
  // Generated to_string() for PUBLIC STRUCT_DECL Base 
  public:
  auto to_string() const {
    return fstr::format("Base: int i={}\n", i);
  }
};

namespace a::b::c {
struct enclose {
  struct outer {
    struct inner {
      static const int x;
      int y;
      Base b;
      // Generated to_string() for PUBLIC STRUCT_DECL a::b::c::enclose::outer::inner 
  public:
  auto to_string() const {
    return fstr::format("a::b::c::enclose::outer::inner: const int x={}, int y={}, Base b={}\n", x, y, b);
  }
} foo;
    // Generated to_string() for PUBLIC STRUCT_DECL a::b::c::enclose::outer 
  public:
  auto to_string() const {
    return fstr::format("a::b::c::enclose::outer: struct inner foo={}\n", foo);
  }
} bar;
  // Generated to_string() for PUBLIC STRUCT_DECL a::b::c::enclose 
  public:
  auto to_string() const {
    return fstr::format("a::b::c::enclose: struct outer bar={}\n", bar);
  }
} obj1;

}  // end namespace a::b::c
const int a::b::c::enclose::outer::inner::x = 42;

int main()
{
  using std::cout;
  cout << fmt::format("file: {}\ntime: {}\n", __FILE_NAME__, __TIMESTAMP__);

  // (unnamed struct) !
  cout << fmt::format("obj3={}", obj3);

  a::b::c::obj1.bar.foo.y = 24;
  cout << fmt::format(" a::b::c::obj1={}", a::b::c::obj1);
  return 0;
}



