/**
 * @file class_derived_template.cpp
 * misc demo of cpp-fstring
 *
 * @ingroup examples
 *
 * @author Sandeep M
 * @copyright Copyright 2023 Sandeep M<deep@tensorfield.ag>
   @license MIT License
*/
// from
// https://stackoverflow.com/questions/41333185/python-clang-getting-template-arguments
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <vector>

#include "fstr.h"

// set1
class A {
  int a = 32;

  friend class B;
  // Generated to_string() for PUBLIC CLASS_DECL A
  public:
  auto to_string() const {
    return fstr::format(": int a={}\n", a);
  }
};

class B : public A {
  int b = 13;
  // Generated to_string() for PUBLIC CLASS_DECL B
  public:
  auto to_string() const {
    return fstr::format(": int b={}, a={}\n", b, this->a);
  }
};

template <typename T>
class X {
 public:
  T x;
  // Generated to_string() for PUBLIC CLASS_TEMPLATE X<T>
  public:
  auto to_string() const {
    return fstr::format("X<T:={}>: T x={}\n", fstr::get_type_name<T>(), x);
  }
};

class Y : public X<bool> {
  int y = 13;
  // Generated to_string() for PUBLIC CLASS_DECL Y
  public:
  auto to_string() const {
    return fstr::format(": int y={}\n", y);
  }
};

// set2
template <typename T>
struct Obj {
  T value;
  // Generated to_string() for PUBLIC CLASS_TEMPLATE Obj<T>
  public:
  auto to_string() const {
    return fstr::format("Obj<T:={}>: T value={}\n", fstr::get_type_name<T>(), value);
  }
};

template <typename K, typename T>
struct Map {
  std::map<K, T> map1;
  std::map<K, Obj<T>> map2;
  // map3 is not found by libclang
  std::map<K, std::vector<Obj<T>>> map3;
  // Generated to_string() for PUBLIC CLASS_TEMPLATE Map<K, T>
  public:
  auto to_string() const {
    return fstr::format("Map<K:={}, T:={}>: int map1={}, map2={}\n", fstr::get_type_name<K>(), fstr::get_type_name<T>(), map1, map2);
  }
};

// from
// https://stackoverflow.com/questions/66949980/variadic-template-data-structures
template <class T>
struct Helper {
  int value = 1;
  // Generated to_string() for PUBLIC CLASS_TEMPLATE Helper<T>
  public:
  auto to_string() const {
    return fstr::format("Helper<T:={}>: int value={}\n", fstr::get_type_name<T>(), value);
  }
};

template <>
struct Helper<int> {
  int value = 2;
  // Generated to_string() for PUBLIC STRUCT_DECL Helper<int>
  public:
  auto to_string() const {
    return fstr::format(": int value={}\n", value);
  }
};

//
// simple template typename example
//

template <typename T, template <typename...> class C>
class Container {
 public:
  void addData(const T& data) { container.push_back(data); }

 private:
  C<T> container;
  // Generated to_string() for PUBLIC CLASS_TEMPLATE Container<T, C>
  public:
  auto to_string() const {
    return fstr::format("Container<T:={}>: C<T> container={}\n", fstr::get_type_name<T>(), container);
  }
};

int main()
{
  using std::cout;
  cout << fmt::format("file: {}\ntime: {}\n", __FILE_NAME__, __TIMESTAMP__);

  // should print both a and b
  cout << fmt::format(" B()={} \n", B());

  cout << fmt::format(" X<int>()={} ", X<int>());
  cout << fmt::format(" X<bool>()={} ", X<bool>());
  cout << fmt::format(" X<std::string>()={} ", X<std::string>());
  cout << fmt::format(" Y()={} ", Y());
  auto y = Y();
  // TODO(deep): fix derived template class missing vars
  cout << " Y() should print both y and x=" << y.x << " \n";

  Map<std::string, int> m;
  m.map1["key1"] = 100;
  m.map1["key2"] = 200;

  m.map2["key3"] = {300};
  m.map2["key4"] = {500};

  m.map3["key5"] = {{600}};
  m.map3["key6"] = {{700}, {800}};

  // TODO(deep): fix derived template class missing map3
  // TODO(deep): map2 print is very ugly
  cout << fmt::format("Map m={}", m);

  cout << fmt::format("Helper<int>()={}", Helper<int>());
  cout << fmt::format("Helper<char>()={}", Helper<char>());

  Container<int, std::vector> dp1;
  dp1.addData(10);
  dp1.addData(20);
  dp1.addData(30);
  cout << fmt::format("Container<int, std::vector> dp1={}", dp1);

  Container<std::string, std::list> dp2;
  dp2.addData("Hello");
  dp2.addData("World");
  cout << fmt::format("Container<std::string, std::list> dp2={}", dp2);
  return 0;
}



