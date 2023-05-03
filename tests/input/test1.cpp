

int main() {
  std::string foo = "k\njh{name}j";
  std::string bar = "k\r\njh{x:3d}j";
  std::cout << "test\n";
  std::cout << ansi::redhb << "foo" << foo << "test\n" << ansi::reset;

  // escapes..taken from:
  // https://github.com/fmtlib/fmt/blob/master/test/format-test.cc
  std::string str;
  str = "{{";
  str = "before {{";
  str = "{{ after";
  str = "before {{ after";
  str = "}}";
  str = "before }}";
  str = "}} after";
  str = "before }} after";
  str = "{{}}";
  str = "{{{42}}}";

}
