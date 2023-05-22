
#include <string>
#include <iostream>


int main() {
  std::string foo = R"(kjh{name}j)";
  std::string bar = "kjh{x:3d}j";
  auto S6 = u8R"("Hello {name} \ world")"s;
  std::cout << "test\n";
  std::string s = "Hello";
  std::cout << R"abc({s}"\()abc" << "\n"; // delimiter sequence is abc and the string content is Hello"\(.
  std::cout << R"xyz({s})")xyz" << "\n"; // delim seq is xyz and string is )"
  std::cout << R"(
   multi
   line
   test
    {s}
  )";


}
