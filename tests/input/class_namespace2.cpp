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
    };

    template <typename T>
    class Derived : public Base<T> {
    public:
        T y;
    };
} // end namespace A


// from https://learn.microsoft.com/en-us/cpp/cpp/namespaces-cpp?view=msvc-170
// test inline namespace
namespace Test {
    namespace old_ns {
        auto Func() { return std::string("Hello from old"); }
        struct S { int a{0}; } s;
    }

    inline namespace new_ns {
        auto Func() { return std::string("Hello from new"); }
        struct S { int a{1}; } s;
    }
}

//
// from https://devdocs.io/cpp/language/derived_class
struct Base0 {
    int a, b , base0;
};

struct Base1 {
    int c, d , base1;
};

// every object of type Derived includes Base as a subobject
struct Derived0 : Base0 {
    int a {10}, derived0;
};

struct Derived1 : Base1 {
    int c {21}, derived1;
};


// every object of type Derived2 includes Derived and Base as subobjects
struct Derived2 : Derived0, Derived1 {
    int b{42}, d{42}, derived2;
};

namespace Parent {
    inline namespace new_ns {
         template <typename T>
         struct C {
             T member {};
         };
    }
    // TODO(deep): including the next line masks to_string
    // template<> class C<int> {};
}

//
// from https://en.cppreference.com/w/cpp/language/template_parameters
//
template<typename T>
class my_array {
  T x;
};

// two type template parameters and one template template parameter:
template<typename K, typename V, template<typename> typename C = my_array>
struct Map
{
    C<K> key = {};
    C<V> value = {};
};
struct Y {
    struct B {
      int X;
    };
    int C;
    int Y;
};

template<class B>
struct X : Y {
    B b; // A's B
};


int main() {
  using namespace Test;
  using std::cout;
  cout << "file: {__FILE_NAME__}\ntime: {__TIMESTAMP__}\n";

    cout << " old Func()={old_ns::Func()=} s = {old_ns::s=} \n";
    cout << " new Func()={Func()=} s = {s} \n";
    cout << " {Base0()=} {Base1()=} {Derived0()=} {Derived1()=} {Derived2()=}\n";

    //A::Derived<int> obj;
    //obj.x = 12;
    //obj.y = 21;
    A::Derived<int> obj{1, 2};
    cout << " {obj=} \n";

    Parent::C<int> c;
    cout << " {c=} \n";

    Map<int, std::string> m1; // uses my_array as default container type
    cout << " {m1=} \n";


    return 0;
}
