
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
to equivalent fmt::format commands. So you can do things like::

    enum class Color { red, yellow, green, blue };
    enum class Fruit { orange, apple, banana };
    std::map<Color, std::vector<Fruit>> mc2f = {
      {Color::red,    {Fruit::apple}},
      {Color::yellow, {Fruit::apple, Fruit::banana}},
    };
    std::cout << "we like to display the fruit by colors: {mc2f} \n";

the script generates the additional boilerplate code to display a variety of types including
enums, simple structs and classes.

Motivation
==========

Just got tired waiting for python style f-strings in C++ .
Proposals like `Interpolated literals <https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p1819r0.html>`_
seem to dead end at C++'s lack of reflection.  There is slow and steady march towards real reflection so we could
do something like:::

    #include <meta>
    template<Enum T>
    std::string to_string(T value) {
      template for (constexpr auto e : std::meta::members_of(^T)) {
        if ([:e:] == value) {
          return std::string(std::meta::name_of(e));
        }
      }
      return "<unnamed>";
    }

But `Scalable Reflection in C++ <https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p1240r2.pdf>`_ seems be 
be on track for C++30 or beyond.

Meanwhile, cpp-fstring cheats by generating code before compile giving explicit instructions on displaying enum/classes etc.
So the code:::

    enum class Color { red, yellow, green = 20, blue };
    std::cout << "the fruit is {Color::yellow}\n";

explodes into:::

    enum class Color { red, yellow, green = 20, blue };
    std::cout << fmt::format("the fruit is {}\n", Color::yellow);

    // Generated formatter for enum Color of type INT scoped True
    template <>
    struct fmt::formatter<Color>: formatter<string_view> {
      template <typename FormatContext>
      auto format(Color val, FormatContext& ctx) const {
        string_view name = "<unknown>";
        switch (val) {
            case Color::red   : name = "red"   ; break;  // index=0
            case Color::yellow: name = "yellow"; break;  // index=1
            case Color::green : name = "green" ; break;  // index=20
            case Color::blue  : name = "blue"  ; break;  // index=21
        }
        return formatter<string_view>::format(name, ctx);
      }
    };


Usage
=====

To install the tool, use:::

    pip install cpp-fstring

The following command then converts foo.cc into foo.cpp:::

    cpp-fstring foo.cc -I ../include > foo.cpp

cpp-fstring can be incorporated in a cmake environment by either moving files to be
processed into another dir or using different suffix. see cpp-fstring-examples

.. _pyscaffold-notes:

Making Changes & Contributing
=============================

This project uses `pre-commit`_, please make sure to install it before making any
changes::

    pip install pre-commit
    cd cpp_fstring
    pre-commit install

It is a good idea to update the hooks to the latest version::

    pre-commit autoupdate

Don't forget to tell your contributors to also install and use pre-commit.

.. _pre-commit: https://pre-commit.com/

Note
====

This project has been set up using PyScaffold 4.4.1. For details and usage
information on PyScaffold see https://pyscaffold.org/.
