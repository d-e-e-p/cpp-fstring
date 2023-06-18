
#include <fmt/format.h>
#include "fmt/os.h"  // fmt::system_category
#include "fmt/ranges.h"
#include "fmt/std.h"

#include <string>
#include <iostream>
#include <ctime>
#include <vector>
#include <map>
#include <valarray>
#include <cmath>

#include <fmt/format.h>
#include <string>
#include <iostream>
#include <ctime>
#include <filesystem>
#include <vector>
#include <algorithm>

#include "fmt/std.h"

#include <stdexcept>
#include <string>
#include <vector>
#include <valarray>
#include <cmath>


#include "fmt/os.h"  // fmt::system_category
#include "fmt/ranges.h"


using namespace std::string_literals;

// test cases from:
// https://github.com/Neargye/magic_enum/blob/master/test/test.cpp
enum class Color1 { RED = -12, GREEN = 7, BLUE = 15 };
enum Color2 { RED = -12, GREEN = 7, BLUE = 15 };
enum class Numbers : int { one = 1, two, three, many = 127 };
enum Directions { Up = 85, Down = -42, Right = 120, Left = -120 };
enum number : unsigned long {
  one = 100,
  two = 200,
  three = 300,
  four = 400,
};
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

enum class Numbers2 : int {
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



int main() {
  std::string str;
  std::cout << fmt::format(" c = {} \n", Color::GREEN);

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
// Generated formatter for PUBLIC enum Numbers of type INT scoped
constexpr auto format_as(const Numbers obj) {
  fmt::string_view name = "<missing>";
  switch (obj) {
    case Numbers::one  : name = "one"  ; break;  // index=1
    case Numbers::two  : name = "two"  ; break;  // index=2
    case Numbers::three: name = "three"; break;  // index=3
    case Numbers::many : name = "many" ; break;  // index=127
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
// Generated formatter for PUBLIC enum number of type ULONG
constexpr auto format_as(const number obj) {
  fmt::string_view name = "<missing>";
  switch (obj) {
    case one  : name = "one"  ; break;  // index=100
    case two  : name = "two"  ; break;  // index=200
    case three: name = "three"; break;  // index=300
    case four : name = "four" ; break;  // index=400
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
// Generated formatter for PUBLIC enum Numbers2 of type INT scoped
constexpr auto format_as(const Numbers2 obj) {
  fmt::string_view name = "<missing>";
  switch (obj) {
    case Numbers2::one  : name = "one"  ; break;  // index=2
    case Numbers2::two  : name = "two"  ; break;  // index=4
    case Numbers2::three: name = "three"; break;  // index=8
    case Numbers2::many : name = "many" ; break;  // index=1073741824
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
