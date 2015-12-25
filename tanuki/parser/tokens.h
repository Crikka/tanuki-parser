#pragma once

#include <string>
#include <vector>
#include <stack>
#include <utility>
#include <array>

#include <climits>

#include "tanuki/misc/misc.h"

namespace tanuki {
// Root
template <typename>
class Token;

// Simple
class ConstantToken;
class CharToken;
class IntegerToken;
class AnyOfToken;
class AnyInToken;

// Unary
template <typename, typename>
class UnaryToken;
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
template <typename, std::size_t>
class RepeatableToken;

// Binary
template <typename, typename, typename>
class BinaryToken;
template <typename, typename>
class OrToken;
template <typename, typename>
class AndToken;
template <typename, typename>
class RangeToken;

// Special
template <typename>
class WordToken;

// ~~~~~~~~ Helper

/**
 * @brief The Token class is the root class of Tokens.
 */
template <typename TReturn>
class Token {
 public:
  virtual ref<TReturn> match(const tanuki::String &in) = 0;
  virtual Piece<TReturn> consume(const tanuki::String &in) = 0;
  virtual int exactSize() { return -1; }
  virtual int biggestSize() { return -1; }

  typedef TReturn TReturnType;
};

/**
 * @brief The ConstantToken class represents a string.
 */
class ConstantToken : public Token<std::string> {
 public:
  explicit ConstantToken(const std::string &constant);
  ref<std::string> match(const tanuki::String &in) override;
  Piece<std::string> consume(const tanuki::String &in) override;
  int exactSize() override { return m_constant.size(); }

 private:
  std::string m_constant;
};

/**
 * @brief The CharToken class represents a char.
 */
class CharToken : public Token<char> {
 public:
  explicit CharToken(char character);
  ref<char> match(const tanuki::String &in) override;
  Piece<char> consume(const tanuki::String &in) override;
  int exactSize() override { return 1; }

 private:
  char m_character;
};

/**
 * @brief The IntegerToken class represents an integer.
 */
class IntegerToken : public Token<int> {
 public:
  explicit IntegerToken();
  ref<int> match(const tanuki::String &in) override;
  Piece<int> consume(const tanuki::String &in) override;

 private:
  int m_result;
};

class AnyOfToken : public Token<char> {
 public:
  explicit AnyOfToken(std::vector<char> initial);
  explicit AnyOfToken();
  void validate(char character);
  ref<char> match(const tanuki::String &in) override;
  Piece<char> consume(const tanuki::String &in) override;

 private:
  bool m_intern[CHAR_MAX];
};

class AnyInToken : public Token<char> {
 public:
  explicit AnyInToken(char inferiorBound, char superiorBound);
  ref<char> match(const tanuki::String &in) override;
  Piece<char> consume(const tanuki::String &in) override;

 private:
  char m_inferiorBound;
  char m_superiorBound;
};

/**
 * @brief The UnaryToken class represents a token with a inner token.
 */
template <typename TToken, typename TReturn>
class UnaryToken : public Token<TReturn> {
 public:
  explicit UnaryToken(ref<TToken> token);

 protected:
  ref<TToken> token() { return m_token; }

