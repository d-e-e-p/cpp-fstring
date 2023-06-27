/**
 * @file class_include.cpp
 * misc demo of cpp-fstring
 *
 * @ingroup examples
 *
 * @author Sandeep M
 * @copyright Copyright 2023 Sandeep M<deep@tensorfield.ag>
 *   @license MIT License
 */
// from:
// https://github.com/robotpy/robotpy-cppheaderparser/blob/main/test/TestSampleClass.h
// Copyright (C) 2011, Jashua R. Cloutier
// License: BSD

#include "class_include.h"

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "fstr.h"
#include "utils.h"

int main()
{
  using std::cout;
  print_info(__FILE__, __TIMESTAMP__);
  cout << fmt::format("1. {}\n", SampleClass());
  cout << fmt::format("2. {}\n", Bug_3488053());
  cout << fmt::format("3. {}\n", Bananna());
  cout << fmt::format("4. {}\n", Bird());
  cout << fmt::format("5. {}\n", EagleClass());
  cout << fmt::format("6. {}\n", FrogClass());
}



