#pragma once

#include <string>
#include <vector>
#include <utility>

#include "tanuki/misc/misc.h"
#include "tokens.h"

namespace tanuki {
// Unary
template <typename>
class NotToken;
template <typename>
class PlusToken;
template <typename>
class StarToken;
template <typename>
class OptionalToken;
template <typename>
class StartWithToken;
template <typename>
class EndWithToken;

// Binary
template <typename, typename>
class OrToken;
template <typename, typename>
class AndToken;
template <typename, typename>
class RangeToken;

template <typename TToken>
ref<NotToken<TToken>> operator!(ref<TToken> token);
template <typename TLeft, typename TRight>
ref<OrToken<TLeft, TRight>> operator||(ref<TLeft> left, ref<TRight> right);
template <typename TLeft, typename TRight>
ref<AndToken<TLeft, TRight>> operator&&(ref<TLeft> left, ref<TRight> right);
template <typename TToken>
ref<PlusToken<TToken>> operator+(ref<TToken> token);
template <typename TToken>
ref<StarToken<TToken>> operator*(ref<TToken> token);
template <typename TToken>
ref<OptionalToken<TToken>> operator~(ref<TToken> token);

// Operator
template <typename TToken>
ref<NotToken<TToken>> operator!(ref<TToken> token) {
  return ref<NotToken<TToken>>(new NotToken<TToken>(token));
}

template <typename TLeft, typename TRight>
ref<OrToken<TLeft, TRight>> operator||(ref<TLeft> left, ref<TRight> right) {
  return ref<OrToken<TLeft, TRight>>(new OrToken<TLeft, TRight>(left, right));
}

template <typename TLeft, typename TRight>
ref<AndToken<TLeft, TRight>> operator&&(ref<TLeft> left, ref<TRight> right) {
  return ref<AndToken<TLeft, TRight>>(new AndToken<TLeft, TRight>(left, right));
}

template <typename TToken>
ref<PlusToken<TToken>> operator+(ref<TToken> token) {
  return ref<PlusToken<TToken>>(new PlusToken<TToken>(token));
}

template <typename TToken>
ref<StarToken<TToken>> operator*(ref<TToken> token) {
  return ref<StarToken<TToken>>(new StarToken<TToken>(token));
}

template <typename TToken>
ref<OptionalToken<TToken>> operator~(ref<TToken> token) {
  return ref<OptionalToken<TToken>>(new OptionalToken<TToken>(token));
}
}
