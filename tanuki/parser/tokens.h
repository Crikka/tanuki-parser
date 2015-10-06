#pragma once

#include <string>
#include <vector>
#include <stack>
#include <utility>

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
  virtual Collect<TReturn> collect(const tanuki::String &in) = 0;
  virtual bool greedy() { return true; }
  virtual bool stopAtFirstGreedyFail() { return true; }
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
  Collect<std::string> collect(const tanuki::String &in) override;
  bool greedy() override { return false; }
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
  Collect<char> collect(const tanuki::String &in) override;
  bool greedy() override { return false; }
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
  Collect<int> collect(const tanuki::String &in) override;

 private:
  int m_result;
};

class AnyOfToken : public Token<char> {
 public:
  explicit AnyOfToken(std::vector<char> initial);
  explicit AnyOfToken();
  void validate(char character);
  ref<char> match(const tanuki::String &in) override;
  Collect<char> collect(const tanuki::String &in) override;

 private:
  bool m_intern[CHAR_MAX];
};

class AnyInToken : public Token<char> {
 public:
  explicit AnyInToken(char inferiorBound, char superiorBound);
  ref<char> match(const tanuki::String &in) override;
  Collect<char> collect(const tanuki::String &in) override;

 private:
  char m_inferiorBound;
  char m_superiorBound;
};

/**
 * @brief The UnaryToken class represents a token with a inner token.
 */
template <typename TToken, typename TReturn = typename TToken::TReturnType>
class UnaryToken : public Token<TReturn> {
 public:
  explicit UnaryToken(undirect_ref<TToken> token);

 protected:
  undirect_ref<TToken> token() { return m_token; }

 private:
  undirect_ref<TToken> m_token;
};

/**
 * @brief The NotToken class represents a token returning false on not inner
 * token.
 */
template <typename TToken, typename TReturn = typename TToken::TReturnType>
class NotToken : public UnaryToken<TToken, std::string> {
 public:
  explicit NotToken(undirect_ref<TToken> token);
  ref<std::string> match(const tanuki::String &in) override;
  Collect<std::string> collect(const tanuki::String &in) override;
};

/**
 * @brief The PlusToken class represents a token repeting a true at least once
 * on not inner token.
 */
template <typename TToken, typename TReturn = typename TToken::TReturnType>
class PlusToken : public UnaryToken<TToken, std::vector<ref<TReturn>>> {
 public:
  explicit PlusToken(undirect_ref<TToken> token);
  ref<std::vector<ref<TReturn>>> match(const tanuki::String &in) override;
  Collect<std::vector<ref<TReturn>>> collect(const tanuki::String &in) override;
  bool stopAtFirstGreedyFail() override { return false; }
};

/**
 * @brief The StarToken class represents a token repeting a true or optional on
 * not inner token.
 */
template <typename TToken, typename TReturn = typename TToken::TReturnType>
class StarToken : public UnaryToken<TToken, std::vector<ref<TReturn>>> {
 public:
  explicit StarToken(undirect_ref<TToken> token);
  ref<std::vector<ref<TReturn>>> match(const tanuki::String &in) override;
  Collect<std::vector<ref<TReturn>>> collect(const tanuki::String &in) override;
  bool stopAtFirstGreedyFail() override { return false; }

 private:
  ref<PlusToken<TToken, TReturn>> m_inner;
};

/**
 * @brief The OptionalToken class
 */
template <typename TToken, typename TReturn = typename TToken::TReturnType>
class OptionalToken : public UnaryToken<TToken, std::vector<ref<TReturn>>> {
 public:
  explicit OptionalToken(undirect_ref<TToken> inner);
  ref<std::vector<ref<TReturn>>> match(const tanuki::String &in) override;
  Collect<std::vector<ref<TReturn>>> collect(const tanuki::String &in) override;
};

/**
 * @brief The StartWith class
 */
template <typename TToken, typename TReturn = typename TToken::TReturnType>
class StartWithToken : public UnaryToken<TToken, TReturn> {
 public:
  explicit StartWithToken(undirect_ref<TToken> inner);
  ref<TReturn> match(const tanuki::String &in) override;
  Collect<TReturn> collect(const tanuki::String &in) override;
};

/**
 * @brief The EndWith class
 */
template <typename TToken, typename TReturn = typename TToken::TReturnType>
class EndWithToken : public UnaryToken<TToken, TReturn> {
 public:
  explicit EndWithToken(undirect_ref<TToken> inner);
  ref<TReturn> match(const tanuki::String &in) override;
  Collect<TReturn> collect(const tanuki::String &in) override;
};

