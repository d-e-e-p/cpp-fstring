/**
 * @file class_scope.cpp
 * misc demo of cpp-fstring
 *
 * @ingroup examples
 *
 * @author Sandeep M
 * @copyright Copyright 2023 Sandeep M<deep@tensorfield.ag>
 *   @license MIT License
 */
#include <iostream>

#include "fstr.h"

// from https://github.com/muellan/clipp/blob/master/include/clipp.h

/// assigns boolean constant to one or multiple target objects
template <class T, class V = T>
class assign_value {
 public:
  template <class X>
  explicit constexpr assign_value(T& target, X&& value) noexcept
      : t_{std::addressof(target)}, v_{std::forward<X>(value)}
  {
  }

  void operator()() const
  {
    if (t_) { *t_ = v_; }
  }

 private:
  T* t_;
  V v_;
  // Generated to_string() for PUBLIC CLASS_TEMPLATE assign_value<T, V>
  public:
  auto to_string() const {
    const std::string fmt_string = "assign_value<T:={}, V:={}>: T * t_={}, V v_={}";
    return fstr::format(fmt_string, fstr::get_type_name<T>(), fstr::get_type_name<V>(), fmt::ptr(t_), v_);
  }
};

// from https://github.com/jarro2783/cxxopts/blob/master/include/cxxopts.hpp
class KeyValue {
 public:
  KeyValue(std::string key_, std::string value_) noexcept
      : m_key(std::move(key_)), m_value(std::move(value_))
  {
  }

 private:
  std::string m_key;
  std::string m_value;
  // Generated to_string() for PUBLIC CLASS_DECL KeyValue
  public:
  auto to_string() const {
    const std::string fmt_string = "KeyValue: int m_key={}, m_value={}";
    return fstr::format(fmt_string, m_key, m_value);
  }
};

class doc {
 public:
  int foo = 0;

 private:
  enum class paragraph { words, punctuation };
  paragraph bar = paragraph::words;
  // TODO(deep): find workaround for ::iterator and ::const_iterator issue with
  // libclang see https://github.com/llvm/llvm-project/issues/63277
  // std::vector<KeyValue>::const_iterator m_iter;
  // Generated to_string() for PUBLIC CLASS_DECL doc
  public:
  auto to_string() const {
    const std::string fmt_string = "doc: int foo={}, paragraph bar={}";
    return fstr::format(fmt_string, foo, bar);
  }
// Generated formatter for PRIVATE enum doc::paragraph of type INT scoped
 friend constexpr auto format_as(const doc::paragraph obj) {
  fmt::string_view name = "<missing>";
  switch (obj) {
    case doc::paragraph::words      : name = "words"      ; break;  // index=0
    case doc::paragraph::punctuation: name = "punctuation"; break;  // index=1
  }
  return name;
}
};

// from https://github.com/llvm/llvm-project/issues/48732
struct S {
  template <int>
  using N = int;
  static constexpr int size = 5;
  // Generated to_string() for PUBLIC STRUCT_DECL S
  public:
  auto to_string() const {
    const std::string fmt_string = "S: const int size={}";
    return fstr::format(fmt_string, size);
  }
};

int main()
{
  using std::cout;
  cout << fmt::format("file: {}\ntime: {}\n", __FILE_NAME__, __TIMESTAMP__);

  bool target = false;
  auto as = assign_value<bool>{target, true};
  cout << fmt::format(" as={} \n", as);
  cout << fmt::format(" doc()={} \n", doc());
  cout << fmt::format(" S()={} \n", S());
  return 0;
}



