#pragma once

#include <string>
#include <vector>
#include <utility>

#include "tanuki/misc/misc.h"
#include "tokens.h"

namespace tanuki {
// Unary
template <typename, typename>
class NotToken;
template <typename, typename>
class PlusToken;
template <typename, typename>
class StarToken;
template <typename, typename>
class OptionalToken;
template <typename, typename>
class StartWithToken;
template <typename, typename>
class EndWithToken;

// Binary
template <typename, typename>
class OrToken;
template <typename, typename>
class AndToken;
template <typename, typename>
class RangeToken;

template <typename TToken, typename TReturn = typename TToken::TReturnType>
ref<NotToken<TToken, TReturn>> operator!(ref<TToken> token);
template <typename TLeft, typename TRight>
ref<OrToken<TLeft, TRight>> operator||(ref<TLeft> left,
                                                         ref<TRight> right);
template <typename TLeft, typename TRight>
ref<AndToken<TLeft, TRight>> operator&&(ref<TLeft> left,
                                                          ref<TRight> right);
template <typename TToken, typename TReturn = typename TToken::TReturnType>
ref<PlusToken<TToken, TReturn>> operator+(ref<TToken> token);
template <typename TToken, typename TReturn = typename TToken::TReturnType>
ref<StarToken<TToken, TReturn>> operator*(ref<TToken> token);
template <typename TToken, typename TReturn = typename TToken::TReturnType>
ref<OptionalToken<TToken, TReturn>> operator~(
    ref<TToken> token);

// Operator
template <typename TToken, typename TReturn>
ref<NotToken<TToken, TReturn>> operator!(ref<TToken> token) {
  return ref<NotToken<TToken, TReturn>>(
      new NotToken<TToken, TReturn>(token));
}

template <typename TLeft, typename TRight>
ref<OrToken<TLeft, TRight>> operator||(ref<TLeft> left,
                                                         ref<TRight> right) {
  return ref<OrToken<TLeft, TRight>>(
      new OrToken<TLeft, TRight>(left, right));
}

template <typename TLeft, typename TRight>
ref<AndToken<TLeft, TRight>> operator&&(ref<TLeft> left,
                                                          ref<TRight> right) {
  return ref<AndToken<TLeft, TRight>>(
      new AndToken<TLeft, TRight>(left, right));
}

template <typename TToken, typename TReturn>
ref<PlusToken<TToken, TReturn>> operator+(ref<TToken> token) {
  return ref<PlusToken<TToken, TReturn>>(
      new PlusToken<TToken, TReturn>(token));
}

template <typename TToken, typename TReturn>
ref<StarToken<TToken, TReturn>> operator*(ref<TToken> token) {
  return ref<StarToken<TToken, TReturn>>(
      new StarToken<TToken, TReturn>(token));
}

template <typename TToken, typename TReturn>
ref<OptionalToken<TToken, TReturn>> operator~(
    ref<TToken> token) {
  return ref<OptionalToken<TToken, TReturn>>(
      new OptionalToken<TToken, TReturn>(token));
}
}
