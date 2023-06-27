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
        // Generated to_string() for PUBLIC STRUCT_DECL (unnamed struct)
  public:
  auto to_string() const {
    const std::string fmt_string = "(unnamed struct): int l={}";
    return fstr::format(fmt_string, l);
  }
} obj0 ;
      // Generated to_string() for PUBLIC STRUCT_DECL (unnamed struct)
  public:
  auto to_string() const {
    const std::string fmt_string = "(unnamed struct): int k={}, struct (unnamed struct) obj0={}";
    return fstr::format(fmt_string, k, obj0);
  }
} obj1 ;
    // Generated to_string() for PUBLIC STRUCT_DECL (unnamed struct)
  public:
  auto to_string() const {
    const std::string fmt_string = "(unnamed struct): int j={}, struct (unnamed struct) obj1={}";
    return fstr::format(fmt_string, j, obj1);
  }
} obj2 ;
  // Generated to_string() for PUBLIC STRUCT_DECL (unnamed struct)
  public:
  auto to_string() const {
    const std::string fmt_string = "(unnamed struct): int i={}, struct (unnamed struct) obj2={}";
    return fstr::format(fmt_string, i, obj2);
  }
} obj3;

// test of namespace and inheritance
struct Base {
  int i;
  // Generated to_string() for PUBLIC STRUCT_DECL Base
  public:
  auto to_string() const {
    const std::string fmt_string = "Base: int i={}";
    return fstr::format(fmt_string, i);
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
    const std::string fmt_string = "a::b::c::enclose::outer::inner: const int x={}, int y={}, Base b={}";
    return fstr::format(fmt_string, x, y, b);
  }
} foo;
    // Generated to_string() for PUBLIC STRUCT_DECL a::b::c::enclose::outer
  public:
  auto to_string() const {
    const std::string fmt_string = "a::b::c::enclose::outer: struct inner foo={}";
    return fstr::format(fmt_string, foo);
  }
} bar;
  // Generated to_string() for PUBLIC STRUCT_DECL a::b::c::enclose
  public:
  auto to_string() const {
    const std::string fmt_string = "a::b::c::enclose: struct outer bar={}";
    return fstr::format(fmt_string, bar);
  }
} obj1;

}  // end namespace a::b::c
const int a::b::c::enclose::outer::inner::x = 42;

int main()
{
  using std::cout;
  print_info(__FILE__, __TIMESTAMP__);

  // (unnamed struct) !
  cout << fmt::format("obj3={}\n", obj3);

  a::b::c::obj1.bar.foo.y = 24;
  cout << fmt::format(" a::b::c::obj1={}\n", a::b::c::obj1);
  return 0;
}



