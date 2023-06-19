/**
 * @file enum_basic.cpp
 * misc demo of cpp-fstring
 *
 * @ingroup examples
 *
 * @author Sandeep M
 * @copyright Copyright 2023 Sandeep M<deep@tensorfield.ag>
   @license MIT License
*/
/**
 * @file  enum_basic.cpp
 * @author  sandeep <deep@tensorfield.ag>
 * @version 1.0
 *
 * @section LICENSE
 *
 * MIT License <http://opensource.org/licenses/MIT>
 *
 * @section DESCRIPTION
 *
 *
 * https://github.com/d-e-e-p/cpp-fstring-examples
 * Copyright (c) 2023 sandeep <deep@tensorfield.ag>
 *
 */

#include <cstdint>         // for uint64_t, uint8_t
#include <iostream>        // for operator<<, cout
#include <limits>          // for numeric_limits
#include <map>             // for operator!=, map
#include <string>          // for basic_string, string_literals
#include <vector>          // for vector

#include "fstr.h"

using namespace std::string_literals;

// from https://en.cppreference.com/w/cpp/language/enum
// The name of an unscoped enumeration may be omitted: such declaration only introduces the enumerators into the enclosing scope:
enum { a, b, c = 0, d = a + 2 }; // defines a = 0, b = 1, c = 0, d = 2

struct X {
    enum direction3 { left = 'l', right = 'r' };
// Generated formatter for PUBLIC enum X::direction3 of type UINT 
 friend constexpr auto format_as(const X::direction3 obj) {
  fmt::string_view name = "<missing>";
  switch (obj) {
    case X::left : name = "left" ; break;  // index=108
    case X::right: name = "right"; break;  // index=114
  }
  return name;
}
};

// test cases from:
// https://github.com/Neargye/magic_enum/blob/master/test/test.cpp
// Copyright (c) 2019 - 2023 Daniil Goncharov <neargye@gmail.com>
enum class Color1 { RED = -12, GREEN = 7, BLUE = 15 };
enum Color2 { RED = -12, GREEN = 7, BLUE = 15 };
enum class Color3 : unsigned char  { RED = u'r', GREEN = u'g', BLUE = u'b' };
enum Directions { Up = 85, Down = -42, Right = 120, Left = -120 };
enum class crc_hack {
  b5a7b602ab754d7ab30fb42c4fb28d82
};

enum class crc_hack_2 {
  b5a7b602ab754d7ab30fb42c4fb28d82,
  d19f2e9e82d14b96be4fa12b8a27ee9f
};

enum class MaxUsedAsInvalid : std::uint8_t {
  ONE,
  TWO = 63,
  INVALID = std::numeric_limits<std::uint8_t>::max()
};

enum class Binary : bool {
  ONE,
  TWO
};

enum class Number1 : int { one = 1, two, three, many = 127 };
enum Number2 : uint64_t {
  one = 100,
  two = 200,
  three = 300,
  four = 400,
  many = std::numeric_limits<std::uint64_t>::max()
};
enum class Number3 : int {
  one = 1 << 1,
  two = 1 << 2,
  three = 1 << 3,
  many = 1 << 30,
};

enum Dir : std::uint64_t {
  L = std::uint64_t{1} << 10,
  D = std::uint64_t{1} << 20,
  U = std::uint64_t{1} << 31,
  R = std::uint64_t{1} << 63,
};

namespace space {
enum class rangers {
  humans, maloqs, pelengs, faeyans, gaalians
};
}


int main() {
  using std::cout;
  cout << fmt::format("file: {}\ntime: {}\n", __FILE_NAME__, __TIMESTAMP__);

  //TODO(deep): get {a} working...
  cout << fmt::format(R"(
  Color1::GREEN={}
  GREEN={}
  L={} D={} U={} R={}
  Number1::many={}
  Number2::many={}
  Number3::one={} Number3::two={} Number3::three={} Number3::many={}
  Binary::ONE={}
  space::rangers::humans={}
)", Color1::GREEN, GREEN, L, D, U, R, Number1::many, Number2::many, Number3::one, Number3::two, Number3::three, Number3::many, Binary::ONE, space::rangers::humans);


  std::vector<Color1> vc = {Color1::RED, Color1::GREEN, Color1::BLUE};
  std::map<Color1, std::vector<Number3>> mc = {
    {Color1::RED,   {Number3::one, Number3::two}},
    {Color1::GREEN, {Number3::three}},
    {Color1::BLUE,  {Number3::many}}
  };

  cout << fmt::format("\n vc={} \n mc={}\n", vc, mc);

}

