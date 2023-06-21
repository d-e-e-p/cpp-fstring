/**
 * @file class_include.cpp
 * misc demo of cpp-fstring
 *
 * @ingroup examples
 *
 * @author Sandeep M
 * @copyright Copyright 2023 Sandeep M<deep@tensorfield.ag>
   @license MIT License
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
  cout << "file: {__FILE_NAME__}\ntime: {__TIMESTAMP__}\n";
  cout << "1. {SampleClass()}";
  cout << "2. {Bug_3488053()}";
  cout << "3. {Bananna()}";
  cout << "4. {Bird()}";
  cout << "5. {EagleClass()}";
  cout << "6. {FrogClass()}";
}
