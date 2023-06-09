
    .. image:: https://api.cirrus-ci.com/github/d-e-e-p/cpp_fstring.svg?branch=main
        :alt: Built Status
        :target: https://cirrus-ci.com/github/d-e-e-p/cpp_fstring
    .. image:: https://readthedocs.org/projects/cpp_fstring/badge/?version=latest
        :alt: ReadTheDocs
        :target: https://cpp_fstring.readthedocs.io/en/stable/
    .. image:: https://img.shields.io/coveralls/github/d-e-e-p/cpp_fstring/main.svg
        :alt: Coveralls
        :target: https://coveralls.io/r/d-e-e-p/cpp_fstring
    .. image:: https://img.shields.io/pypi/v/cpp_fstring.svg
        :alt: PyPI-Server
        :target: https://pypi.org/project/cpp_fstring/
    .. image:: https://img.shields.io/conda/vn/conda-forge/cpp_fstring.svg
        :alt: Conda-Forge
        :target: https://anaconda.org/conda-forge/cpp_fstring


|

=========================================
cpp-fstring: python style f-string in C++
=========================================

cpp-fstring is a C++ code processor that expands {var} type statements inside strings
to equivalent fmt::format commands. So you can do things like:

.. code-block:: CPP

    enum class Color { red, yellow, green, blue };
    enum class Fruit { orange, apple, banana };
    std::map<Color, std::vector<Fruit>> mc = {
      {Color::red,    {Fruit::apple}},
      {Color::yellow, {Fruit::apple, Fruit::banana}},
    };
    ...
    std::cout << "fruit by colors: {mc=} \n";


result:

.. code-block:: sh

    fruit by colors: mc = {red: [apple], yellow: [apple, banana]}

cpp-fstring generates the boilerplate code to display enums and containers like structs and classes.

Credits
=======

-  This project has been set up using `PyScaffold <https://pyscaffold.org/>`__
-  Useful post explaining `C++ templates <https://victor-istomin.github.io/c-with-crosses/posts/templates-are-easy/>`__
-  Discussions on stackoverflow like `enum to string in modern C++ <https://stackoverflow.com/questions/28828957/enum-to-string-in-modern-c11-c14-c17-and-future-c20>`__
-  C++ formatting library `{fmt} <https://fmt.dev/latest/index.html>`__
-  Python interface to the `Clang indexing library <https://libclang.readthedocs.io/en/latest/>`__
-  Packaged version of Clang Python Bindings `libclang <https://pypi.org/project/libclang/>`__

Motivation
==========

Just got tired waiting for python style f-strings in C++ .
Proposals like `Interpolated literals <https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p1819r0.html>`__
seem to hit the "lack of reflection" brick wall.  Hopefully by C++30 we could do something like the
`Scalable Reflection in C++ <https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p1240r2.pdf>`__ proposal:

.. code-block:: CPP

    #include <meta>
    template<Enum T>
    std::string to_string(T value) {
      template for (constexpr auto e: std::meta::members_of(^T)) {
        if ([:e:] == value) {
          return std::string(std::meta::name_of(e));
        }
      }
      return "<unnamed>";
    }

In the mean time, cpp-fstring cheats by pre-processing C++17 code. So:

.. code-block:: CPP

    enum class Color { red, yellow, green = 20, blue };
    std::cout << "the fruit is {Color::yellow}\n";

explodes into:

.. code-block:: CPP

    enum class Color { red, yellow, green = 20, blue };
    std::cout << fmt::format("the fruit is {}\n", Color::yellow);
    // Generated formatter for PUBLIC enum Color of type INT scoped
    constexpr auto format_as(const Color obj) {
      fmt::string_view name = "<missing>";
      switch (obj) {
        case Color::red   : name = "red"   ; break;  // index=0
        case Color::yellow: name = "yellow"; break;  // index=1
        case Color::green : name = "green" ; break;  // index=20
        case Color::blue  : name = "blue"  ; break;  // index=21
      }
      return name;
    }

