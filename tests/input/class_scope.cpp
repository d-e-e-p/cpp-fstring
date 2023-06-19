/**
 * @file class_scope.cpp
 * misc demo of cpp-fstring
 *
 * @ingroup examples
 *
 * @author Sandeep M
 * @copyright Copyright 2023 Sandeep M<deep@tensorfield.ag>
   @license MIT License
*/
/*************************************************************************//**
 *
 * @brief tests variable scope
 *
 *****************************************************************************/
#include <iostream>
#include "fstr.h"

// from https://github.com/muellan/clipp/blob/master/include/clipp.h

/// assigns boolean constant to one or multiple target objects
template<class T, class V = T>
class assign_value {
public:
    template<class X>
    explicit constexpr
    assign_value(T& target, X&& value) noexcept :
        t_{std::addressof(target)}, v_{std::forward<X>(value)}
    {}

    void operator () () const {
        if(t_) {
          *t_ = v_;
        }
    }

private:
    T* t_;
    V v_;
};

// from https://github.com/jarro2783/cxxopts/blob/master/include/cxxopts.hpp
 class KeyValue {
    public:
    KeyValue(std::string key_, std::string value_) noexcept
    : m_key(std::move(key_))
    , m_value(std::move(value_))
    {
    }
    private:
    std::string m_key;
    std::string m_value;
 };


class documentation {
  public:
    int foo = 0;
  private:
    enum class paragraph { param, group };
    paragraph bar = paragraph::param;
    // TODO(deep): find workaround for ::iterator and ::const_iterator issue with libclang
    // see https://github.com/llvm/llvm-project/issues/63277
    //std::vector<KeyValue>::const_iterator m_iter;
};

int main() {
  using std::cout;
  cout << "file: {__FILE_NAME__}\ntime: {__TIMESTAMP__}\n";

  bool target = false;
  auto as = assign_value<bool>{target,true};
  cout << " {as=} \n";
  documentation doc;
  cout << " {doc=} \n";
  return 0;
}
