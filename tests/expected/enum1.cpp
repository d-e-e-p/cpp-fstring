
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

enum class cdir { left = 'l', right = 'r' };
enum dir { left = 'l', right = 'r' };


// test cases from:
struct Xstruct {
  enum dir { left = 'l', right = 'r' };
  enum class cdir { left = 'l', right = 'r' };
};

class Xclass {
  enum dir { left = 'l', right = 'r' };
  enum class cdir { left = 'l', right = 'r' };
};

namespace Xnamespace {
  enum dir { left = 'l', right = 'r' };
  enum class cdir { left = 'l', right = 'r' };
}


int main() {
  std::cout << fmt::format(" d = {} \n", Xstruct::direction::left);
}


// Generated formatter for INVALID enum cdir of type INT scoped True
template <>
struct fmt::formatter<cdir>: formatter<string_view> {
  template <typename FormatContext>
  auto format(cdir val, FormatContext& ctx) const {
    string_view name = "<unknown>";
    switch (val) {
        case cdir::left : name = "left" ; break;  // index=108
        case cdir::right: name = "right"; break;  // index=114
    }
    return formatter<string_view>::format(name, ctx);
  }
};
// Generated formatter for INVALID enum dir of type UINT scoped False
template <>
struct fmt::formatter<dir>: formatter<string_view> {
  template <typename FormatContext>
  auto format(dir val, FormatContext& ctx) const {
    string_view name = "<unknown>";
    switch (val) {
        case left : name = "left" ; break;  // index=108
        case right: name = "right"; break;  // index=114
    }
    return formatter<string_view>::format(name, ctx);
  }
};
// Generated formatter for PUBLIC enum Xstruct::dir of type UINT scoped False
template <>
struct fmt::formatter<Xstruct::dir>: formatter<string_view> {
  template <typename FormatContext>
  auto format(Xstruct::dir val, FormatContext& ctx) const {
    string_view name = "<unknown>";
    switch (val) {
        case Xstruct::left : name = "left" ; break;  // index=108
        case Xstruct::right: name = "right"; break;  // index=114
    }
    return formatter<string_view>::format(name, ctx);
  }
};
// Generated formatter for PUBLIC enum Xstruct::cdir of type INT scoped True
template <>
struct fmt::formatter<Xstruct::cdir>: formatter<string_view> {
  template <typename FormatContext>
  auto format(Xstruct::cdir val, FormatContext& ctx) const {
    string_view name = "<unknown>";
    switch (val) {
        case Xstruct::cdir::left : name = "left" ; break;  // index=108
        case Xstruct::cdir::right: name = "right"; break;  // index=114
    }
    return formatter<string_view>::format(name, ctx);
  }
};
// Generated formatter for PRIVATE enum Xclass::dir of type UINT scoped False
template <>
struct fmt::formatter<Xclass::dir>: formatter<string_view> {
  template <typename FormatContext>
  auto format(Xclass::dir val, FormatContext& ctx) const {
    string_view name = "<unknown>";
    switch (val) {
        case Xclass::left : name = "left" ; break;  // index=108
        case Xclass::right: name = "right"; break;  // index=114
    }
    return formatter<string_view>::format(name, ctx);
  }
};
// Generated formatter for PRIVATE enum Xclass::cdir of type INT scoped True
template <>
struct fmt::formatter<Xclass::cdir>: formatter<string_view> {
  template <typename FormatContext>
  auto format(Xclass::cdir val, FormatContext& ctx) const {
    string_view name = "<unknown>";
    switch (val) {
        case Xclass::cdir::left : name = "left" ; break;  // index=108
        case Xclass::cdir::right: name = "right"; break;  // index=114
    }
    return formatter<string_view>::format(name, ctx);
  }
};
// Generated formatter for INVALID enum Xnamespace::dir of type UINT scoped False
template <>
struct fmt::formatter<Xnamespace::dir>: formatter<string_view> {
  template <typename FormatContext>
  auto format(Xnamespace::dir val, FormatContext& ctx) const {
    string_view name = "<unknown>";
    switch (val) {
        case Xnamespace::left : name = "left" ; break;  // index=108
        case Xnamespace::right: name = "right"; break;  // index=114
    }
    return formatter<string_view>::format(name, ctx);
  }
};
// Generated formatter for INVALID enum Xnamespace::cdir of type INT scoped True
template <>
struct fmt::formatter<Xnamespace::cdir>: formatter<string_view> {
  template <typename FormatContext>
  auto format(Xnamespace::cdir val, FormatContext& ctx) const {
    string_view name = "<unknown>";
    switch (val) {
        case Xnamespace::cdir::left : name = "left" ; break;  // index=108
        case Xnamespace::cdir::right: name = "right"; break;  // index=114
    }
    return formatter<string_view>::format(name, ctx);
  }
};
