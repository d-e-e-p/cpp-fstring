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
  cout << "1. {SampleClass()}\n";
  cout << "2. {Bug_3488053()}\n";
  cout << "3. {Bananna()}\n";
  cout << "4. {Bird()}\n";
  cout << "5. {EagleClass()}\n";
  cout << "6. {FrogClass()}\n";
}
