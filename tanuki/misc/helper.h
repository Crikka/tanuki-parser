#pragma once

#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>
#include <cstring>
#include <vector>
#include <iostream>

#include "utf8.h"

namespace tanuki {
template <typename T>
class OneUse {
 public:
  explicit OneUse(T *data) : m_data(data) {}

  ~OneUse() { delete m_data; }
  T exposeValue() { return (*m_data); }
  T *expose() { return m_data; }
  T *operator->() { return m_data; }

 private:
  T *m_data;
};

template <typename T>
OneUse<T> useOnce(T *data) {
  return OneUse<T>(data);
}

template <typename TTransformer, typename TToken>
typename TToken::TDeepType parse(TToken token, const char *input) {
  unsigned char* zz = TTransformer::convert(input);

  std::cout << zz << std::endl;

  //return token->match(zz);
}

template <typename TToken>
typename TToken::TDeepType parse(TToken token, const char *input) {
  return token->match(input);
}

class UTF16 {
public:
  static unsigned char *convert(const char *input) {
    std::vector<unsigned char> utf8result;
    utf8::utf16to8(input, input + strlen(input) + 1, back_inserter(utf8result));

    int size = utf8result.size();

    unsigned char* result = (unsigned char*) malloc(size*sizeof(unsigned char));
    for (int i = 0; i < size; i++) {
      result[i] = utf8result[i];
    }

    return result;
  }
};

/**
 * Bellow Code from StackOverflow by DRayX
 * ---------------------------------------
 */

template <size_t N>
struct Apply {
  template <typename F, typename T, typename... A>
  static inline auto apply(F &&f, T &&t, A &&... a) {
    return Apply<N - 1>::apply(::std::forward<F>(f), ::std::forward<T>(t),
                               ::std::get<N - 1>(::std::forward<T>(t)),
                               ::std::forward<A>(a)...);
  }
};

template <>
struct Apply<0> {
  template <typename F, typename T, typename... A>
  static inline auto apply(F &&f, T &&, A &&... a) {
    return ::std::forward<F>(f)(::std::forward<A>(a)...);
  }
};

template <typename F, typename T>
inline auto apply(F &&f, T &&t) {
  return Apply< ::std::tuple_size< ::std::decay_t<T> >::value>::apply(
      ::std::forward<F>(f), ::std::forward<T>(t));
}
}
