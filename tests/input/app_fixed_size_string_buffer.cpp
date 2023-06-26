/**
 * @file app_fixed_size_string_buffer.cpp
 * cpp-fstring on fixed_size_string_buffer
 *
 * @ingroup examples
 *
 * @author Sandeep M
 * @copyright Copyright 2023 Sandeep M<deep@tensorfield.ag>
 * @license MIT License
 */
#include <iostream>
#include <sstream>

#include "fixed_size_string_buffer.h"
#include "fstr.h"

int main()
{
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
  while (ss >> word) { rb.push(word); }
  cout << " {rb=} ";
}
