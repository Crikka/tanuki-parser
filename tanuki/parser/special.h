#pragma once

#include "tanuki/misc/ref.h"
#include "tokens.h"

namespace tanuki {
undirect_ref<ConstantToken> constant(const std::string &constant);
undirect_ref<CharToken> constant(char character);
undirect_ref<IntegerToken> integer();
undirect_ref<AnyInToken> anyIn(char inferiorBound, char superiorBound);

// Helper constants
undirect_ref<CharToken> space();
undirect_ref<CharToken> tab();
undirect_ref<OrToken<CharToken, CharToken>> blank();
undirect_ref<OrToken<CharToken, CharToken>> lineTerminator();
undirect_ref<AnyInToken> digit();
undirect_ref<AnyInToken> letter();
undirect_ref<AnyOfToken> anyOf(char c);

template <typename TToken>
undirect_ref<WordToken<TToken>> word(undirect_ref<TToken> inner) {
    return undirect_ref<WordToken<TToken>>(new WordToken<TToken>(inner));
}

template <typename TToken>
undirect_ref<StartWithToken<TToken, typename TToken::TReturnType>> startWith(
    undirect_ref<TToken> inner) {
  return undirect_ref<StartWithToken<TToken, typename TToken::TReturnType>>(
      new StartWithToken<TToken, typename TToken::TReturnType>(inner));
}

template <typename TToken>
undirect_ref<EndWithToken<TToken, typename TToken::TReturnType>> endWith(
    undirect_ref<TToken> inner) {
  return undirect_ref<EndWithToken<TToken, typename TToken::TReturnType>>(
      new EndWithToken<TToken, typename TToken::TReturnType>(inner));
}

template <typename TLeft, typename TRight>
undirect_ref<RangeToken<TLeft, TRight>> range(undirect_ref<TLeft> left,
                                              undirect_ref<TRight> right) {
  return undirect_ref<RangeToken<TLeft, TRight>>(
      new RangeToken<TLeft, TRight>(left, right));
}


template <typename... TRest>
undirect_ref<AnyOfToken> anyOf(char c, TRest... rest) {
  undirect_ref<AnyOfToken> result(anyOf(rest...));
  result->validate(c);

  return result;
}
}