while:

.. code-block:: CPP

    template <typename T, template<typename...> class C>
    class Container {
    public:
        void addData(const T& data) {
            container.push_back(data);
        }

    private:
        C<T> container;
    };

gets an extra `to_string()` function:

.. code-block:: CPP

    template <typename T, template<typename...> class C>
    class Container {
    public:
        void addData(const T& data) {
            container.push_back(data);
        }

    private:
        C<T> container;
    public:
       // Generated to_string() for PUBLIC CLASS_TEMPLATE Container<T, C>
       auto to_string() const {
         return fstr::format("Container<T:={}>: C<T> container={}", fstr::get_type_name<T>(), container);
      }
    };




Install
=======

To install the tool, use:

.. code-block:: sh

    pip install cpp-fstring

The following command then converts foo.cc into foo.cpp:

.. code-block:: sh

    cpp-fstring foo.cc -I ../include > foo.cpp

You also need to add this include to foo.cc:

.. code-block:: CPP

    #include "fstr.h"

`fstr.h <https://github.com/d-e-e-p/cpp-fstring/blob/main/src/cpp_fstring/include/fstr.h>`__ contains helper routines
needed to stringify enums and classes.  An example of using cpp-fstring in cmake environment
is at `cpp-fstring-examples <https://github.com/d-e-e-p/cpp-fstring-examples>`__

At present only clang is supported--gcc and MinGW are in-progress.

There are 2 main dependencies: python libclang library to run cpp-format and C++ fmt library to display objects.
In cmake environment the best way to pickup latest version of `fmt <https://fmt.dev/latest/index.html>`__ library is:

.. code-block:: sh

    CPMAddPackage(NAME fmt SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/fmt" GIT_REPOSITORY "https://github.com/fmtlib/fmt.git" GIT_TAG "master")

or:

    CPMAddPackage("gh:fmtlib/fmt#10.0.0")

Generated code needs at least `10.0.0 <https://github.com/fmtlib/fmt/releases/tag/10.0.0>`__ version of fmt.
cpp-fstring also needs `libclang <https://pypi.org/project/libclang/>`__ :

.. code-block:: sh

    pip install libclang

libclang installs the dynamic library file (`libclang.dylib`, `libclang.dll` or `libclang.so`)
in a path like `/opt/homebrew/lib/python3.11/site-packages/clang/native/libclang.dylib` .
If an incorrect version of library file is installed, you can get a strange error like `this <https://github.com/sighingnow/libclang/issues/54>`__
You can download a more recent version of libclang library from:

.. code-block:: sh

    https://github.com/llvm/llvm-project/releases/

The only file you need from binary distribution is the libclang dynamic lib for your machine, ie `libclang.dylib`, `libclang.dll` or `libclang.so`.
You might have to copy this file to the `native` directory of python clang lib.

Usage: What Works
=================

See `demo_misc.cpp <https://github.com/d-e-e-p/cpp-fstring-examples/blob/main/examples/psrc/demo_misc.cpp>`__
for a demo of Format Specifiers, Dates, Expressions and Ranges:

.. code-block:: CPP

  using IArr =  std::valarray<int>;
  IArr a {1,2,3};
  IArr b {4,5,6};
  IArr ab = std::pow(a, b);
  IArr ba = std::pow(b, a);
  IArr abba = ab+ba;

  cout <<  R"(
    Valarray:
      a^b + b^a = {a}^{b} + {b}^{a}
                = {ab} + {ba}
                = {abba}

      min({abba}) = {abba.min()}
      sum({abba}) = {abba.sum()}
      max({abba}) = {abba.max()}
   )" ;

outputs:

.. code-block:: sh

    Valarray:
      a^b + b^a = [1, 2, 3]^[4, 5, 6] + [4, 5, 6]^[1, 2, 3]
                = [1, 32, 729] + [4, 25, 216]
                = [5, 57, 945]

      min([5, 57, 945]) = 5
      sum([5, 57, 945]) = 1007
      max([5, 57, 945]) = 945


