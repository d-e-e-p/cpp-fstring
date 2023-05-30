

int main() {
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
  str = "{{42}}";
  str = fmt::format("{{{}}}", 42);
  str = fmt::format("{{{}}}", foo);
  str = fmt::format("{{{}}}", 0x42);
}



