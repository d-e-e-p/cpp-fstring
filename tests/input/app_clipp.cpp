/**
 * @file complex_clipp.cpp
 * misc demo of cpp-fstring
 *
 * @ingroup examples
 *
 * @author Sandeep M
 * @copyright Copyright 2023 Sandeep M<deep@tensorfield.ag>
   @license MIT License
*/
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "fstr.h"
#include "clipp.h"

int main()
{
  using namespace clipp;
  using std::cout;
  cout << "file: {__FILE_NAME__}\ntime: {__TIMESTAMP__}\n";

  bool a = false, b = false, c = true; //target variables

  auto cli = (
        option("-a").set(a)                  % "activates a",
        option("-b").set(b)                  % "activates b",
        option("-c", "--noc").set(c,false)   % "deactivates c",
        option("--hi")([]{cout << "hi!\n";}) % "says hi");

  auto usage = usage_lines(cli, "prefix>");

  cout << " {cli=} \n";
  cout << " {usage=} \n";
}