/**
 * @brief The BinaryToken class
 */
template <typename TLeft, typename TRight,
          typename TReturn = typename TLeft::TReturnType>
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
  Collect<std::string> collect(const tanuki::String &in) override;
};

/**
 * @brief The AndToken class
 */
template <typename TLeft, typename TRight>
class AndToken : public BinaryToken<TLeft, TRight, std::string> {
 public:
  explicit AndToken(ref<TLeft> left, ref<TRight> right);
  ref<std::string> match(const tanuki::String &in) override;
  Collect<std::string> collect(const tanuki::String &in) override;
};

/**
 * @brief The RangeToken class
 */
template <typename TLeft, typename TRight>
class RangeToken : public Token<std::string> {
 public:
  explicit RangeToken(undirect_ref<TLeft> left, undirect_ref<TRight> right);
  ref<std::string> match(const tanuki::String &in) override;
  Collect<std::string> collect(const tanuki::String &in) override;

 private:
  undirect_ref<StartWithToken<TLeft>> m_left;
  undirect_ref<EndWithToken<TRight>> m_right;
};

template <typename TToken>
class WordToken : public Token<std::string> {
 public:
  explicit WordToken(undirect_ref<TToken> inner);
  ref<std::string> match(const tanuki::String &in) override;
  Collect<std::string> collect(const tanuki::String &in) override;

 private:
  undirect_ref<PlusToken<TToken, typename TToken::TReturnType>> m_inner;
};

// ------ Method --------
// Unary
template <typename TToken, typename TReturn>
UnaryToken<TToken, TReturn>::UnaryToken(undirect_ref<TToken> token)
    : Token<TReturn>(), m_token(token) {}

template <typename TToken, typename TReturn>
NotToken<TToken, TReturn>::NotToken(undirect_ref<TToken> token)
    : UnaryToken<TToken, std::string>(token) {}

template <typename TToken, typename TReturn>
ref<std::string> NotToken<TToken, TReturn>::match(const tanuki::String &in) {
  if (UnaryToken<TToken, std::string>::token()->match(in)) {
    return ref<std::string>();
  } else {
    return ref<std::string>(new std::string(in.toStdString()));
  }
}

template <typename TToken, typename TReturn>
Collect<std::string> NotToken<TToken, TReturn>::collect(
    const tanuki::String &in) {
  Collect<TReturn> result(
      UnaryToken<TToken, std::string>::token()->collect(in));

  if (result.second) {
    return std::make_pair(result.first,
                          ref<std::string>(new std::string(in.toStdString())));
  } else {
    return std::make_pair(0, ref<std::string>());
  }
}

template <typename TToken, typename TReturn>
PlusToken<TToken, TReturn>::PlusToken(undirect_ref<TToken> token)
    : UnaryToken<TToken, std::vector<ref<TReturn>>>(token) {}

template <typename TToken, typename TReturn>
ref<std::vector<ref<TReturn>>> PlusToken<TToken, TReturn>::match(
    const tanuki::String &in) {
  if (in.empty()) {
    return ref<std::vector<ref<TReturn>>>();
  }

  bool res = true;
  unsigned int current = 0;
  unsigned int length = in.size();

  ref<std::vector<ref<TReturn>>> result(new std::vector<ref<TReturn>>());

  while (current < length) {
    Collect<TReturn> currentRes =
        UnaryToken<TToken, std::vector<ref<TReturn>>>::token()->collect(
            in.substr(current));

    if (currentRes.second) {
      current += currentRes.first;
      result->push_back(currentRes.second);
    } else {
      res = false;
      break;
    }
  }

  if (res) {
    return result;
  } else {
    return ref<std::vector<ref<TReturn>>>();
  }
}

template <typename TToken, typename TReturn>
Collect<std::vector<ref<TReturn>>> PlusToken<TToken, TReturn>::collect(
    const tanuki::String &in) {
  if (in.empty()) {
    return std::make_pair(0, ref<std::vector<ref<TReturn>>>());
  }

  unsigned int current = 0;
  unsigned int length = in.size();

  ref<std::vector<ref<TReturn>>> result(new std::vector<ref<TReturn>>());

  while (current < length) {
    Collect<TReturn> currentRes =
        UnaryToken<TToken, std::vector<ref<TReturn>>>::token()->collect(
            in.substr(current));

    if (currentRes.second) {
      current += currentRes.first;
      result->push_back(currentRes.second);
    } else {
      break;
    }
  }

  if (result->empty()) {
    return std::make_pair(0, ref<std::vector<ref<TReturn>>>());
  } else {
    return std::make_pair(current, result);
  }
}

