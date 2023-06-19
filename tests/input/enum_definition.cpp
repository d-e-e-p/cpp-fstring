/**
 * @file enum_definition.cpp
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
#include <vector>

#include "fstr.h"

// unnamed unscoped enum can't be printed at present
  // would require generating with std::enable_if_t<std::is_same_v<T, decltype(x)>>
enum { x, y = x + 2 };

enum class Foo { a, b, c = 10, d, e = 1, f, g = f + c };
enum class Color { red, yellow, green = 20, blue };
struct X { enum direction { left = 'l', right = 'r' }; };
struct Y { enum E1 : int {}; };
enum E2 { e1 };
enum class Fruit { orange, apple, banana };
struct S { using enum Fruit; };
enum struct E11 { x, y };

std::vector<Foo> vc = {Foo::a, Foo::b, Foo::c, Foo::d, Foo::e, Foo::f, Foo::g};
std::map<Color, std::vector<Fruit>> mc = {
  {Color::red,    {Fruit::apple}},
  {Color::yellow, {Fruit::apple, Fruit::banana}},
};

enum class Shape { TRIANGLE, SQUARE, };
struct Point { int x; int y; };
std::map<Shape, std::vector<Point>> shapes = {
    {Shape::TRIANGLE, {{0, 0}, {1, 0}, {0, 1}}},
    {Shape::SQUARE, {{0, 0}, {1, 0}, {1, 1}, {0, 1}}},
};

enum class Key { Key1, Key2, Key3 };
enum class SubKey { SubKey1, SubKey2, SubKey3 };
enum class Value { Value1, Value2, Value3 };
std::map<Key, std::map<SubKey, Value>> data = {
    {Key::Key1, {{SubKey::SubKey1, Value::Value1}, {SubKey::SubKey2, Value::Value2}}},
    {Key::Key2, {{SubKey::SubKey3, Value::Value3}}},
    {Key::Key3, {}}
};

int main() {
  using std::cout;
  cout << "file: {__FILE_NAME__}\ntime: {__TIMESTAMP__}\n";

  cout << "x = " << x << "\n";
  // TODO(deep): make {x} work
  // std::cout << "{x=}\n";

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

  std::map<Shape, std::vector<Point>> {shapes=}

  std::map<Key, std::map<SubKey, Value>> {data=}

)";

}