// Generated formatter for PUBLIC enum Color1 of type INT scoped
constexpr auto format_as(const Color1 obj) {
  fmt::string_view name = "<missing>";
  switch (obj) {
    case Color1::RED  : name = "RED"  ; break;  // index=-12
    case Color1::GREEN: name = "GREEN"; break;  // index=7
    case Color1::BLUE : name = "BLUE" ; break;  // index=15
  }
  return name;
}
// Generated formatter for PUBLIC enum Color2 of type INT 
constexpr auto format_as(const Color2 obj) {
  fmt::string_view name = "<missing>";
  switch (obj) {
    case RED  : name = "RED"  ; break;  // index=-12
    case GREEN: name = "GREEN"; break;  // index=7
    case BLUE : name = "BLUE" ; break;  // index=15
  }
  return name;
}
// Generated formatter for PUBLIC enum Color3 of type UCHAR scoped
constexpr auto format_as(const Color3 obj) {
  fmt::string_view name = "<missing>";
  switch (obj) {
    case Color3::RED  : name = "RED"  ; break;  // index=114
    case Color3::GREEN: name = "GREEN"; break;  // index=103
    case Color3::BLUE : name = "BLUE" ; break;  // index=98
  }
  return name;
}
// Generated formatter for PUBLIC enum Directions of type INT 
constexpr auto format_as(const Directions obj) {
  fmt::string_view name = "<missing>";
  switch (obj) {
    case Up   : name = "Up"   ; break;  // index=85
    case Down : name = "Down" ; break;  // index=-42
    case Right: name = "Right"; break;  // index=120
    case Left : name = "Left" ; break;  // index=-120
  }
  return name;
}
// Generated formatter for PUBLIC enum crc_hack of type INT scoped
constexpr auto format_as(const crc_hack obj) {
  fmt::string_view name = "<missing>";
  switch (obj) {
    case crc_hack::b5a7b602ab754d7ab30fb42c4fb28d82: name = "b5a7b602ab754d7ab30fb42c4fb28d82"; break;  // index=0
  }
  return name;
}
// Generated formatter for PUBLIC enum crc_hack_2 of type INT scoped
constexpr auto format_as(const crc_hack_2 obj) {
  fmt::string_view name = "<missing>";
  switch (obj) {
    case crc_hack_2::b5a7b602ab754d7ab30fb42c4fb28d82: name = "b5a7b602ab754d7ab30fb42c4fb28d82"; break;  // index=0
    case crc_hack_2::d19f2e9e82d14b96be4fa12b8a27ee9f: name = "d19f2e9e82d14b96be4fa12b8a27ee9f"; break;  // index=1
  }
  return name;
}
// Generated formatter for PUBLIC enum MaxUsedAsInvalid of type INT scoped
constexpr auto format_as(const MaxUsedAsInvalid obj) {
  fmt::string_view name = "<missing>";
  switch (obj) {
    case MaxUsedAsInvalid::ONE    : name = "ONE"    ; break;  // index=0
    case MaxUsedAsInvalid::TWO    : name = "TWO"    ; break;  // index=63
    case MaxUsedAsInvalid::INVALID: name = "INVALID"; break;  // index=64
  }
  return name;
}
// Generated formatter for PUBLIC enum Binary of type BOOL scoped
constexpr auto format_as(const Binary obj) {
  fmt::string_view name = "<missing>";
  switch (obj) {
    case Binary::ONE: name = "ONE"; break;  // index=0
    case Binary::TWO: name = "TWO"; break;  // index=-1
  }
  return name;
}
// Generated formatter for PUBLIC enum Number1 of type INT scoped
constexpr auto format_as(const Number1 obj) {
  fmt::string_view name = "<missing>";
  switch (obj) {
    case Number1::one  : name = "one"  ; break;  // index=1
    case Number1::two  : name = "two"  ; break;  // index=2
    case Number1::three: name = "three"; break;  // index=3
    case Number1::many : name = "many" ; break;  // index=127
  }
  return name;
}
// Generated formatter for PUBLIC enum Number2 of type INT 
constexpr auto format_as(const Number2 obj) {
  fmt::string_view name = "<missing>";
  switch (obj) {
    case one  : name = "one"  ; break;  // index=100
    case two  : name = "two"  ; break;  // index=200
    case three: name = "three"; break;  // index=300
    case four : name = "four" ; break;  // index=400
    case many : name = "many" ; break;  // index=401
  }
  return name;
}
// Generated formatter for PUBLIC enum Number3 of type INT scoped
constexpr auto format_as(const Number3 obj) {
  fmt::string_view name = "<missing>";
  switch (obj) {
    case Number3::one  : name = "one"  ; break;  // index=2
    case Number3::two  : name = "two"  ; break;  // index=4
    case Number3::three: name = "three"; break;  // index=8
    case Number3::many : name = "many" ; break;  // index=1073741824
  }
  return name;
}
// Generated formatter for PUBLIC enum Dir of type INT 
constexpr auto format_as(const Dir obj) {
  fmt::string_view name = "<missing>";
  switch (obj) {
    case L: name = "L"; break;  // index=0
    case D: name = "D"; break;  // index=1
    case U: name = "U"; break;  // index=2
    case R: name = "R"; break;  // index=3
  }
  return name;
}
// Generated formatter for PUBLIC enum space::rangers of type INT scoped
constexpr auto format_as(const space::rangers obj) {
  fmt::string_view name = "<missing>";
  switch (obj) {
    case space::rangers::humans  : name = "humans"  ; break;  // index=0
    case space::rangers::maloqs  : name = "maloqs"  ; break;  // index=1
    case space::rangers::pelengs : name = "pelengs" ; break;  // index=2
    case space::rangers::faeyans : name = "faeyans" ; break;  // index=3
    case space::rangers::gaalians: name = "gaalians"; break;  // index=4
  }
  return name;
}

namespace space {using ::format_as;}

