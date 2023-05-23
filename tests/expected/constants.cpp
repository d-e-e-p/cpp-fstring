int main() {
	str = fmt::format("{}", 42);
	str = fmt::format("{:x}", 0x42);
	str = fmt::format("{}", 42ll);
	str = fmt::format("{}", 42ul);
	str = fmt::format("{}", 42ull);
  str = fmt::format("{}", 0x42);
}
