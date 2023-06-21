/**
 * @file class_namespace2.cpp
 * misc demo of cpp-fstring
 *
 * @ingroup examples
 *
 * @author Sandeep M
 * @copyright Copyright 2023 Sandeep M<deep@tensorfield.ag>
   @license MIT License
*/
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "fstr.h"

//
// test classes in different hier and namespaces...
//

namespace A {
template <typename T>
class Base {
 public:
  T x;
  // Generated to_string() for PUBLIC CLASS_TEMPLATE A::Base<T>
  public:
  auto to_string() const {
    return fstr::format("A::Base<T:={}>: T x={}\n", fstr::get_type_name<T>(), x);
  }
};

template <typename T>
class Derived : public Base<T> {
 public:
  T y;
  // Generated to_string() for PUBLIC CLASS_TEMPLATE A::Derived<T>
  public:
  auto to_string() const {
    return fstr::format("A::Derived<T:={}>: T y={}, x={}\n", fstr::get_type_name<T>(), y, this->x);
  }
};
}  // end namespace A

// from https://learn.microsoft.com/en-us/cpp/cpp/namespaces-cpp?view=msvc-170
// test inline namespace
namespace Test {
namespace old_ns {
auto Func() { return std::string("Hello from old"); }
struct S {
  int a{0};
  // Generated to_string() for PUBLIC STRUCT_DECL Test::old_ns::S
  public:
  auto to_string() const {
    return fstr::format(": int a={}\n", a);
  }
} s;
}  // namespace old_ns

inline namespace new_ns {
auto Func() { return std::string("Hello from new"); }
struct S {
  int a{1};
  // Generated to_string() for PUBLIC STRUCT_DECL Test::new_ns::S
  public:
  auto to_string() const {
    return fstr::format(": int a={}\n", a);
  }
} s;
}  // namespace new_ns
}  // namespace Test

//
// from https://devdocs.io/cpp/language/derived_class
struct Base0 {
  int a, b, base0;
  // Generated to_string() for PUBLIC STRUCT_DECL Base0
  public:
  auto to_string() const {
    return fstr::format(": int a={}, b={}, base0={}\n", a, b, base0);
  }
};

struct Base1 {
  int c, d, base1;
  // Generated to_string() for PUBLIC STRUCT_DECL Base1
  public:
  auto to_string() const {
    return fstr::format(": int c={}, d={}, base1={}\n", c, d, base1);
  }
};

// every object of type Derived includes Base as a subobject
struct Derived0 : Base0 {
  int a{10}, derived0;
  // Generated to_string() for PUBLIC STRUCT_DECL Derived0
  public:
  auto to_string() const {
    return fstr::format(": int a={}, derived0={}, b={}, base0={}\n", a, derived0, this->b, this->base0);
  }
};

struct Derived1 : Base1 {
  int c{21}, derived1;
  // Generated to_string() for PUBLIC STRUCT_DECL Derived1
  public:
  auto to_string() const {
    return fstr::format(": int c={}, derived1={}, d={}, base1={}\n", c, derived1, this->d, this->base1);
  }
};

// every object of type Derived2 includes Derived and Base as subobjects
struct Derived2 : Derived0, Derived1 {
  int b{42}, d{42}, derived2;
  // Generated to_string() for PUBLIC STRUCT_DECL Derived2
  public:
  auto to_string() const {
    return fstr::format(": int b={}, d={}, derived2={}, a={}, derived0={}, base0={}, c={}, derived1={}, base1={}\n", b, d, derived2, this->a, this->derived0, this->base0, this->c, this->derived1, this->base1);
  }
};

namespace Parent {
inline namespace new_ns {
template <typename T>
struct C {
  T member{};
  // Generated to_string() for PUBLIC CLASS_TEMPLATE Parent::new_ns::C<T>
  public:
  auto to_string() const {
    return fstr::format("Parent::new_ns::C<T:={}>: T member={}\n", fstr::get_type_name<T>(), member);
  }
};
}  // namespace new_ns
   // TODO(deep): including the next line masks to_string
   // template<> class C<int> {};
}  // namespace Parent

//
// from https://en.cppreference.com/w/cpp/language/template_parameters
//
template <typename T>
class my_array {
  T x;
  // Generated to_string() for PUBLIC CLASS_TEMPLATE my_array<T>
  public:
  auto to_string() const {
    return fstr::format("my_array<T:={}>: T x={}\n", fstr::get_type_name<T>(), x);
  }
};

// two type template parameters and one template template parameter:
template <typename K, typename V, template <typename> typename C = my_array>
struct Map {
  C<K> key = {};
  C<V> value = {};
  // Generated to_string() for PUBLIC CLASS_TEMPLATE Map<K, V, C>
  public:
  auto to_string() const {
    return fstr::format("Map<K:={}, V:={}>: C<K> key={}, C<V> value={}\n", fstr::get_type_name<K>(), fstr::get_type_name<V>(), key, value);
  }
};
struct Y {
  struct B {
    int X;
    // Generated to_string() for PUBLIC STRUCT_DECL Y::B
  public:
  auto to_string() const {
    return fstr::format(": int X={}\n", X);
  }
};
  int C;
  int Y;
  // Generated to_string() for PUBLIC STRUCT_DECL Y
  public:
  auto to_string() const {
    return fstr::format(": int C={}, Y={}\n", C, Y);
  }
};

template <class B>
struct X : Y {
  B b;  // A's B
  // Generated to_string() for PUBLIC CLASS_TEMPLATE X<B>
  public:
  auto to_string() const {
    return fstr::format("X<B:={}>: B b={}, int C={}, Y={}\n", fstr::get_type_name<B>(), b, this->C, this->Y);
  }
};

int main()
{
  using namespace Test;
  using std::cout;
  cout << fmt::format("file: {}\ntime: {}\n", __FILE_NAME__, __TIMESTAMP__);

  cout << fmt::format(" old Func()=old_ns::Func()={} s = old_ns::s={} \n", old_ns::Func(), old_ns::s);
  cout << fmt::format(" new Func()=Func()={} s = {} \n", Func(), s);
  cout << fmt::format(" Base0()={} Base1()={} Derived0()={} Derived1()={} Derived2()={}\n", Base0(), Base1(), Derived0(), Derived1(), Derived2());

  // A::Derived<int> obj;
  // obj.x = 12;
  // obj.y = 21;
  A::Derived<int> obj{1, 2};
  cout << fmt::format(" obj={} \n", obj);

  Parent::C<int> c;
  cout << fmt::format(" c={} \n", c);

  Map<int, std::string> m1;  // uses my_array as default container type
  cout << fmt::format(" m1={} \n", m1);

  return 0;
}



