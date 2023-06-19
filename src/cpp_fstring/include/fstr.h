/**
 * @file fstr.h
 * Companion utilities for cpp-fstring, which brings python-style fstring to C++
 * 
 * This file contains 2 different routines:
 *   1. If a class has a member function called to_string(void), then a custom formatter is inferred for
 *   that class giving fmt::format the ability to display the stringified version of that class. 
 *   the cpp-fstring routine from https://github.com/d-e-e-p/cpp-fstring automatically generates these
 *   to_string() calls. eg loading fstr.h allows fmt::print to format Foo using it's to_string() function:
 *   
 *   @code{.cpp}
 *      #include "fstr.h"
 *      struct Foo {
 *        int a = 32;
 *        auto to_string() const {
 *          return fstr::format(R"( Foo: PUBLIC int a: {})", a);
 *        }
 *      };
 *      Foo f;
 *      fmt::print("f={}\n", f);
 *   @endcode
 *  
 *   produces the result:  
 *  
 *   @code{.sh}
 *    Foo: PUBLIC int a: 32
 *   @endcode
 *
 *   2. fstr::format is a wrapper around fmt::format that tries to skip unformattable entries, replacing
 *   them with "<unknown>" .  Does not currently support range specifications. eg:
 *
 *   @code{.cpp}
 *      int main() {
 *        int i = 0;
 *        std::vector<int> v{1,3,4};
 *        int a[10] = {};
 *        enum class example {yes, no};
 *        example e = example::yes;
 *      
 *        std::cout << fstr::format("i={}, v={}, a={}, e={}\n", i, v, a, e);
 *      }
 *   @endcode
 *
 *   produces the result:  
 *  
 *   @code{.sh}
 *   i=0, v=[1, 3, 4], a=[0, 0, 0, 0, 0, 0, 0, 0, 0, 0], e=<unknown>
 *   @endcode
 *
 * @defgroup fstring-include
 *
 * @author Sandeep M
 * @copyright Copyright 2023 Sandeep M<deep@tensorfield.ag> All rights reserved.
   @license MIT License
*/

#pragma once

#include <string>
#include <type_traits>
#include <fmt/ranges.h>
#include <fmt/std.h>
#include <fmt/xchar.h>

namespace fstr {

// Helper function to check if a type has a to_string member function
template <typename T>
constexpr auto has_to_string(int)
    -> decltype(std::declval<T>().to_string(), std::true_type{});

template <typename T>
constexpr auto has_to_string(...) -> std::false_type;

} // namespace fstr

// Custom formatter for classes and structs with a to_string member function
template <typename T>
struct fmt::formatter<T, char, std::enable_if_t<decltype(fstr::has_to_string<T>(0))::value>> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const T& obj, FormatContext& ctx) const {
        // Call the to_string function and format the resulting string
        return fmt::format_to(ctx.out(), "{}", obj.to_string());
    }
};

// safe format returns <unknown> string for un-formattable variables
namespace fstr {

// return param if it's formattable, else return <unknown> atring.
template <typename T>
auto safe_format(const T& value) -> std::string {
  if constexpr (fmt::is_formattable<T>::value) {
    return fmt::format("{}", value);
  } else {
    return "<unknown>";
  }
}

// Inspect all format args for fmt:: formattability
// TODO(deep): deal with unformattable entries with format spec expecting integers or floats
template <typename... Args>
std::string format(const std::string& fmt_string, const Args&... args) {
  return fmt::format(fmt_string, safe_format(args)...);
}

} // namespace fstr

