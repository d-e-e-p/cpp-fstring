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
};

class Xclass {
  public:
    enum dir { left, right };
    enum class cdir { left, right };
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
  cout << "file: {__FILE_NAME__}\ntime: {__TIMESTAMP__}\n";

  cout << R"(

Test enum namespaces
--------------------

     {right=}                       {left=}
     {cdir::right=}                 {cdir::left=}
     {eright=}                     {eleft=}
     {ecdir::right=}                {ecdir::left=}
     {Xstruct::right=}              {Xstruct::left=}
     {Xstruct::cdir::right=}        {Xstruct::cdir::left=}
     {Xclass::right=}               {Xclass::left=}
     {Xclass::cdir::right=}         {Xclass::cdir::left=}
     {Xnamespace::right=}           {Xnamespace::left=}
     {Xnamespace::cdir::right=}     {Xnamespace::cdir::left=}

     std::map<sym, int> {roman::numerals=}

  )";

}
