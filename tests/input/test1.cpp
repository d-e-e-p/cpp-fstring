

int main() {
  std::string foo = "k\njh{name}j";
  std::string bar = "k\r\njh{x:3d}j";
  std::cout << "test\n";
  std::cout << ansi::redhb << "foo" << foo << "test\n" << ansi::reset;

}
