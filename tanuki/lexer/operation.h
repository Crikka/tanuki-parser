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

// Binary
template <typename, typename, typename>
class OrToken;
template <typename, typename, typename>
class AndToken;
template <typename, typename, typename>
class ContinuousToken;

template <typename TToken, typename TReturn = typename TToken::TReturnValue>
ref<NotToken<TToken, TReturn>> operator!(ref<TToken> token);
template <typename TLeft, typename TRight,
          typename TReturn = typename TLeft::TReturnValue>
ref<OrToken<TLeft, TRight, TReturn>> operator||(ref<TLeft> left,
                                                ref<TRight> right);
template <typename TLeft, typename TRight,
          typename TReturn = typename TLeft::TReturnValue>
ref<AndToken<TLeft, TRight, TReturn>> operator&&(ref<TLeft> left,
                                                 ref<TRight> right);
template <typename THead, typename TTail,
          typename TReturn = typename THead::TReturnValue>
ref<ContinuousToken<THead, TTail, TReturn>> operator, (ref<THead> head,
                                                       ref<TTail> tail);
template <typename TToken, typename TReturn = typename TToken::TReturnValue>
ref<PlusToken<TToken, TReturn>> operator+(ref<TToken> token);
template <typename TToken, typename TReturn = typename TToken::TReturnValue>
ref<StarToken<TToken, TReturn>> operator*(ref<TToken> token);
template <typename TToken, typename TReturn = typename TToken::TReturnValue>
ref<OptionalToken<TToken, TReturn>> operator~(ref<TToken> token);


// Operator
template <typename TToken, typename TReturn>
ref<NotToken<TToken, TReturn>> operator!(ref<TToken> token) {
  return ref<NotToken<TToken, TReturn>>(new NotToken<TToken, TReturn>(token));
}

template <typename TLeft, typename TRight, typename TReturn>
ref<OrToken<TLeft, TRight, TReturn>> operator||(ref<TLeft> left,
                                                ref<TRight> right) {
  return ref<OrToken<TLeft, TRight, TReturn>>(
      new OrToken<TLeft, TRight, TReturn>(left, right));
}

template <typename TLeft, typename TRight, typename TReturn>
ref<AndToken<TLeft, TRight, TReturn>> operator&&(ref<TLeft> left,
                                                 ref<TRight> right) {
  return ref<AndToken<TLeft, TRight, TReturn>>(
      new AndToken<TLeft, TRight, TReturn>(left, right));
}

template <typename THead, typename TTail, typename TReturn>
ref<ContinuousToken<THead, TTail, TReturn>> operator, (ref<THead> head,
                                                       ref<TTail> tail) {
  return ref<ContinuousToken<THead, TTail, TReturn>>(
      new ContinuousToken<THead, TTail, TReturn>(head, tail));
}

template <typename TToken, typename TReturn>
ref<PlusToken<TToken, TReturn>> operator+(ref<TToken> token) {
  return ref<PlusToken<TToken, TReturn>>(new PlusToken<TToken, TReturn>(token));
}

template <typename TToken, typename TReturn>
ref<StarToken<TToken, TReturn>> operator*(ref<TToken> token) {
  return ref<StarToken<TToken, TReturn>>(new StarToken<TToken, TReturn>(token));
}

template <typename TToken, typename TReturn>
ref<OptionalToken<TToken, TReturn>> operator~(ref<TToken> token) {
  return ref<OptionalToken<TToken, TReturn>>(new OptionalToken<TToken, TReturn>(token));
}
}
}
