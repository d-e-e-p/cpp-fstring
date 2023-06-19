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

// from https://stackoverflow.com/questions/65781641/im-trying-to-format-a-template-using-fmt
template <typename T, T Min, T Max>
class LimitedInt {
   T mValue{Min};
public:
   explicit LimitedInt(const T value) {
      setValue(value);
   }
   void setValue(const T value) {
      mValue = value;
   }
   T getValue() const {
      return mValue;
   }
// Generated to_string for PUBLIC CLASS_TEMPLATE LimitedInt<T, Min, Max>
  public:
  auto to_string() const {
    return fstr::format(R"( LimitedInt<T, Min, Max>: <{}> mValue={}
)", typeid(T).name(), mValue);
  }
};

template<auto n>
struct Auto {
  int v = 1;
// Generated to_string for PUBLIC CLASS_TEMPLATE Auto<n>
  public:
  auto to_string() const {
    return fstr::format(R"( Auto<n>: int v={}
)", v);
  }
};


#include <array>
template <auto... Values>
struct ValueList {
    static constexpr auto values = std::array{Values...};
// Generated to_string for PUBLIC CLASS_TEMPLATE ValueList<Values>
  public:
  auto to_string() const {
    return fstr::format(R"( ValueList<Values>: const auto values={}
)", values);
  }
};


template<int N0,int N1,int N2,int N3>
struct Const {
  // making this static would optimize out v!
	//static const int v = N0 + N1 + N2 + N3;
	int v = N0 + N1 + N2 + N3;
// Generated to_string for PUBLIC CLASS_TEMPLATE Const<N0, N1, N2, N3>
  public:
  auto to_string() const {
    return fstr::format(R"( Const<N0, N1, N2, N3>: int v={}
)", v);
  }
};

template<typename T>
class my_array {// Generated to_string for PUBLIC CLASS_TEMPLATE my_array<T>
  public:
  auto to_string() const {
    return fstr::format(R"( my_array<T>: 
)");
  }
};

// two type template parameters and one template template parameter:
template<typename K, typename V, template<typename> typename C = my_array>
class Map {
    C<K> key;
    C<V> value;
// Generated to_string for PUBLIC CLASS_TEMPLATE Map<K, V, C>
  public:
  auto to_string() const {
    return fstr::format(R"( Map<K, V, C>: C<K> key={}, C<V> value={}
)", key, value);
  }
};

struct A {
    struct B {
      int X;
    // Generated to_string for PUBLIC STRUCT_DECL A::B
  public:
  auto to_string() const {
    return fstr::format(R"( A::B: int X={}
)", X);
  }
};
    int C;
    int Y;
// Generated to_string for PUBLIC STRUCT_DECL A
  public:
  auto to_string() const {
    return fstr::format(R"( A: int C={}, Y={}
)", C, Y);
  }
};

template<class B>
struct X : A {
    B b; // A's B
// Generated to_string for PUBLIC CLASS_TEMPLATE X<B>
  public:
  auto to_string() const {
    return fstr::format(R"( X<B>: <{}> b={}, int C={}, Y={}
)", typeid(B).name(), b, this->C, this->Y);
  }
};


template <typename T>
struct Pair {
    T first{};
    T second{};
// Generated to_string for PUBLIC CLASS_TEMPLATE Pair<T>
  public:
  auto to_string() const {
    return fstr::format(R"( Pair<T>: <{}> first={}, second={}
)", typeid(T).name(), first, second);
  }
};

// simple non-type template parameter
template<typename T, int N>
struct S {T a[N] = {}; // Generated to_string for PUBLIC CLASS_TEMPLATE S<T, N>
  public:
  auto to_string() const {
    return fstr::format(R"( S<T, N>: T[N] a={}
)", a);
  }
};

enum class Cowboys {good, bad, ugly};

// from https://codereview.stackexchange.com/questions/279379/template-complex-class
#include <type_traits>
#include <iostream>

template<typename Ty>
class Complex {
    static_assert(std::is_arithmetic_v<Ty>, "Complex requires an arithmetic type.");

public:
    Complex(const Ty& r, const Ty& i) noexcept :
        r(r), i(i)
    {}

    Ty real() const noexcept { return r; }
    Ty imag() const noexcept { return i; }

private:
    Ty r, i;
// Generated to_string for PUBLIC CLASS_TEMPLATE Complex<Ty>
  public:
  auto to_string() const {
    return fstr::format(R"( Complex<Ty>: <{}> r={}, i={}
)", typeid(Ty).name(), r, i);
  }
};


int main() {
  using std::cout;
  cout << fmt::format("file: {}\ntime: {}\n", __FILE_NAME__, __TIMESTAMP__);

  cout << fmt::format("ValueList<12, 24, 42>()={}", ValueList<12, 24, 42>());
  cout << fmt::format("ValueList<'a', 'b', 'c'>()={}", ValueList<'a', 'b', 'c'>());
  cout << fmt::format("Const<1,1,1,1>()={}", Const<1,1,1,1>());
  cout << fmt::format("LimitedInt<uint16_t, 0, 4094>(10)={}", LimitedInt<uint16_t, 0, 4094>(10));
  cout << fmt::format("Auto<'a'>()={}", Auto<'a'>());

  S<bool,10> s;
  s.a[9] = true;
  cout << fmt::format("s={}", s);

  S<Cowboys,10> cb;
  cb.a[0] = Cowboys::bad;
  cout << fmt::format("cb={}", cb);

  Pair<double> pd{ 7.8, 9.0 };
  cout << fmt::format("pd={}", pd);

  Pair<Cowboys> pc{ Cowboys::bad, Cowboys::good };
  cout << fmt::format("pc={}", pc);

  Complex<double> xa(1.2, 2.25);
  Complex<int> xb(2, -1);
  cout << fmt::format("Complex<double> xa={}", xa);
  cout << fmt::format("Complex<int> xb={}", xb);

}

// Generated formatter for PUBLIC enum Cowboys of type INT scoped
constexpr auto format_as(const Cowboys obj) {
  fmt::string_view name = "<missing>";
  switch (obj) {
    case Cowboys::good: name = "good"; break;  // index=0
    case Cowboys::bad : name = "bad" ; break;  // index=1
    case Cowboys::ugly: name = "ugly"; break;  // index=2
  }
  return name;
}

