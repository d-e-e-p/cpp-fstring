
#include <string>
#include <iostream>


int main() {
  std::string foo = fmt::format(R"(kjh{}j)", name);
  std::string bar = fmt::format("kjh{:3d}j", x);
  auto S6 = fmt::format(u8R"("Hello {} \ world")"s, name);
  std::cout << "test\n";
  std::string s = "Hello";
  std::cout << fmt::format(R"abc({}"\()abc", s) << "\n"; // delimiter sequence is abc and the string content is Hello"\(.
  std::cout << fmt::format(R"xyz({})")xyz", s) << "\n"; // delim seq is xyz and string is )"
  std::cout << fmt::format(R"(
   multi
   line
   test
    {}
  )", s);


}
