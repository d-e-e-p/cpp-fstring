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
#include "utils.h"

// set1
class A {
  int a = 1;

  friend class B;

  friend class C;

  friend class D;
  // Generated to_string() for PUBLIC CLASS_DECL A
  public:
  auto to_string() const {
    const std::string fmt_string = "A: int a={}";
    return fstr::format(fmt_string, a);
  }
};

class B : public A {
  int b = 2;

  friend class C;

  friend class D;
  // Generated to_string() for PUBLIC CLASS_DECL B
  public:
  auto to_string() const {
    const std::string fmt_string = "B: int b={}, a={}";
    return fstr::format(fmt_string, b, this->a);
  }
};

class C : public B {
  int c = 3;

  friend class D;
  // Generated to_string() for PUBLIC CLASS_DECL C
  public:
  auto to_string() const {
    const std::string fmt_string = "C: int c={}, b={}, a={}";
    return fstr::format(fmt_string, c, this->b, this->a);
  }
};

class D : public C {
  int d = 4;
  // Generated to_string() for PUBLIC CLASS_DECL D
  public:
  auto to_string() const {
    const std::string fmt_string = "D: int d={}, c={}, b={}, a={}";
    return fstr::format(fmt_string, d, this->c, this->b, this->a);
  }
};


template <typename T>
class X {
 public:
  T x;
  // Generated to_string() for PUBLIC CLASS_TEMPLATE X<T>
  public:
  auto to_string() const {
    const std::string fmt_string = "X<T:={}>: T x={}";
    return fstr::format(fmt_string, fstr::get_type_name<T>(), x);
  }
};

class Y : public X<bool> {
  int y = 13;
  // Generated to_string() for PUBLIC CLASS_DECL Y
  public:
  auto to_string() const {
    const std::string fmt_string = "Y: int y={}";
    return fstr::format(fmt_string, y);
  }
};

// set2
template <typename T>
struct Obj {
  T value;
  // Generated to_string() for PUBLIC CLASS_TEMPLATE Obj<T>
  public:
  auto to_string() const {
    const std::string fmt_string = "Obj<T:={}>: T value={}";
    return fstr::format(fmt_string, fstr::get_type_name<T>(), value);
  }
};

template <typename K, typename T>
struct Map {
  std::map<K, T> map1;
  std::map<K, Obj<T>> map2;
  // TODO(deep): find workaround
  // map3 is not found by libclang
  std::map<K, std::vector<Obj<T>>> map3;
  // Generated to_string() for PUBLIC CLASS_TEMPLATE Map<K, T>
  public:
  auto to_string() const {
    const std::string fmt_string = "Map<K:={}, T:={}>: int map1={}, map2={}";
    return fstr::format(fmt_string, fstr::get_type_name<K>(), fstr::get_type_name<T>(), map1, map2);
  }
};

// from
// https://stackoverflow.com/questions/66949980/variadic-template-data-structures
//
template <class T>
struct Helper {
  int value = 1;
  // Generated to_string() for PUBLIC CLASS_TEMPLATE Helper<T>
  public:
  auto to_string() const {
    const std::string fmt_string = "Helper<T:={}>: int value={}";
    return fstr::format(fmt_string, fstr::get_type_name<T>(), value);
  }
};

template <>
struct Helper<int> {
  int value = 2;
  // Generated to_string() for PUBLIC STRUCT_DECL Helper<int>
  public:
  auto to_string() const {
    const std::string fmt_string = "Helper<int>: int value={}";
    return fstr::format(fmt_string, value);
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
    const std::string fmt_string = "Container<T:={}>: C<T> container={}";
    return fstr::format(fmt_string, fstr::get_type_name<T>(), container);
  }
};

int main()
{
  using std::cout;
  print_info(__FILE__, __TIMESTAMP__);

  // should print a, b, c, d
  cout << fmt::format(" D()={}\n", D());

  cout << fmt::format(" X<int>()={} \n", X<int>());
  cout << fmt::format(" X<bool>()={} \n", X<bool>());
  cout << fmt::format(" X<std::string>()={} \n", X<std::string>());
  cout << fmt::format(" Y()={} \n", Y());
  auto y = Y();
  // TODO(deep): fix derived template class missing vars
  cout << "\n Y() should print both y and x=" << y.x << "\n";

  Map<std::string, int> m;
  m.map1["key1"] = 100;
  m.map1["key2"] = 200;

  m.map2["key3"] = {300};
  m.map2["key4"] = {500};

  m.map3["key5"] = {{600}};
  m.map3["key6"] = {{700}, {800}};

  // TODO(deep): fix derived template class missing map3
  // TODO(deep): map2 print is very ugly
  cout << fmt::format("Map m={}\n", m);

  cout << fmt::format("Helper<int>()={}\n", Helper<int>());
  cout << fmt::format("Helper<char>()={}\n", Helper<char>());

  Container<int, std::vector> dp1;
  dp1.addData(10);
  dp1.addData(20);
  dp1.addData(30);
  cout << fmt::format("Container<int, std::vector> dp1={}\n", dp1);

  Container<std::string, std::list> dp2;
  dp2.addData("Hello");
  dp2.addData("World");
  cout << fmt::format("Container<std::string, std::list> dp2={}\n", dp2);

  Container<std::tuple<int, char, double>, std::vector> dp3;
  dp3.addData(std::make_tuple(10, 'a', 1.0));
  dp3.addData(std::make_tuple(20, 'b', 2.0));
  cout << fmt::format("Container<std::tuple<int, char, double>, std::vector> {}\n", dp3);

  Container<D, std::vector> db4;
  db4.addData(D());
  db4.addData(D());
  cout << fmt::format("Container<D, std::vector> {}\n", db4);


  return 0;
}



