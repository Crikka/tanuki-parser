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
}
