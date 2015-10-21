#pragma once

#include "tanuki/misc/ref.h"
#include "tokens.h"

namespace tanuki {
ref<ConstantToken> constant(const std::string &constant);
ref<CharToken> constant(char character);
ref<IntegerToken> integer();
ref<AnyInToken> anyIn(char inferiorBound, char superiorBound);

// Helper constants
ref<CharToken> space();
ref<CharToken> tab();
ref<OrToken<CharToken, CharToken>> blank();
ref<OrToken<CharToken, CharToken>> lineTerminator();
ref<AnyInToken> digit();
ref<AnyInToken> letter();
ref<AnyOfToken> anyOf(char c);

template <typename TToken>
ref<WordToken<TToken>> word(ref<TToken> inner) {
    return ref<WordToken<TToken>>(new WordToken<TToken>(inner));
}

template <typename TToken>
ref<StartWithToken<TToken>> startWith(
    ref<TToken> inner) {
  return ref<StartWithToken<TToken>>(
      new StartWithToken<TToken>(inner));
}

template <typename TToken>
ref<EndWithToken<TToken>> endWith(
    ref<TToken> inner) {
  return ref<EndWithToken<TToken>>(
      new EndWithToken<TToken>(inner));
}

template <typename TLeft, typename TRight>
ref<RangeToken<TLeft, TRight>> range(ref<TLeft> left,
                                              ref<TRight> right) {
  return ref<RangeToken<TLeft, TRight>>(
      new RangeToken<TLeft, TRight>(left, right));
}


template <typename... TRest>
ref<AnyOfToken> anyOf(char c, TRest... rest) {
  ref<AnyOfToken> result(anyOf(rest...));
  result->validate(c);

  return result;
}
}
