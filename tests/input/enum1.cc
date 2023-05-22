
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
  std::cout << " d = {Xstruct::direction::left} \n";
}
