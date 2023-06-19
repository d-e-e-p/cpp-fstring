/**
 * @file enum_include.h
 * misc demo of cpp-fstring
 *
 * @ingroup examples
 *
 * @author Sandeep M
 * @copyright Copyright 2023 Sandeep M<deep@tensorfield.ag>
   @license MIT License
*/


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

