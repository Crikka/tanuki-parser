#include "tokens.h"

#include <string>
#include <re2/re2.h>

namespace tanuki {
namespace lexer {
undirect_ref<ConstantToken> &space() {
  static undirect_ref<ConstantToken> res = constant(' ');

  return res;
}

undirect_ref<ConstantToken> &tab() {
  static undirect_ref<ConstantToken> res = constant('\t');

  return res;
}

undirect_ref<OrToken<ConstantToken, ConstantToken, std::string>> &blank() {
  static undirect_ref<OrToken<ConstantToken, ConstantToken, std::string>> res =
      (space() or tab());

  return res;
}

undirect_ref<OrToken<ConstantToken, ConstantToken, std::string>> &
lineTerminator() {
  static undirect_ref<OrToken<ConstantToken, ConstantToken, std::string>> res =
      (constant('\r') or constant('\n'));

  return res;
}

ConstantToken::ConstantToken(const std::string &constant)
    : Token<std::string>(), m_constant(constant) {}

ConstantToken::ConstantToken(char constant)
    : Token<std::string>(), m_constant({constant}) {}

ref<std::string> ConstantToken::match(const std::string &in) {
  return ((m_constant == in) ? ref<std::string>(new std::string(m_constant))
                             : ref<std::string>());
}

RegexToken::RegexToken(const std::string &regex)
    : Token<std::string>(), m_regex(new re2::RE2("(" + regex + ")")) {}

RegexToken::~RegexToken() { delete m_regex; }

ref<std::string> RegexToken::match(const std::string &in) {
  if ((RE2::FullMatch(in, *m_regex, &m_result))) {
    return ref<std::string>(new std::string(m_result));
  } else {
    return ref<std::string>();
  }
}

IntegerToken::IntegerToken() : Token<int>(), m_regex(new re2::RE2("(\\d+)")) {}

IntegerToken::~IntegerToken() { delete m_regex; }

ref<int> IntegerToken::match(const std::string &in) {
  if ((RE2::FullMatch(in, *m_regex, &m_result))) {
    return ref<int>(new int(m_result));
  } else {
    return ref<int>();
  }
}

// Helper
undirect_ref<ConstantToken> constant(char character) {
  return undirect_ref<ConstantToken>(new ConstantToken(character));
}

undirect_ref<ConstantToken> constant(const std::string &constant) {
  return undirect_ref<ConstantToken>(new ConstantToken(constant));
}

undirect_ref<RegexToken> regex(const std::string &regex) {
  return undirect_ref<RegexToken>(new RegexToken(regex));
}

undirect_ref<IntegerToken> integer() {
  return undirect_ref<IntegerToken>(new IntegerToken());
}
}
}
