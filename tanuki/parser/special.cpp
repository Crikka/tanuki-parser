#include "special.h"

#include "operation.h"

namespace tanuki {
undirect_ref<ConstantToken> constant(const std::string &constant) {
  return undirect_ref<ConstantToken>(new ConstantToken(constant));
}

undirect_ref<CharToken> constant(char character) {
  return undirect_ref<CharToken>(new CharToken(character));
}

undirect_ref<IntegerToken> integer() {
  return undirect_ref<IntegerToken>(new IntegerToken());
}

undirect_ref<AnyInToken> anyIn(char inferiorBound, char superiorBound) {
  return undirect_ref<AnyInToken>(new AnyInToken(inferiorBound, superiorBound));
}

undirect_ref<CharToken> space() {
  return constant(' ');
}

undirect_ref<CharToken> tab() {
  return constant('\t');
}

undirect_ref<OrToken<CharToken, CharToken>> blank() {
  return (space() or tab());
}

undirect_ref<OrToken<CharToken, CharToken>> lineTerminator() {
  return (constant('\r') or constant('\n'));
}

undirect_ref<AnyInToken> digit() {
  return anyIn('0', '9');
}

undirect_ref<AnyInToken> letter() {
  return anyIn('A', 'z');
}

undirect_ref<AnyOfToken> anyOf(char c) {
  undirect_ref<AnyOfToken> result(new AnyOfToken());
  result->validate(c);

  return result;
}
}
