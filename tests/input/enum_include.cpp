/**
 * @file enum_include.cpp
 * misc demo of cpp-fstring
 *
 * @ingroup examples
 *
 * @author Sandeep M
 * @copyright Copyright 2023 Sandeep M<deep@tensorfield.ag>
 *   @license MIT License
 */

#include "enum_include.h"

#include <iostream>
#include <map>

#include "fstr.h"

int main()
{
  using std::cout;
  cout << "file: {__FILE_NAME__}\ntime: {__TIMESTAMP__}\n";

  // TODO(deep):  make {x} work?
  cout << "x = " << x << "\n";

  cout << R"(

  {Foo::b=} {Foo::e=}

  {Color::yellow=}
  {X::left=} {X::right=}
  {e1=}
  {Fruit::apple=}
  {S::apple=}
  {E11::x=}

  vector<Foo> vc = {vc}
  map<Color, vector<Fruit>> mc = {mc}

)";
}
