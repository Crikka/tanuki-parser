#include "tokens.h"

#include <string>

#include "operation.h"
#include "special.h"

namespace tanuki {
ConstantToken::ConstantToken(const std::string &constant)
    : Token<std::string>(), m_constant(constant) {}

ref<std::string> ConstantToken::match(const tanuki::String &in) {
  return ((in == m_constant) ? ref<std::string>(new std::string(m_constant))
                             : ref<std::string>());
}

Collect<std::string> ConstantToken::collect(const tanuki::String &in) {
  int length = m_constant.size();

  if (in.size() < length) {
    return std::make_pair(0, ref<std::string>());
  } else {
    bool result = true;

    for (int i = 0; i < length; i++) {
      if (in[i] != m_constant[i]) {
        result = false;
        break;
      }
    }

    if (result) {
      return std::make_pair(length,
                            ref<std::string>(new std::string(m_constant)));
    } else {
      return std::make_pair(0, ref<std::string>());
    }
  }
}

CharToken::CharToken(char character) : Token<char>(), m_character(character) {}

ref<char> CharToken::match(const tanuki::String &in) {
  if (in.size() == 1) {
    return ((in[0] == m_character) ? ref<char>(new char(m_character))
                                   : ref<char>());
  } else {
    return ref<char>();
  }
}

Collect<char> CharToken::collect(const tanuki::String &in) {
  if (in.empty()) {
    return std::make_pair(0, ref<char>());
  } else {
    if (in[0] == m_character) {
      return std::make_pair(1, ref<char>(new char(m_character)));
    } else {
      return std::make_pair(0, ref<char>());
    }
  }
}

IntegerToken::IntegerToken() : Token<int>() {}

ref<int> IntegerToken::match(const tanuki::String &in) {
  static auto inner(word(anyIn('0', '9')));

  ref<std::string> result(inner->match(in));

  if (result) {
    return ref<int>(
        new int(std::stoi(useOnce(result.release()).exposeValue())));
  } else {
    return ref<int>();
  }
}

Collect<int> IntegerToken::collect(const tanuki::String &in) {
  static auto inner(word(anyIn('0', '9')));

  Collect<std::string> result(inner->collect(in));

  if (result.second) {
    return std::make_pair(
        result.first, ref<int>(new int(std::stoi(
                          useOnce(result.second.release()).exposeValue()))));
  } else {
    return std::make_pair(0, ref<int>());
  }
}

AnyOfToken::AnyOfToken(std::vector<char> initial) : AnyOfToken() {
  for (char c : initial) {
    this->m_intern[c] = true;
  }
}

AnyOfToken::AnyOfToken() : Token<char>() {
  for (int i = 0; i < CHAR_MAX; i++) {
    this->m_intern[i] = false;
  }
}

void AnyOfToken::validate(char character) { this->m_intern[character] = true; }

ref<char> AnyOfToken::match(const tanuki::String &in) {
  if (in.size() == 1) {
    return ((this->m_intern[in[0]]) ? ref<char>(new char(in[0])) : ref<char>());
  } else {
    return ref<char>();
  }
}

Collect<char> AnyOfToken::collect(const tanuki::String &in) {
  if (in.empty()) {
    return std::make_pair(0, ref<char>());
  } else {
    if (this->m_intern[in[0]]) {
      return std::make_pair(1, ref<char>(new char(in[0])));
    } else {
      return std::make_pair(0, ref<char>());
    }
  }
}

AnyInToken::AnyInToken(char inferiorBound, char superiorBound)
    : Token<char>(),
      m_inferiorBound(inferiorBound),
      m_superiorBound(superiorBound) {}

ref<char> AnyInToken::match(const tanuki::String &in) {
  if (in.size() == 1) {
    return (((in[0] >= m_inferiorBound) and (in[0] <= m_superiorBound))
                ? ref<char>(new char(in[0]))
                : ref<char>());
  } else {
    return ref<char>();
  }
}

Collect<char> AnyInToken::collect(const tanuki::String &in) {
  if (in.empty()) {
    return std::make_pair(0, ref<char>());
  } else {
    if ((in[0] >= m_inferiorBound) and (in[0] <= m_superiorBound)) {
      return std::make_pair(1, ref<char>(new char(in[0])));
    } else {
      return std::make_pair(0, ref<char>());
    }
  }
}
}