See `enum_namespace.cpp <https://github.com/d-e-e-p/cpp-fstring-examples/blob/main/examples/psrc/enum_namespace.cpp>`__ for examples of simple enums:

.. code-block:: CPP

    namespace roman {
      enum class sym {M, D, C, L, X, V, I};
      std::map<sym, int> numerals = {
        {sym::M, 1000},
        {sym::D,  500},
        {sym::C,  100},
        {sym::L,   50},
        {sym::X,   10},
        {sym::V,    5},
        {sym::I,    1}
      };
    }  // namespace roman

    ...
    std::cout << " {roman::numerals=}\n";

outputs:

.. code-block:: sh

    roman::numerals={M: 1000, D: 500, C: 100, L: 50, X: 10, V: 5, I: 1}

See `class_ctad.cpp <https://github.com/d-e-e-p/cpp-fstring-examples/blob/main/examples/psrc/class_ctad.cpp>`__ for an example of derived template classes:

.. code-block:: CPP

    #include <iostream>
    #include "fstr.h"

    template<class T>
    struct A {
        T t;
        struct {
            long a, b;
        } u;
    };


    template<class T>
    struct B {
        T t;
        A<T> a;
    };

    int main() {
      using std::cout;
      auto b = B<int>{1, {2,{3,4}}};
      cout << " {b=}";
    }

outputs:

.. code-block:: sh

     b=B<T:=int>: T t=1, A<T> a=A<T:=int>: T t=2, long u.a=3, u.b=4


Usage: What Doesn't Work
========================

4 underlying reasons behind stuff that doesn't work:

1. Bugs in libclang, eg

* iterator class variables are incorrectly parsed. See this `issue <https://github.com/llvm/llvm-project/issues/63277>`__ :

.. code-block:: cpp

   const std::vector<int>::const_iterator i_iter;

* base class with templates are sometimes missing in the derived class, so x doesn't show up when dumping Y() :

.. code-block:: cpp

    template <typename T> class X {
      public:
      T x;
    };

    class Y : public X<bool> {
      int y = 13;
    };

* missing vector variable in class, see `issue <https://github.com/llvm/llvm-project/issues/63372>`__ :

.. code-block:: cpp

    struct Map {
      std::map<int, std::vector<int>> m_is_invisible;
    };


2. Limitations in fmt:: library, eg wchar_t is not completely supported even with xchar.h:

.. code-block:: cpp

    #include <fmt/xchar.h>
    #include <fmt/format.h>
    #include <fmt/ranges.h>
    #include <map>

    int main() {
      // works
      std::map<int, char> box1 = { {1,  L'⎧'}, {2,  L'╭'} };
      fmt::print("box1: {}\n", box1);

      // doesn't work..needs additional formatter to be defined to handle wchar_t
      std::map<int, wchar_t> box2 = { {1,  L'⎧'}, {2,  L'╭'} };
      fmt::print("box2: {}\n", box2);
      return 0;
    }

3. C++ features, eg inside functions we can't have other functions or template struct so
   there is no way to define a formatter for `enum line` in :

.. code-block:: cpp

    int main() {
        //can't print enum becaause it's inside main()
        enum class line { words, spaces };
    }

4. Bugs/limitations of cpp-fstring.

* majority of bugs are of course in this section, eg code with:
    * ambiguous partial specializations
    * template parameter packs
    * ...

  Perfect segway to contributing.

Making Changes & Contributing
=============================

This project uses `pre-commit <https://pre-commit.com/>` :

.. code-block:: sh

    cd cpp_fstring
    pip install pre-commit
    pre-commit install
    pre-commit autoupdate


Authors
=======

**Sandeep** - `@d-e-e-p <https://github.com/d-e-e-p>`

## License

The project is available under the `MIT <https://opensource.org/licenses/MIT>` license.
See `LICENSE` file for details
