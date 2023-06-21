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
struct X {
  enum direction { left = 'l', right = 'r' };
// Generated formatter for PUBLIC enum X::direction of type UINT 
 friend constexpr auto format_as(const X::direction obj) {
  fmt::string_view name = "<missing>";
  switch (obj) {
    case X::left : name = "left" ; break;  // index=108
    case X::right: name = "right"; break;  // index=114
  }
  return name;
}
};
struct Y {
  enum E1 : int {};
// Generated formatter for PUBLIC enum Y::E1 of type INT 
 friend constexpr auto format_as(const Y::E1 obj) {
  fmt::string_view name = "<missing>";
  switch (obj) {
  }
  return name;
}
};
enum E2 { e1 };
enum class Fruit { orange, apple, banana };
struct S {
  using enum Fruit;
};
enum struct E11 { x, y };

std::vector<Foo> vc = {Foo::a, Foo::b, Foo::c, Foo::d, Foo::e, Foo::f, Foo::g};
std::map<Color, std::vector<Fruit>> mc = {
    {Color::red, {Fruit::apple}},
    {Color::yellow, {Fruit::apple, Fruit::banana}},
};

enum class Shape {
  TRIANGLE,
  SQUARE,
};
struct Point {
  int x;
  int y;
  // Generated to_string() for PUBLIC STRUCT_DECL Point
  public:
  auto to_string() const {
    return fstr::format(": int x={}, y={}\n", x, y);
  }
};
std::map<Shape, std::vector<Point>> shapes = {
    {Shape::TRIANGLE, {{0, 0}, {1, 0}, {0, 1}}},
    {Shape::SQUARE, {{0, 0}, {1, 0}, {1, 1}, {0, 1}}},
};

enum class Key { Key1, Key2, Key3 };
enum class SubKey { SubKey1, SubKey2, SubKey3 };
enum class Value { Value1, Value2, Value3 };
std::map<Key, std::map<SubKey, Value>> data = {
    {Key::Key1,
     {{SubKey::SubKey1, Value::Value1}, {SubKey::SubKey2, Value::Value2}}},
    {Key::Key2, {{SubKey::SubKey3, Value::Value3}}},
    {Key::Key3, {}}};

int main()
{
  using std::cout;
  cout << fmt::format("file: {}\ntime: {}\n", __FILE_NAME__, __TIMESTAMP__);

  cout << "x = " << x << "\n";
  // TODO(deep): make {x} work
  // std::cout << "{x=}\n";

  cout << fmt::format(R"(

  Foo::b={} Foo::e={}

  Color::yellow={}
  X::left={} X::right={}
  e1={}
  Fruit::apple={}
  S::apple={}
  E11::x={}

  vector<Foo> vc = {}
  map<Color, vector<Fruit>> mc = {}

  std::map<Shape, std::vector<Point>> shapes={}

  std::map<Key, std::map<SubKey, Value>> data={}

)", Foo::b, Foo::e, Color::yellow, X::left, X::right, e1, Fruit::apple, S::apple, E11::x, vc, mc, shapes, data);
}

// Generated formatter for PUBLIC enum Foo of type INT scoped
constexpr auto format_as(const Foo obj) {
  fmt::string_view name = "<missing>";
  switch (obj) {
    case Foo::a: name = "a"; break;  // index=0
    case Foo::b: name = "b"; break;  // index=1
    case Foo::c: name = "c"; break;  // index=10
    case Foo::d: name = "d"; break;  // index=11
//  case Foo::e: name = "e"; break;  // index=1 <-- index is duplicate
    case Foo::f: name = "f"; break;  // index=2
    case Foo::g: name = "g"; break;  // index=12
  }
  return name;
}
// Generated formatter for PUBLIC enum Color of type INT scoped
constexpr auto format_as(const Color obj) {
  fmt::string_view name = "<missing>";
  switch (obj) {
    case Color::red   : name = "red"   ; break;  // index=0
    case Color::yellow: name = "yellow"; break;  // index=1
    case Color::green : name = "green" ; break;  // index=20
    case Color::blue  : name = "blue"  ; break;  // index=21
  }
  return name;
}
// Generated formatter for PUBLIC enum E2 of type UINT 
constexpr auto format_as(const E2 obj) {
  fmt::string_view name = "<missing>";
  switch (obj) {
    case e1: name = "e1"; break;  // index=0
  }
  return name;
}
// Generated formatter for PUBLIC enum Fruit of type INT scoped
constexpr auto format_as(const Fruit obj) {
  fmt::string_view name = "<missing>";
  switch (obj) {
    case Fruit::orange: name = "orange"; break;  // index=0
    case Fruit::apple : name = "apple" ; break;  // index=1
    case Fruit::banana: name = "banana"; break;  // index=2
  }
  return name;
}
// Generated formatter for PUBLIC enum E11 of type INT scoped
constexpr auto format_as(const E11 obj) {
  fmt::string_view name = "<missing>";
  switch (obj) {
    case E11::x: name = "x"; break;  // index=0
    case E11::y: name = "y"; break;  // index=1
  }
  return name;
}
// Generated formatter for PUBLIC enum Shape of type INT scoped
constexpr auto format_as(const Shape obj) {
  fmt::string_view name = "<missing>";
  switch (obj) {
    case Shape::TRIANGLE: name = "TRIANGLE"; break;  // index=0
    case Shape::SQUARE  : name = "SQUARE"  ; break;  // index=1
  }
  return name;
}
// Generated formatter for PUBLIC enum Key of type INT scoped
constexpr auto format_as(const Key obj) {
  fmt::string_view name = "<missing>";
  switch (obj) {
    case Key::Key1: name = "Key1"; break;  // index=0
    case Key::Key2: name = "Key2"; break;  // index=1
    case Key::Key3: name = "Key3"; break;  // index=2
  }
  return name;
}
// Generated formatter for PUBLIC enum SubKey of type INT scoped
constexpr auto format_as(const SubKey obj) {
  fmt::string_view name = "<missing>";
  switch (obj) {
    case SubKey::SubKey1: name = "SubKey1"; break;  // index=0
    case SubKey::SubKey2: name = "SubKey2"; break;  // index=1
    case SubKey::SubKey3: name = "SubKey3"; break;  // index=2
  }
  return name;
}
// Generated formatter for PUBLIC enum Value of type INT scoped
constexpr auto format_as(const Value obj) {
  fmt::string_view name = "<missing>";
  switch (obj) {
    case Value::Value1: name = "Value1"; break;  // index=0
    case Value::Value2: name = "Value2"; break;  // index=1
    case Value::Value3: name = "Value3"; break;  // index=2
  }
  return name;
}


