
#include "fixed_size_string_buffer.h"
#include "fstr.h"

#include <iostream>
#include <sstream>

int main() {
  using std::cout;
  cout << "file: {__FILE_NAME__}\ntime: {__TIMESTAMP__}\n";

  cout << "fixed_size_string_buffer demo \n\n";
  constexpr size_t max_size = 10;
  auto rb = fssb::FixedSizeStringBuffer<max_size>();
  cout << " {rb=} ";
  // add strings
  const std::string str = "The Quick Brown Fox Jumped Over The Lazy Dog";
  std::istringstream ss(str);
  std::string word;
  while (ss >> word) {
    rb.push(word);
  }
  cout << " {rb=} ";
}