template <typename TToken, typename TReturn>
StarToken<TToken, TReturn>::StarToken(undirect_ref<TToken> token)
    : UnaryToken<TToken, std::vector<ref<TReturn>>>(token), m_inner(+token) {}

template <typename TToken, typename TReturn>
ref<std::vector<ref<TReturn>>> StarToken<TToken, TReturn>::match(
    const tanuki::String &in) {
  ref<std::vector<ref<TReturn>>> result = (m_inner->match(in));

  if (result) {
    return result;
  } else {
    return ref<std::vector<ref<TReturn>>>(new std::vector<ref<TReturn>>());
  }
}

template <typename TToken, typename TReturn>
Collect<std::vector<ref<TReturn>>> StarToken<TToken, TReturn>::collect(
    const tanuki::String &in) {
  Collect<std::vector<ref<TReturn>>> result = (m_inner->collect(in));

  if (result.second) {
    return result;
  } else {
    return std::make_pair(
        0, ref<std::vector<ref<TReturn>>>(new std::vector<ref<TReturn>>()));
  }
}

template <typename TToken, typename TReturn>
OptionalToken<TToken, TReturn>::OptionalToken(undirect_ref<TToken> token)
    : UnaryToken<TToken, std::vector<ref<TReturn>>>(token) {}

template <typename TToken, typename TReturn>
ref<std::vector<ref<TReturn>>> OptionalToken<TToken, TReturn>::match(
    const tanuki::String &in) {
  if (in.empty()) {
    return ref<std::vector<ref<TReturn>>>(new std::vector<ref<TReturn>>());
  }

  ref<std::vector<ref<TReturn>>> result(new std::vector<ref<TReturn>>());

  int exactSize =
      UnaryToken<TToken, std::vector<ref<TReturn>>>::token().exactSize();
  int length = in.size();

  if (exactSize == -1) {
    int biggestSize =
        UnaryToken<TToken, std::vector<ref<TReturn>>>::token().biggestSize();

    if (length <= biggestSize) {
      ref<TReturn> subResult =
          UnaryToken<TToken, std::vector<ref<TReturn>>>::token()->match(in);

      if (subResult) {
        result = ref<std::vector<ref<TReturn>>>(
            new std::vector<ref<TReturn>>({subResult}));
      }
    }
  } else {
    if (length == exactSize) {
      ref<TReturn> subResult =
          UnaryToken<TToken, std::vector<ref<TReturn>>>::token()->match(in);

      if (subResult) {
        result = ref<std::vector<ref<TReturn>>>(
            new std::vector<ref<TReturn>>({subResult}));
      }
    }
  }

  return result;
}

template <typename TToken, typename TReturn>
Collect<std::vector<ref<TReturn>>> OptionalToken<TToken, TReturn>::collect(
    const tanuki::String &in) {
  Collect<TReturn> result =
      UnaryToken<TToken, std::vector<ref<TReturn>>>::token()->collect(in);

  if (result.second) {
    return std::make_pair(result.first, ref<std::vector<ref<TReturn>>>(
                                            new std::vector<ref<TReturn>>()));
  } else {
    return std::make_pair(
        0, ref<std::vector<ref<TReturn>>>(new std::vector<ref<TReturn>>()));
  }
}

template <typename TToken, typename TReturn>
StartWithToken<TToken, TReturn>::StartWithToken(undirect_ref<TToken> token)
    : UnaryToken<TToken, TReturn>(token) {}

template <typename TToken, typename TReturn>
ref<TReturn> StartWithToken<TToken, TReturn>::match(const tanuki::String &in) {
  Collect<TReturn> result = UnaryToken<TToken, TReturn>::token()->collect(in);

  if (result.second) {
    return result.second;
  } else {
    return ref<TReturn>();
  }
}

template <typename TToken, typename TReturn>
Collect<TReturn> StartWithToken<TToken, TReturn>::collect(
    const tanuki::String &in) {
  Collect<TReturn> result = UnaryToken<TToken, TReturn>::token()->collect(in);

  if (result.second) {
    return result;
  } else {
    return std::make_pair(0, ref<TReturn>());
  }
}

