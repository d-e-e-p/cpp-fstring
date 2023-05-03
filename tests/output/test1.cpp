

int main() {
  std::string foo = std::format("k\njh{}j", name);;
  std::string bar = std::format("k\r\njh{:3d}j", x);;
  std::cout << "test\n";
  std::cout << ansi::redhb << "foo" << foo << "test\n" << ansi::reset;

}

