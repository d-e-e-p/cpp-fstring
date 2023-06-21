/**
 * @file class_template1.cpp
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

// from
// https://stackoverflow.com/questions/65781641/im-trying-to-format-a-template-using-fmt
template <typename T, T Min, T Max>
class LimitedInt {
  T mValue{Min};

 public:
  explicit LimitedInt(const T value) { setValue(value); }
  void setValue(const T value) { mValue = value; }
  T getValue() const { return mValue; }
};

template <auto n>
struct Auto {
  int v = 1;
};

#include <array>
template <auto... Values>
struct ValueList {
  static constexpr auto values = std::array{Values...};
};

template <int N0, int N1, int N2, int N3>
struct Const {
  // making this static would optimize out v!
  // static const int v = N0 + N1 + N2 + N3;
  int v = N0 + N1 + N2 + N3;
};

template <typename T>
class my_array {};

// two type template parameters and one template template parameter:
template <typename K, typename V, template <typename> typename C = my_array>
class Map {
  C<K> key;
  C<V> value;
};

struct A {
  struct B {
    int X;
  };
  int C;
  int Y;
};

template <class B>
struct X : A {
  B b;  // A's B
};

template <typename T>
struct Pair {
  T first{};
  T second{};
};

// simple non-type template parameter
template <typename T, int N>
struct S {
  T a[N] = {};
};

enum class Cowboys { good, bad, ugly };

// from https://codereview.stackexchange.com/questions/279379/template-complex-class
#include <iostream>
#include <type_traits>

template <typename Ty>
class Complex {
  static_assert(std::is_arithmetic_v<Ty>, "Complex requires an arithmetic type.");

 public:
  Complex(const Ty& r, const Ty& i) noexcept : r(r), i(i) {}

  Ty real() const noexcept { return r; }
  Ty imag() const noexcept { return i; }

 private:
  Ty r, i;
};

int main()
{
  using std::cout;
  cout << "file: {__FILE_NAME__}\ntime: {__TIMESTAMP__}\n";

  cout << "{ValueList<12, 24, 42>()=}";
  cout << "{ValueList<'a', 'b', 'c'>()=}";
  cout << "{Const<1,1,1,1>()=}";
  cout << "{LimitedInt<uint16_t, 0, 4094>(10)=}";
  cout << "{Auto<'a'>()=}";

  S<bool, 10> s;
  s.a[9] = true;
  cout << "{s=}";

  S<Cowboys, 10> cb;
  cb.a[0] = Cowboys::bad;
  cout << "{cb=}";

  Pair<double> pd{7.8, 9.0};
  cout << "{pd=}";

  Pair<Cowboys> pc{Cowboys::bad, Cowboys::good};
  cout << "{pc=}";

  Complex<double> xa(1.2, 2.25);
  Complex<int> xb(2, -1);
  cout << "Complex<double> {xa=}";
  cout << "Complex<int> {xb=}";
}
