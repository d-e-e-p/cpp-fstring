
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
  std::cout << " c = {Color::GREEN} \n";

}
