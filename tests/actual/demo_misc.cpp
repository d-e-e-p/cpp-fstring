/**
 * @file demo_misc.cpp
 * misc demo of cpp-fstring for STL variables
 *
 * Examples of:
 *   1. Format Specifiers
 *   2. Dates
 *   3. Expressions
 *   4. Ranges
 *
 * @ingroup examples
 *
 * @author Sandeep M
 * @copyright Copyright 2023 Sandeep M<deep@tensorfield.ag> All rights reserved.
 *   @license MIT License
 */

#include <chrono>
#include <cmath>
#include <ctime>     // for tm
#include <cwchar>    // for tm
#include <iostream>  // for operator<<, cout, basic_ostream
#include <list>
#include <map>  // for operator!=, map
#include <string>  // for basic_string, char_traits, operator""s, operator<, string_literals
#include <tuple>     // for tuple
#include <valarray>  // for __val_expr, pow, begin, end, operator+, valarray
#include <variant>   // for variant
#include <vector>    // for vector

#include "fmt/chrono.h"
#include "fstr.h"

using namespace std::string_literals;

enum vtypes {INT, FLOAT, STRING, CHAR};

int main()
{
  using std::cout;
  cout << fmt::format("file: {}\ntime: {}\n", __FILE_NAME__, __TIMESTAMP__);

  std::string str;
  int num = 97;
  cout << fmt::format(R"(
Overview of f-strings in C++
============================

1. Format Specifiers
--------------------


+----------------------+
| int     | {: >8d}   |
+=========+============+
| {{{}:#b}} | {:#08b}  |
+---------+------------+
| {{{}:c}}  | {: >8c}   |
+---------+------------+
| {{{}:d}}  | {: 8d}   |
+---------+------------+
| {{{}:o}}  | {: 8o}   |
+---------+------------+
| {{{}:x}}  | {: 8x}   |
+---------+------------+

+----------------------------------+
|  float    | {}    |
+==================================+
| {{M_PI:a}}  | {: <20a} |
+----------------------------------+
| {{M_PI:e}}  | {: <20e} |
+----------------------------------+
| {{M_PI:g}}  | {: <20g} |
+----------------------------------+
| {{M_PI:f}}  | {: <20f} |
+----------------------------------+

)", num, num, num, num, num, num, num, num, num, num, num, M_PI, M_PI, M_PI, M_PI, M_PI);

  struct tm time = {.tm_year = 2023 - 1900, .tm_mday = 1};
  // int large = 10'000'000;

  cout << fmt::format(R"(
2. Dates
---------

It was a sunny {{time:%A}} in {{time:%B}} around {{time:%OI}}{{time:%p}}
          becomes:
It was a sunny {:%A} in {:%B} around {:%OI}{:%p}

)", time, time, time, time);

  cout << R"(
3. Expressions
--------------
)";

  auto va = std::vector<float>{5, M_PI, 10'000'000};
  int xb = 10;

  for (auto& ia : va) {
    cout << fmt::format(R"(

  when a={}, b={} =>

      {} + {} = {}
      {} * {} = {}
      {} ^ {} = {}

     )", ia, xb, ia, xb, ia+xb, ia, xb, ia*xb, ia, xb, std::pow(ia, xb));  // cout
  }

  int arr[] = {3, 2, 1};
  auto v1 = std::vector<int>{
      1,
      2,
      3,
  };
  auto v2 = std::vector<std::vector<int>>{{1, 2}, {3, 5}, {7, 11}};
  std::vector<std::vector<int>> v3 = {{1, 2, 3}, {4, 5}, {6}};

  auto m1 = std::map<std::string, int>{{"one", 1}, {"two", 2}};
  std::map<int, std::vector<int>> m2 = {{1, {1, 2}}, {2, {3, 4}}};

  auto p1 = std::pair<int, float>(42, 1.5F);
  std::tuple<double, char, std::string> t1 = {3.8, 'A', "Lisa Simpson"};

  cout << fmt::format(R"(

4. Ranges
---------

  Vectors:
    vector<int> v1={}
    vector<vector<int>> v2={}
    vector<vector<int>> v3={}

  Pairs:
    pair<int, float> p1={}

  Maps:
    map<string, int> m1={}
    map<int, vector<int>> m2={}

  Tuples:
    tuple<double, char, string> t1={}

   )", v1, v2, v3, p1, m1, m2, t1);

  using Var = std::variant<int, float, std::string, char>;
  std::map<vtypes,Var> vmap = {{vtypes::INT, 10}, {vtypes::FLOAT, 3.14f}, {vtypes::STRING, std::string("Hello")}, {vtypes::CHAR, 'a'}};

  cout << fmt::format(R"(
  Variants:
    vmap={}
   )", vmap);

  using IArr = std::valarray<int>;
  IArr a{1, 2, 3};
  IArr b{4, 5, 6};
  IArr ab = std::pow(a, b);
  IArr ba = std::pow(b, a);
  IArr abba = ab + ba;

  cout << fmt::format(R"(
    Valarray:
      a^b + b^a = {}^{} + {}^{}
                = {} + {}
                = {}

      min({}) = {}
      sum({}) = {}
      max({}) = {}
   )", a, b, b, a, ab, ba, abba, abba, abba.min(), abba, abba.sum(), abba, abba.max());
}

// Generated formatter for PUBLIC enum vtypes of type UINT 
constexpr auto format_as(const vtypes obj) {
  fmt::string_view name = "<missing>";
  switch (obj) {
    case INT   : name = "INT"   ; break;  // index=0
    case FLOAT : name = "FLOAT" ; break;  // index=1
    case STRING: name = "STRING"; break;  // index=2
    case CHAR  : name = "CHAR"  ; break;  // index=3
  }
  return name;
}


