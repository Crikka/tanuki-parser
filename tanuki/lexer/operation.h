#pragma once

#include <string>
#include <vector>
#include <utility>

#include "tanuki/misc/misc.h"
#include "tokens.h"

namespace tanuki {
namespace lexer {

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
template <typename, typename, typename>
class OrToken;
template <typename, typename, typename>
class AndToken;
template <typename, typename>
class RangeToken;

template <typename TToken, typename TReturn = typename TToken::TReturnType>
undirect_ref<NotToken<TToken, TReturn>> operator!(undirect_ref<TToken> token);
template <typename TLeft, typename TRight,
          typename TReturn = typename TLeft::TReturnType>
undirect_ref<OrToken<TLeft, TRight, TReturn>> operator||(ref<TLeft> left,
                                                         ref<TRight> right);
template <typename TLeft, typename TRight,
          typename TReturn = typename TLeft::TReturnType>
undirect_ref<AndToken<TLeft, TRight, TReturn>> operator&&(ref<TLeft> left,
                                                          ref<TRight> right);
template <typename TToken, typename TReturn = typename TToken::TReturnType>
undirect_ref<PlusToken<TToken, TReturn>> operator+(undirect_ref<TToken> token);
template <typename TToken, typename TReturn = typename TToken::TReturnType>
undirect_ref<StarToken<TToken, TReturn>> operator*(undirect_ref<TToken> token);
template <typename TToken, typename TReturn = typename TToken::TReturnType>
undirect_ref<OptionalToken<TToken, TReturn>> operator~(
    undirect_ref<TToken> token);
template <typename TToken>
undirect_ref<StartWithToken<TToken, typename TToken::TReturnType>> startWith(
    undirect_ref<TToken>);
template <typename TToken>
undirect_ref<EndWithToken<TToken, typename TToken::TReturnType>> endWith(
    undirect_ref<TToken>);
template <typename TLeft, typename TRight>
undirect_ref<RangeToken<TLeft, TRight>> range(undirect_ref<TLeft>,
                                              undirect_ref<TRight>);

// Operator
template <typename TToken, typename TReturn>
undirect_ref<NotToken<TToken, TReturn>> operator!(undirect_ref<TToken> token) {
  return undirect_ref<NotToken<TToken, TReturn>>(
      new NotToken<TToken, TReturn>(token));
}

template <typename TLeft, typename TRight, typename TReturn>
undirect_ref<OrToken<TLeft, TRight, TReturn>> operator||(ref<TLeft> left,
                                                         ref<TRight> right) {
  return undirect_ref<OrToken<TLeft, TRight, TReturn>>(
      new OrToken<TLeft, TRight, TReturn>(left, right));
}

template <typename TLeft, typename TRight, typename TReturn>
undirect_ref<AndToken<TLeft, TRight, TReturn>> operator&&(ref<TLeft> left,
                                                          ref<TRight> right) {
  return undirect_ref<AndToken<TLeft, TRight, TReturn>>(
      new AndToken<TLeft, TRight, TReturn>(left, right));
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
}
}