 private:
  ref<TToken> m_token;
};

/**
 * @brief The NotToken class represents a token returning false on not inner
 * token.
 */
template <typename TToken>
class NotToken : public UnaryToken<TToken, std::string> {
 public:
  explicit NotToken(ref<TToken> token);
  ref<std::string> match(const tanuki::String &in) override;
  Piece<std::string> consume(const tanuki::String &in) override;
};

/**
 * @brief The PlusToken class represents a token repeting a true at least once
 * on not inner token.
 */
template <typename TToken>
class PlusToken
    : public UnaryToken<TToken,
                        std::vector<ref<typename TToken::TReturnType>>> {
 public:
  explicit PlusToken(ref<TToken> token);
  ref<std::vector<ref<typename TToken::TReturnType>>> match(
      const tanuki::String &in) override;
  Piece<std::vector<ref<typename TToken::TReturnType>>> consume(
      const tanuki::String &in) override;
};

/**
 * @brief The StarToken class represents a token repeting a true or optional on
 * not inner token.
 */
template <typename TToken>
class StarToken
    : public UnaryToken<
          TToken,
          Optional<ref<std::vector<ref<typename TToken::TReturnType>>>>> {
 public:
  explicit StarToken(ref<TToken> token);
  ref<Optional<ref<std::vector<ref<typename TToken::TReturnType>>>>> match(
      const tanuki::String &in) override;
  Piece<Optional<ref<std::vector<ref<typename TToken::TReturnType>>>>> consume(
      const tanuki::String &in) override;

 private:
  ref<OptionalToken<PlusToken<TToken>>> m_inner;
};

/**
 * @brief The OptionalToken class
 */
template <typename TToken>
class OptionalToken
    : public UnaryToken<TToken, Optional<ref<typename TToken::TReturnType>>> {
 public:
  explicit OptionalToken(ref<TToken> inner);
  ref<Optional<ref<typename TToken::TReturnType>>> match(
      const tanuki::String &in) override;
  Piece<Optional<ref<typename TToken::TReturnType>>> consume(
      const tanuki::String &in) override;
};

/**
 * @brief The StartWith class
 */
template <typename TToken>
class StartWithToken : public UnaryToken<TToken, typename TToken::TReturnType> {
 public:
  explicit StartWithToken(ref<TToken> inner);
  ref<typename TToken::TReturnType> match(const tanuki::String &in) override;
  Piece<typename TToken::TReturnType> consume(
      const tanuki::String &in) override;
};

/**
 * @brief The EndWith class
 */
template <typename TToken>
class EndWithToken : public UnaryToken<TToken, typename TToken::TReturnType> {
 public:
  explicit EndWithToken(ref<TToken> inner);
  ref<typename TToken::TReturnType> match(const tanuki::String &in) override;
  Piece<typename TToken::TReturnType> consume(
      const tanuki::String &in) override;
};

/**
 * @brief The RepeatableToken class
 */
template <typename TToken, std::size_t size>
class RepeatableToken
    : public UnaryToken<TToken,
                        std::array<typename TToken::TReturnType, size>> {
 public:
  explicit RepeatableToken(ref<TToken> inner);
  ref<std::array<typename TToken::TReturnType, size>> match(
      const tanuki::String &in) override;
  Piece<std::array<typename TToken::TReturnType, size>> consume(
      const tanuki::String &in) override;
};

/**
 * @brief The BinaryToken class
 */
template <typename TLeft, typename TRight, typename TReturn>
class BinaryToken : public Token<TReturn> {
 public:
  explicit BinaryToken(ref<TLeft> left, ref<TRight> right);

 protected:
  ref<TLeft> left() { return m_left; }
  ref<TRight> right() { return m_right; }

 private:
  ref<TLeft> m_left;
  ref<TRight> m_right;
};

/**
 * @brief The OrToken class
 */
template <typename TLeft, typename TRight>
class OrToken : public BinaryToken<TLeft, TRight, std::string> {
 public:
  explicit OrToken(ref<TLeft> left, ref<TRight> right);
  ref<std::string> match(const tanuki::String &in) override;
  Piece<std::string> consume(const tanuki::String &in) override;
};

/**
 * @brief The AndToken class
 */
template <typename TLeft, typename TRight>
class AndToken : public BinaryToken<TLeft, TRight, std::string> {
 public:
  explicit AndToken(ref<TLeft> left, ref<TRight> right);
  ref<std::string> match(const tanuki::String &in) override;
  Piece<std::string> consume(const tanuki::String &in) override;
};

/**
 * @brief The RangeToken class
 */
template <typename TLeft, typename TRight>
class RangeToken : public Token<std::string> {
 public:
  explicit RangeToken(ref<TLeft> left, ref<TRight> right);
  ref<std::string> match(const tanuki::String &in) override;
  Piece<std::string> consume(const tanuki::String &in) override;

 private:
  ref<StartWithToken<TLeft>> m_left;
  ref<EndWithToken<TRight>> m_right;
};

template <typename TToken>
class WordToken : public Token<std::string> {
 public:
  explicit WordToken(ref<TToken> inner);
  ref<std::string> match(const tanuki::String &in) override;
  Piece<std::string> consume(const tanuki::String &in) override;

