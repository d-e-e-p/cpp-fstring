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

int main()
{
  using std::cout;
  cout << fmt::format("file: {}\ntime: {}\n", __FILE_NAME__, __TIMESTAMP__);
  cout << fmt::format("1. {}", SampleClass());
  cout << fmt::format("2. {}", Bug_3488053());
  cout << fmt::format("3. {}", Bananna());
  cout << fmt::format("4. {}", Bird());
  cout << fmt::format("5. {}", EagleClass());
  cout << fmt::format("6. {}", FrogClass());
}



