
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

cpp-fstring is a C++ code processor that expands any {var} type statements inside strings
to equivalent fmt::format commands. So you can do things like:

.. code-block:: CPP

    enum class Color { red, yellow, green, blue };
    enum class Fruit { orange, apple, banana };
    std::map<Color, std::vector<Fruit>> mc = {
      {Color::red,    {Fruit::apple}},
      {Color::yellow, {Fruit::apple, Fruit::banana}},
    };
    std::cout << "fruit by colors: {mc=} \n";


which produces the output:

.. code-block:: sh

    fruit by colors: mc = {red: [apple], yellow: [apple, banana]}

cpp-fstring generates the boilerplate code to display a variety of containers including
enums, structs and classes.

Credits
=======

-  This project has been set up using `PyScaffold <https://pyscaffold.org/>`__
-  Posts explaining `C++ template <https://victor-istomin.github.io/c-with-crosses/posts/templates-are-easy/>`__
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

In the mean time, cpp-fstring cheats by pre-processing. so the C++17 code :

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

Usage
=====

To install the tool, use:

.. code-block:: sh

    pip install cpp-fstring

The following command then converts foo.cc into foo.cpp:

.. code-block:: sh

    cpp-fstring foo.cc -I ../include > foo.cpp

You also need to add this to foo.cc:

.. code-block:: CPP

    #include "fstr.h"

`fstr.h <src/cpp_fstring/include/fstr.h>`__ contains helper routines needed to stringify enums and classes.
An example of using cpp-fstring in cmake environment is at `cpp-fstring-examples <https://github.com/d-e-e-p/cpp-fstring-examples>`__

There are 2 dependencies to install. fmt using one of:

.. code-block:: sh

    sudo apt install libfmt-dev  # or
    brew install fmt
    vcpkg install fmt
    conda install -c conda-forge fmt

and libclang:

.. code-block:: sh

    pip install libclang

What Works
==========

`Examples <https://github.com/d-e-e-p/cpp-fstring-examples/blob/main/examples/psrc/demo_misc.cpp>`__ of Format Specifiers, Dates, Expressions and Ranges:

.. code-block:: CPP

    using IArr =  std::valarray<int>;
    IArr ia {1,2,3};
    IArr ib {4,5,6};
    IArr iab = std::pow(ia, ib);
    IArr iba = std::pow(ib, ia);
    IArr iabba = iab+iba;

    cout <<  R"(
      Valarray:
        a^b + b^a = {ia}^{ib} + {ib}^{ia}
                  = {iab} + {iba}
                  = {iabba}

        min({iabba}) = {iabba.min()}
        sum({iabba}) = {iabba.sum()}
        max({iabba}) = {iabba.max()}
      

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


`Example <https://github.com/d-e-e-p/cpp-fstring-examples/blob/main/examples/psrc/enum_namespace.cpp>`__ of enum in namespaces:

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

`class_ctad.cpp <https://github.com/d-e-e-p/cpp-fstring-examples/blob/main/examples/psrc/class_ctad.cpp>`__ example of derived template classes:

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

      A<int> a{1,{2,3}};
      auto b = B<int>{1, {2,{3,4}}};
      cout << "{a=} {b=}";
      cout << "\n";

    }

outputs:

.. code-block:: sh

     b= B<T>:
         T=i t: 1
         A<T> a:  A<T>:
         T=i t: 2
            long u.a: 3
            long u.b: 4


What Doesn't Work
=================

4 catagories of stuff that doesn't work.

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


2. Limitations in fmt:: library, eg wchar_t is not supported:

.. code-block:: cpp
       static const std::unordered_map<int, wchar_t> k_escapes = {
        {  0,   L'•' }, 
        {  1,   L'␁' }
       };

3. Features of C++, eg inside functions we can't have other functions or template struct
   
.. code-block:: cpp
    int main() {
        //can't print enum
        enum class paragraph { param, group };
    }

4. Bugs/limitations of cpp-fstring.  
* majority of bugs are of course in this section. Perfect segway to contributing. 

Making Changes & Contributing
=============================

This project uses `pre-commit <https://pre-commit.com/>` :::

    pip install pre-commit
    cd cpp_fstring
    pre-commit install
    pre-commit autoupdate


Authors
=======

**Sandeep** - `@d-e-e-p <https://github.com/d-e-e-p>`

## License

The project is available under the `MIT <https://opensource.org/licenses/MIT>` license.
See `LICENSE` file for details
