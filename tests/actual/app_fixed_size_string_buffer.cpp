
#include "fixed_size_string_buffer.h"
#include "fstr.h"

#include <iostream>
#include <sstream>

int main() {
  using std::cout;
  cout << fmt::format("file: {}\ntime: {}\n", __FILE_NAME__, __TIMESTAMP__);

  cout << "fixed_size_string_buffer demo \n\n";
  constexpr size_t max_size = 10;
  auto rb = fssb::FixedSizeStringBuffer<max_size>();
  cout << fmt::format(" rb={} ", rb);
  // add strings
  const std::string str = "The Quick Brown Fox Jumped Over The Lazy Dog";
  std::istringstream ss(str);
  std::string word;
  while (ss >> word) {
    rb.push(word);
  }
  cout << fmt::format(" rb={} ", rb);
}



