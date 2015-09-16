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
undirect_ref<NotToken<TToken, TReturn>> operator!(undirect_ref<TToken> token);
template <typename TLeft, typename TRight>
undirect_ref<OrToken<TLeft, TRight>> operator||(ref<TLeft> left,
                                                         ref<TRight> right);
template <typename TLeft, typename TRight>
undirect_ref<AndToken<TLeft, TRight>> operator&&(ref<TLeft> left,
                                                          ref<TRight> right);
template <typename TToken, typename TReturn = typename TToken::TReturnType>
undirect_ref<PlusToken<TToken, TReturn>> operator+(undirect_ref<TToken> token);
template <typename TToken, typename TReturn = typename TToken::TReturnType>
undirect_ref<StarToken<TToken, TReturn>> operator*(undirect_ref<TToken> token);
template <typename TToken, typename TReturn = typename TToken::TReturnType>
undirect_ref<OptionalToken<TToken, TReturn>> operator~(
    undirect_ref<TToken> token);

// Operator
template <typename TToken, typename TReturn>
undirect_ref<NotToken<TToken, TReturn>> operator!(undirect_ref<TToken> token) {
  return undirect_ref<NotToken<TToken, TReturn>>(
      new NotToken<TToken, TReturn>(token));
}

template <typename TLeft, typename TRight>
undirect_ref<OrToken<TLeft, TRight>> operator||(ref<TLeft> left,
                                                         ref<TRight> right) {
  return undirect_ref<OrToken<TLeft, TRight>>(
      new OrToken<TLeft, TRight>(left, right));
}

template <typename TLeft, typename TRight>
undirect_ref<AndToken<TLeft, TRight>> operator&&(ref<TLeft> left,
                                                          ref<TRight> right) {
  return undirect_ref<AndToken<TLeft, TRight>>(
      new AndToken<TLeft, TRight>(left, right));
}

template <typename TToken, typename TReturn>
undirect_ref<PlusToken<TToken, TReturn>> operator+(undirect_ref<TToken> token) {
  return undirect_ref<PlusToken<TToken, TReturn>>(
      new PlusToken<TToken, TReturn>(token));
}

template <typename TToken, typename TReturn>
undirect_ref<StarToken<TToken, TReturn>> operator*(undirect_ref<TToken> token) {
  return undirect_ref<StarToken<TToken, TReturn>>(
      new StarToken<TToken, TReturn>(token));
}

template <typename TToken, typename TReturn>
undirect_ref<OptionalToken<TToken, TReturn>> operator~(
    undirect_ref<TToken> token) {
  return undirect_ref<OptionalToken<TToken, TReturn>>(
      new OptionalToken<TToken, TReturn>(token));
}
}
