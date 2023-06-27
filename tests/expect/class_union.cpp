/**
 * @file class_union.cpp
 * misc demo of cpp-fstring
 *
 * @ingroup examples
 *
 * @author Sandeep M
 * @copyright Copyright 2023 Sandeep M<deep@tensorfield.ag>
 *   @license MIT License
 */
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "fstr.h"
#include "utils.h"

struct Base {
  int a;
  union {
    int i;
    double d;
    char c;
    // Generated to_string() for PUBLIC UNION_DECL Base::(unnamed union)
  public:
  auto to_string() const {
    const std::string fmt_string = "Base::(unnamed union): int i={}, double d={}, char c={}";
    return fstr::format(fmt_string, i, d, c);
  }
} u;
  // Generated to_string() for PUBLIC STRUCT_DECL Base
  public:
  auto to_string() const {
    const std::string fmt_string = "Base: int a={}, union (unnamed union) u={}";
    return fstr::format(fmt_string, a, u);
  }
} b;

union Onion {
  int i;
  double d;
  char c;
  // Generated to_string() for PUBLIC UNION_DECL Onion
  public:
  auto to_string() const {
    const std::string fmt_string = "Onion: int i={}, double d={}, char c={}";
    return fstr::format(fmt_string, i, d, c);
  }
} u;

int main()
{
  using std::cout;
  print_info(__FILE__, __TIMESTAMP__);

  cout << fmt::format(" b={} \n", b);
  u.i = 10;
  u.d = 4.2;
  u.c = 'u';
  cout << "u.i = " << u.i << " u.d = " << u.d << " u.c = " << u.c << " \n";
  cout << fmt::format(" u={} \n", u);
}



