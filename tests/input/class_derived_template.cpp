/**
 * @file class_derived_template.cpp
 * misc demo of cpp-fstring
 *
 * @ingroup examples
 *
 * @author Sandeep M
 * @copyright Copyright 2023 Sandeep M<deep@tensorfield.ag>
 *   @license MIT License
 */
// from
// https://stackoverflow.com/questions/41333185/python-clang-getting-template-arguments
//
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <vector>

#include "fstr.h"

// set1
class A {
  int a = 32;
};

class B : public A {
  int b = 13;
};

template <typename T>
class X {
 public:
  T x;
};

class Y : public X<bool> {
  int y = 13;
};

// set2
template <typename T>
struct Obj {
  T value;
};

template <typename K, typename T>
struct Map {
  std::map<K, T> map1;
  std::map<K, Obj<T>> map2;
  // TODO(deep): find workaround
  // map3 is not found by libclang
  std::map<K, std::vector<Obj<T>>> map3;
};

// from
// https://stackoverflow.com/questions/66949980/variadic-template-data-structures
//
template <class T>
struct Helper {
  int value = 1;
};

template <>
struct Helper<int> {
  int value = 2;
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
};

int main()
{
  using std::cout;
  cout << "file: {__FILE_NAME__}\ntime: {__TIMESTAMP__}\n";

  // should print both a and b
  cout << " {B()=} \n";

  cout << " {X<int>()=} ";
  cout << " {X<bool>()=} ";
  cout << " {X<std::string>()=} ";
  cout << " {Y()=} ";
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
  cout << "Map {m=}";

  cout << "{Helper<int>()=}";
  cout << "{Helper<char>()=}";

  Container<int, std::vector> dp1;
  dp1.addData(10);
  dp1.addData(20);
  dp1.addData(30);
  cout << "Container<int, std::vector> {dp1=}";

  Container<std::string, std::list> dp2;
  dp2.addData("Hello");
  dp2.addData("World");
  cout << "Container<std::string, std::list> {dp2=}";
  return 0;
}
