/**
 * @file enum_namespace.cpp
 * misc demo of cpp-fstring
 *
 * @ingroup examples
 *
 * @author Sandeep M
 * @copyright Copyright 2023 Sandeep M<deep@tensorfield.ag>
   @license MIT License
*/

#include <map>
#include <iostream>
#include "fstr.h"

using namespace std::string_literals;

enum class cdir { left, right };
enum dir { left, right };

namespace {

enum class ecdir { left, right };
enum edir { eleft, eright };

}

struct Xstruct {
  public:
    enum dir { left, right };
    enum class cdir { left, right };
// Generated formatter for PUBLIC enum Xstruct::cdir of type INT scoped
 friend constexpr auto format_as(const Xstruct::cdir obj) {
  fmt::string_view name = "<missing>";
  switch (obj) {
    case Xstruct::cdir::left : name = "left" ; break;  // index=0
    case Xstruct::cdir::right: name = "right"; break;  // index=1
  }
  return name;
}
// Generated formatter for PUBLIC enum Xstruct::dir of type UINT 
 friend constexpr auto format_as(const Xstruct::dir obj) {
  fmt::string_view name = "<missing>";
  switch (obj) {
    case Xstruct::left : name = "left" ; break;  // index=0
    case Xstruct::right: name = "right"; break;  // index=1
  }
  return name;
}
};

class Xclass {
  public:
    enum dir { left, right };
    enum class cdir { left, right };
// Generated formatter for PUBLIC enum Xclass::cdir of type INT scoped
 friend constexpr auto format_as(const Xclass::cdir obj) {
  fmt::string_view name = "<missing>";
  switch (obj) {
    case Xclass::cdir::left : name = "left" ; break;  // index=0
    case Xclass::cdir::right: name = "right"; break;  // index=1
  }
  return name;
}
// Generated formatter for PUBLIC enum Xclass::dir of type UINT 
 friend constexpr auto format_as(const Xclass::dir obj) {
  fmt::string_view name = "<missing>";
  switch (obj) {
    case Xclass::left : name = "left" ; break;  // index=0
    case Xclass::right: name = "right"; break;  // index=1
  }
  return name;
}
};


namespace Xnamespace {
  enum dir { left, right };
  enum class cdir { left, right };
}

//
// see https://wgml.pl/blog/formatting-user-defined-types-fmt.html
//
namespace roman {

  enum class sym {M, D, C, L, X, V, I};

  std::map<sym, int> numerals = {
    {sym::M, 1000},
    {sym::D,  500},
    {sym::C,  100},
    {sym::L,   50},
    {sym::X,   10},
    {sym::V,    5},
    {sym::I,    1}
  };

}  // namespace roman



int main() {
  using std::cout;
  cout << fmt::format("file: {}\ntime: {}\n", __FILE_NAME__, __TIMESTAMP__);

  cout << fmt::format(R"(

Test enum namespaces
--------------------

     right={}                       left={}
     cdir::right={}                 cdir::left={}
     eright={}                     eleft={}
     ecdir::right={}                ecdir::left={}
     Xstruct::right={}              Xstruct::left={}
     Xstruct::cdir::right={}        Xstruct::cdir::left={}
     Xclass::right={}               Xclass::left={}
     Xclass::cdir::right={}         Xclass::cdir::left={}
     Xnamespace::right={}           Xnamespace::left={}
     Xnamespace::cdir::right={}     Xnamespace::cdir::left={}

     std::map<sym, int> roman::numerals={}

  )", right, left, cdir::right, cdir::left, eright, eleft, ecdir::right, ecdir::left, Xstruct::right, Xstruct::left, Xstruct::cdir::right, Xstruct::cdir::left, Xclass::right, Xclass::left, Xclass::cdir::right, Xclass::cdir::left, Xnamespace::right, Xnamespace::left, Xnamespace::cdir::right, Xnamespace::cdir::left, roman::numerals);

}

// Generated formatter for PUBLIC enum cdir of type INT scoped
constexpr auto format_as(const cdir obj) {
  fmt::string_view name = "<missing>";
  switch (obj) {
    case cdir::left : name = "left" ; break;  // index=0
    case cdir::right: name = "right"; break;  // index=1
  }
  return name;
}
// Generated formatter for PUBLIC enum dir of type UINT 
constexpr auto format_as(const dir obj) {
  fmt::string_view name = "<missing>";
  switch (obj) {
    case left : name = "left" ; break;  // index=0
    case right: name = "right"; break;  // index=1
  }
  return name;
}
// Generated formatter for PUBLIC enum ecdir of type INT scoped
constexpr auto format_as(const ecdir obj) {
  fmt::string_view name = "<missing>";
  switch (obj) {
    case ecdir::left : name = "left" ; break;  // index=0
    case ecdir::right: name = "right"; break;  // index=1
  }
  return name;
}
// Generated formatter for PUBLIC enum edir of type UINT 
constexpr auto format_as(const edir obj) {
  fmt::string_view name = "<missing>";
  switch (obj) {
    case eleft : name = "eleft" ; break;  // index=0
    case eright: name = "eright"; break;  // index=1
  }
  return name;
}
// Generated formatter for PUBLIC enum Xnamespace::dir of type UINT 
constexpr auto format_as(const Xnamespace::dir obj) {
  fmt::string_view name = "<missing>";
  switch (obj) {
    case Xnamespace::left : name = "left" ; break;  // index=0
    case Xnamespace::right: name = "right"; break;  // index=1
  }
  return name;
}
// Generated formatter for PUBLIC enum Xnamespace::cdir of type INT scoped
constexpr auto format_as(const Xnamespace::cdir obj) {
  fmt::string_view name = "<missing>";
  switch (obj) {
    case Xnamespace::cdir::left : name = "left" ; break;  // index=0
    case Xnamespace::cdir::right: name = "right"; break;  // index=1
  }
  return name;
}
// Generated formatter for PUBLIC enum roman::sym of type INT scoped
constexpr auto format_as(const roman::sym obj) {
  fmt::string_view name = "<missing>";
  switch (obj) {
    case roman::sym::M: name = "M"; break;  // index=0
    case roman::sym::D: name = "D"; break;  // index=1
    case roman::sym::C: name = "C"; break;  // index=2
    case roman::sym::L: name = "L"; break;  // index=3
    case roman::sym::X: name = "X"; break;  // index=4
    case roman::sym::V: name = "V"; break;  // index=5
    case roman::sym::I: name = "I"; break;  // index=6
  }
  return name;
}

namespace  {using ::format_as;}
namespace Xnamespace {using ::format_as;}
namespace roman {using ::format_as;}

