#include "special.h"

#include "operation.h"

namespace tanuki {
ref<ConstantToken> constant(const std::string &constant) {
  return ref<ConstantToken>(new ConstantToken(constant));
}

ref<CharToken> constant(char character) {
  return ref<CharToken>(new CharToken(character));
}

ref<IntegerToken> integer() {
  return ref<IntegerToken>(new IntegerToken());
}

ref<AnyInToken> anyIn(char inferiorBound, char superiorBound) {
  return ref<AnyInToken>(new AnyInToken(inferiorBound, superiorBound));
}

ref<CharToken> space() {
  return constant(' ');
}

ref<CharToken> tab() {
  return constant('\t');
}

ref<OrToken<CharToken, CharToken>> blank() {
  return (space() or tab());
}

ref<OrToken<CharToken, CharToken>> lineTerminator() {
  return (constant('\r') or constant('\n'));
}

ref<AnyInToken> digit() {
  return anyIn('0', '9');
}

ref<AnyInToken> letter() {
  return anyIn('A', 'z');
}

ref<AnyOfToken> anyOf(char c) {
  ref<AnyOfToken> result(new AnyOfToken());
  result->validate(c);

  return result;
}
}