template <typename TToken, typename TReturn>
EndWithToken<TToken, TReturn>::EndWithToken(undirect_ref<TToken> token)
    : UnaryToken<TToken, TReturn>(token) {}

template <typename TToken, typename TReturn>
ref<TReturn> EndWithToken<TToken, TReturn>::match(const tanuki::String &in) {
  if (in.empty()) {
    return ref<TReturn>();
  }

  int exactSize = UnaryToken<TToken, TReturn>::token().exactSize();
  int length = in.size();

  if (exactSize == -1) {
    ref<TReturn> result;

    int biggestSize = UnaryToken<TToken, TReturn>::token().biggestSize();
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

      result = UnaryToken<TToken, TReturn>::token()->match(in.substr(length));

      if (result) {
        break;
      }

    } while (length > minimum);

    return result;
  } else {
    int delta = (in.size() - exactSize);

    if (delta < 0) {
      return ref<TReturn>();
    } else {
      ref<TReturn> result =
          (UnaryToken<TToken, TReturn>::token()->match(in.substr(delta)));

      if (result) {
        return result;
      } else {
        return ref<TReturn>();
      }
    }
  }
}

template <typename TToken, typename TReturn>
Collect<TReturn> EndWithToken<TToken, TReturn>::collect(
    const tanuki::String &in) {
  int length = in.size();
  Collect<TReturn> result = std::make_pair(0, ref<TReturn>());

  for (int i = 0; i < length; i++) {
    result = (UnaryToken<TToken, TReturn>::token()->collect(in.substr(i)));

    if (result.second) {
      result.first = i;
      break;
    }
  }

  return result;
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
Collect<std::string> OrToken<TLeft, TRight>::collect(const tanuki::String &in) {
  Collect<typename TLeft::TReturnType> leftResult =
      (BinaryToken<TLeft, TRight, std::string>::left()->collect(in));

  if (leftResult.second) {
    return std::make_pair(leftResult.first,
                          ref<std::string>(new std::string(
                              in.substr(0, leftResult.first).toStdString())));
  } else {
    Collect<typename TRight::TReturnType> rightResult =
        (BinaryToken<TLeft, TRight, std::string>::right()->collect(in));

    if (rightResult.second) {
      return std::make_pair(
          rightResult.first,
          ref<std::string>(
              new std::string(in.substr(0, rightResult.first).toStdString())));
    } else {
      return std::make_pair(0, ref<std::string>());
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
Collect<std::string> AndToken<TLeft, TRight>::collect(
    const tanuki::String &in) {
  Collect<typename TLeft::TReturnType> leftResult =
      (BinaryToken<TLeft, TRight, std::string>::left()->collect(in));

  if (leftResult.second) {
    Collect<typename TRight::TReturnType> rightResult =
        (BinaryToken<TLeft, TRight, std::string>::right()->collect(in));

    if (rightResult.second) {
      if (leftResult.first == rightResult.first) {
        return std::make_pair(
            leftResult.first,
            new std::string(in.substr(0, rightResult.first).toStdString()));
      } else {
        return std::make_pair(0, ref<std::string>());
      }
    } else {
      return std::make_pair(0, ref<std::string>());
    }
  } else {
    return std::make_pair(0, ref<std::string>());
  }
}

template <typename TLeft, typename TRight>
RangeToken<TLeft, TRight>::RangeToken(undirect_ref<TLeft> left,
                                      undirect_ref<TRight> right)
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
Collect<std::string> RangeToken<TLeft, TRight>::collect(
    const tanuki::String &in) {
  Collect<std::string> result = std::make_pair(0, ref<std::string>());

  if (m_left->collect(in).second) {
    auto right = m_right->collect(in);

    if (right.second) {
      result = std::make_pair(right.first,
                              ref<std::string>(new std::string(
                                  in.substr(right.first).toStdString())));
    }
  }

  return result;
}

template <typename TToken>
WordToken<TToken>::WordToken(undirect_ref<TToken> inner)
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
Collect<std::string> WordToken<TToken>::collect(const tanuki::String &in) {
  Collect<std::vector<ref<typename TToken::TReturnType>>> result(
      m_inner->collect(in));

  if (result.second) {
    int length = result.first;

    return std::make_pair(
        result.first,
        ref<std::string>(new std::string(in.substr(0, length).toStdString())));
  } else {
    return std::make_pair(0, ref<std::string>());
  }
}
}