 private:
  ref<PlusToken<TToken>> m_inner;
};

// ------ Method --------
// Unary
template <typename TToken, typename TReturn>
UnaryToken<TToken, TReturn>::UnaryToken(ref<TToken> token)
    : Token<TReturn>(), m_token(token) {}

template <typename TToken>
NotToken<TToken>::NotToken(ref<TToken> token)
    : UnaryToken<TToken, std::string>(token) {}

template <typename TToken>
ref<std::string> NotToken<TToken>::match(const tanuki::String &in) {
  if (UnaryToken<TToken, std::string>::token()->match(in)) {
    return ref<std::string>();
  } else {
    return ref<std::string>(new std::string(in.toStdString()));
  }
}

template <typename TToken>
Piece<std::string> NotToken<TToken>::consume(const tanuki::String &in) {
  Piece<typename TToken::TReturnType> result(
      UnaryToken<TToken, std::string>::token()->consume(in));

  if (result.result) {
    return Piece<std::string>{
        result.length, ref<std::string>(new std::string(in.toStdString()))};
  } else {
    return Piece<std::string>{0, ref<std::string>()};
  }
}

template <typename TToken>
PlusToken<TToken>::PlusToken(ref<TToken> token)
    : UnaryToken<TToken, std::vector<ref<typename TToken::TReturnType>>>(
          token) {}

template <typename TToken>
ref<std::vector<ref<typename TToken::TReturnType>>> PlusToken<TToken>::match(
    const tanuki::String &in) {
  if (in.empty()) {
    return ref<std::vector<ref<typename TToken::TReturnType>>>();
  }

  bool res = true;
  unsigned int current = 0;
  unsigned int length = in.size();

  ref<std::vector<ref<typename TToken::TReturnType>>> result(
      new std::vector<ref<typename TToken::TReturnType>>());

  while (current < length) {
    Piece<typename TToken::TReturnType> currentRes =
        UnaryToken<TToken,
                   std::vector<ref<typename TToken::TReturnType>>>::token()
            ->consume(in.substr(current));

    if (currentRes.result) {
      current += currentRes.length;
      result->push_back(currentRes.result);
    } else {
      res = false;
      break;
    }
  }

  if (res) {
    return result;
  } else {
    return ref<std::vector<ref<typename TToken::TReturnType>>>();
  }
}

template <typename TToken>
Piece<std::vector<ref<typename TToken::TReturnType>>>
PlusToken<TToken>::consume(const tanuki::String &in) {
  if (in.empty()) {
    return Piece<std::vector<ref<typename TToken::TReturnType>>>{
        0, ref<std::vector<ref<typename TToken::TReturnType>>>()};
  }

  uint32_t current = 0;
  uint32_t length = in.size();

  ref<std::vector<ref<typename TToken::TReturnType>>> result(
      new std::vector<ref<typename TToken::TReturnType>>());

  while (current < length) {
    Piece<typename TToken::TReturnType> currentRes =
        UnaryToken<TToken,
                   std::vector<ref<typename TToken::TReturnType>>>::token()
            ->consume(in.substr(current));

    if (currentRes.result) {
      current += currentRes.length;
      result->push_back(currentRes.result);
    } else {
      break;
    }
  }

  if (result->empty()) {
    return Piece<std::vector<ref<typename TToken::TReturnType>>>{
        0, ref<std::vector<ref<typename TToken::TReturnType>>>()};
  } else {
    return Piece<std::vector<ref<typename TToken::TReturnType>>>{current,
                                                                 result};
  }
}

template <typename TToken>
StarToken<TToken>::StarToken(ref<TToken> token)
    : UnaryToken<TToken,
                 Optional<ref<std::vector<ref<typename TToken::TReturnType>>>>>(
          token),
      m_inner(~ + token) {}

template <typename TToken>
ref<Optional<ref<std::vector<ref<typename TToken::TReturnType>>>>>
StarToken<TToken>::match(const tanuki::String &in) {
  return m_inner->match(in);
}

template <typename TToken>
Piece<Optional<ref<std::vector<ref<typename TToken::TReturnType>>>>>
StarToken<TToken>::consume(const tanuki::String &in) {
  return m_inner->consume(in);
}

template <typename TToken>
OptionalToken<TToken>::OptionalToken(ref<TToken> token)
    : UnaryToken<TToken, Optional<ref<typename TToken::TReturnType>>>(token) {}

template <typename TToken>
ref<Optional<ref<typename TToken::TReturnType>>> OptionalToken<TToken>::match(
    const tanuki::String &in) {
  ref<Optional<ref<typename TToken::TReturnType>>> result(
      new Optional<ref<typename TToken::TReturnType>>());
  ref<typename TToken::TReturnType> subresult = this->token()->match(in);

  if (subresult) {
    result->token = subresult;
  }

  return result;
}

template <typename TToken>
Piece<Optional<ref<typename TToken::TReturnType>>>
OptionalToken<TToken>::consume(const tanuki::String &in) {
  Piece<Optional<ref<typename TToken::TReturnType>>> result{
      0, new Optional<ref<typename TToken::TReturnType>>()};
  Piece<typename TToken::TReturnType> subresult = this->token()->consume(in);

  if (subresult) {
    result.length = subresult.length;
    result.result->token = subresult.result;
  }

  return result;
}

template <typename TToken>
StartWithToken<TToken>::StartWithToken(ref<TToken> token)
    : UnaryToken<TToken, typename TToken::TReturnType>(token) {}

template <typename TToken>
ref<typename TToken::TReturnType> StartWithToken<TToken>::match(
    const tanuki::String &in) {
  Piece<typename TToken::TReturnType> result =
      UnaryToken<TToken, typename TToken::TReturnType>::token()->consume(in);

  if (result.result) {
    return result.result;
  } else {
    return ref<typename TToken::TReturnType>();
  }
}

template <typename TToken>
Piece<typename TToken::TReturnType> StartWithToken<TToken>::consume(
    const tanuki::String &in) {
  Piece<typename TToken::TReturnType> result =
      UnaryToken<TToken, typename TToken::TReturnType>::token()->consume(in);

  if (result.result) {
    return result;
  } else {
    return Piece<typename TToken::TReturnType>{
        0, ref<typename TToken::TReturnType>()};
  }
}

template <typename TToken>
EndWithToken<TToken>::EndWithToken(ref<TToken> token)
    : UnaryToken<TToken, typename TToken::TReturnType>(token) {}

template <typename TToken>
ref<typename TToken::TReturnType> EndWithToken<TToken>::match(
    const tanuki::String &in) {
  if (in.empty()) {
    return ref<typename TToken::TReturnType>();
  }

  int exactSize =
      UnaryToken<TToken, typename TToken::TReturnType>::token().exactSize();
  int length = in.size();

  if (exactSize == -1) {
    ref<typename TToken::TReturnType> result;

    int biggestSize =
        UnaryToken<TToken, typename TToken::TReturnType>::token().biggestSize();
    int minimum;

    if (biggestSize == -1) {
      minimum = 0;
    } else {
      minimum = (length - biggestSize);

      if (minimum < 0) {
        minimum = 0;
      }
    }

    do {
      length--;

      result = UnaryToken<TToken, typename TToken::TReturnType>::token()->match(
          in.substr(length));

      if (result) {
        break;
      }

    } while (length > minimum);

    return result;
  } else {
    int delta = (in.size() - exactSize);

    if (delta < 0) {
      return ref<typename TToken::TReturnType>();
    } else {
      ref<typename TToken::TReturnType> result =
          (UnaryToken<TToken, typename TToken::TReturnType>::token()->match(
              in.substr(delta)));

      if (result) {
        return result;
      } else {
        return ref<typename TToken::TReturnType>();
      }
    }
  }
}

template <typename TToken>
Piece<typename TToken::TReturnType> EndWithToken<TToken>::consume(
    const tanuki::String &in) {
  int length = in.size();
  Piece<typename TToken::TReturnType> result;

  for (int i = 0; i < length; i++) {
    result =
        (UnaryToken<TToken, typename TToken::TReturnType>::token()->consume(
            in.substr(i)));

    if (result.result) {
      result.length += i;  // We need to keep size of consume
      break;
    }
  }

  return result;
}

template <typename TToken, std::size_t size>
RepeatableToken<TToken, size>::RepeatableToken(ref<TToken> token)
    : UnaryToken<TToken, std::array<typename TToken::TReturnType, size>>(
          token) {}

template <typename TToken, std::size_t size>
ref<std::array<typename TToken::TReturnType, size>>
RepeatableToken<TToken, size>::match(const tanuki::String &in) {
  std::array<typename TToken::TReturnType, size> result;

  std::size_t index = 0;
  tanuki::String current = in;

  while (index < size) {
    Piece<typename TToken::TReturnType> buffer =
        UnaryToken<TToken,
                   std::array<typename TToken::TReturnType, size>>::token()
            ->consume(current);

    if (buffer.result) {
      result[index] = buffer.result;
      current = current.substr(buffer.length);

      index++;
    } else {
      break;
    }
  }

  if (index == size && current.empty()) {
    return ref<std::array<typename TToken::TReturnType, size>>(
        new std::array<typename TToken::TReturnType, size>(result));
  } else {
    return ref<std::array<typename TToken::TReturnType, size>>();
  }
}

template <typename TToken, std::size_t size>
Piece<std::array<typename TToken::TReturnType, size>>
RepeatableToken<TToken, size>::consume(const tanuki::String &in) {
  std::array<typename TToken::TReturnType, size> result;

  std::size_t index = 0;
  uint32_t matchSize = 0;
  tanuki::String current = in;

  while (index < size) {
    Piece<typename TToken::TReturnType> buffer =
        UnaryToken<TToken,
                   std::array<typename TToken::TReturnType, size>>::token()
            ->consume(current);

    if (buffer.result) {
      result[index] = buffer.result;
      current = current.substr(buffer.length);
      matchSize += buffer.length;

      index++;
    } else {
      break;
    }
  }

  if (index == size && current.empty()) {
    return Piece<std::array<typename TToken::TReturnType, size>>{
        matchSize,
        ref<std::array<typename TToken::TReturnType, size>>(
            new std::array<typename TToken::TReturnType, size>(result))};
  } else {
    return Piece<std::array<typename TToken::TReturnType, size>>{
        0, ref<std::array<typename TToken::TReturnType, size>>()};
  }
}

// Binary
template <typename TLeft, typename TRight, typename TReturn>
BinaryToken<TLeft, TRight, TReturn>::BinaryToken(ref<TLeft> left,
                                                 ref<TRight> right)
    : Token<TReturn>(), m_left(left), m_right(right) {}

template <typename TLeft, typename TRight>
OrToken<TLeft, TRight>::OrToken(ref<TLeft> left, ref<TRight> right)
    : BinaryToken<TLeft, TRight, std::string>(left, right) {}

template <typename TLeft, typename TRight>
ref<std::string> OrToken<TLeft, TRight>::match(const tanuki::String &in) {
  ref<typename TLeft::TReturnType> leftResult =
      (BinaryToken<TLeft, TRight, std::string>::left()->match(in));

  if (leftResult) {
    return ref<std::string>(new std::string(in.toStdString()));
  } else {
    ref<typename TRight::TReturnType> rightResult =
        (BinaryToken<TLeft, TRight, std::string>::right()->match(in));

    if (rightResult) {
      return ref<std::string>(new std::string(in.toStdString()));
    } else {
      return ref<std::string>();
    }
  }
}

template <typename TLeft, typename TRight>
Piece<std::string> OrToken<TLeft, TRight>::consume(const tanuki::String &in) {
  Piece<typename TLeft::TReturnType> leftResult =
      (BinaryToken<TLeft, TRight, std::string>::left()->consume(in));

  if (leftResult.result) {
    return Piece<std::string>{
        leftResult.length, ref<std::string>(new std::string(
                               in.substr(0, leftResult.length).toStdString()))};
  } else {
    Piece<typename TRight::TReturnType> rightResult =
        (BinaryToken<TLeft, TRight, std::string>::right()->consume(in));

    if (rightResult.result) {
      return Piece<std::string>{
          rightResult.length,
          ref<std::string>(
              new std::string(in.substr(0, rightResult.length).toStdString()))};
    } else {
      return Piece<std::string>{0, ref<std::string>()};
    }
  }
}

template <typename TLeft, typename TRight>
AndToken<TLeft, TRight>::AndToken(ref<TLeft> left, ref<TRight> right)
    : BinaryToken<TLeft, TRight, std::string>(left, right) {}

template <typename TLeft, typename TRight>
ref<std::string> AndToken<TLeft, TRight>::match(const tanuki::String &in) {
  ref<typename TLeft::TReturnType> leftResult =
      BinaryToken<TLeft, TRight, std::string>::left()->match(in);

  if (leftResult) {
    ref<typename TRight::TReturnType> rightResult =
        BinaryToken<TLeft, TRight, std::string>::right()->match(in);

    if (rightResult) {
      return ref<std::string>(new std::string(in.toStdString()));
    } else {
      return ref<std::string>();
    }
  } else {
    return ref<std::string>();
  }
}

template <typename TLeft, typename TRight>
Piece<std::string> AndToken<TLeft, TRight>::consume(const tanuki::String &in) {
  Piece<typename TLeft::TReturnType> leftResult =
      (BinaryToken<TLeft, TRight, std::string>::left()->consume(in));

  if (leftResult.result) {
    Piece<typename TRight::TReturnType> rightResult =
        (BinaryToken<TLeft, TRight, std::string>::right()->consume(in));

    if (rightResult.result) {
      if (leftResult.length == rightResult.length) {
        return Piece<std::string>{
            leftResult.length,
            new std::string(in.substr(0, rightResult.length).toStdString())};
      } else {
        return Piece<std::string>{0, ref<std::string>()};
      }
    } else {
      return Piece<std::string>{0, ref<std::string>()};
    }
  } else {
    return Piece<std::string>{0, ref<std::string>()};
  }
}

template <typename TLeft, typename TRight>
RangeToken<TLeft, TRight>::RangeToken(ref<TLeft> left, ref<TRight> right)
    : Token<std::string>(), m_left(startWith(left)), m_right(endWith(right)) {}

template <typename TLeft, typename TRight>
ref<std::string> RangeToken<TLeft, TRight>::match(const tanuki::String &in) {
  ref<std::string> result;

  if (m_left->match(in)) {
    if (m_right->match(in)) {
      result = ref<std::string>(new std::string(in.toStdString()));
    }
  }

  return result;
}

template <typename TLeft, typename TRight>
Piece<std::string> RangeToken<TLeft, TRight>::consume(
    const tanuki::String &in) {
  Piece<std::string> result;

  if (m_left->consume(in).result) {
    auto right = m_right->consume(in);

    if (right.result) {
      result = Piece<std::string>{
          right.length, ref<std::string>(new std::string(
                            in.substr(0, right.length).toStdString()))};
    }
  }

  return result;
}

template <typename TToken>
WordToken<TToken>::WordToken(ref<TToken> inner)
    : Token<std::string>(), m_inner(+inner) {}

template <typename TToken>
ref<std::string> WordToken<TToken>::match(const tanuki::String &in) {
  ref<std::vector<ref<typename TToken::TReturnType>>> result(
      m_inner->match(in));

  if (result) {
    return ref<std::string>(new std::string(in.toStdString()));
  } else {
    return ref<std::string>();
  }
}

template <typename TToken>
Piece<std::string> WordToken<TToken>::consume(const tanuki::String &in) {
  Piece<std::vector<ref<typename TToken::TReturnType>>> result(
      m_inner->consume(in));

  if (result.result) {
    int length = result.length;

    return Piece<std::string>{
        result.length,
        ref<std::string>(new std::string(in.substr(0, length).toStdString()))};
  } else {
    return Piece<std::string>{0, ref<std::string>()};
  }
}
}
