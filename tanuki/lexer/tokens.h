#pragma once

#include <string>
#include <vector>
#include <utility>

#include "tanuki/misc/misc.h"
#include "operation.h"

namespace re2 {
class RE2;
}

namespace tanuki {
namespace lexer {
// Root
template <typename>
class Token;

// Simple
class ConstantToken;
class RegexToken;
class IntegerToken;

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
template <typename, typename, typename>
class OrToken;
template <typename, typename, typename>
class AndToken;
template <typename, typename>
class RangeToken;

// ~~~~~~~~ Helper

// Method facility
undirect_ref<ConstantToken> constant(char character);
undirect_ref<ConstantToken> constant(const std::string &constant);
undirect_ref<RegexToken> regex(const std::string &regex);
undirect_ref<IntegerToken> integer();

// Operator
template <typename TLeft, typename TRight,
          typename TReturn = typename TLeft::TReturnType>
undirect_ref<OrToken<TLeft, TRight, TReturn>> operator||(ref<TLeft> left,
                                                         ref<TRight> right);
template <typename TToken, typename TReturn = typename TToken::TReturnType>
undirect_ref<PlusToken<TToken, TReturn>> operator+(ref<TToken> token);

template <typename TLeft, typename TRight,
          typename TReturn = typename TLeft::TReturnType>
undirect_ref<OrToken<TLeft, TRight, TReturn>> operator||(ref<TLeft> left,
                                                         ref<TRight> right);

// Helper constants
undirect_ref<ConstantToken> &space();
undirect_ref<ConstantToken> &tab();
undirect_ref<OrToken<ConstantToken, ConstantToken, std::string>> &blank();
undirect_ref<OrToken<ConstantToken, ConstantToken, std::string>> &
lineTerminator();

// Declaration

/**
 * @brief The Token class is the root class of Tokens.
 */
template <typename TReturn>
class Token {
 public:
  virtual ref<TReturn> match(const std::string &in) = 0;
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
  explicit ConstantToken(char constant);
  ref<std::string> match(const std::string &in) override;
  bool greedy() override { return false; }
  int exactSize() override { return m_constant.size(); }

 private:
  std::string m_constant;
};

/**
 * @brief The RegexToken class represents a regular expression token.
 */
class RegexToken : public Token<std::string> {
 public:
  explicit RegexToken(const std::string &regex);
  virtual ~RegexToken();
  ref<std::string> match(const std::string &in) override;
  bool stopAtFirstGreedyFail() override { return false; }

 private:
  re2::RE2 *m_regex;
  std::string m_result;
};

/**
 * @brief The IntegerToken class represents an integer.
 */
class IntegerToken : public Token<int> {
 public:
  explicit IntegerToken();
  virtual ~IntegerToken();
  ref<int> match(const std::string &in) override;

 private:
  re2::RE2 *m_regex;
  int m_result;
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
  ref<std::string> match(const std::string &in) override;
};

/**
 * @brief The PlusToken class represents a token repeting a true at least once
 * on not inner token.
 */
template <typename TToken, typename TReturn = typename TToken::TReturnType>
class PlusToken : public UnaryToken<TToken, std::vector<ref<TReturn>>> {
 public:
  explicit PlusToken(undirect_ref<TToken> token);
  ref<std::vector<ref<TReturn>>> match(const std::string &in) override;
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
  ref<std::vector<ref<TReturn>>> match(const std::string &in) override;
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
  ref<std::vector<ref<TReturn>>> match(const std::string &in) override;
};

/**
 * @brief The StartWith class
 */
template <typename TToken, typename TReturn = typename TToken::TReturnType>
class StartWithToken : public UnaryToken<TToken, TReturn> {
 public:
  explicit StartWithToken(undirect_ref<TToken> inner);
  ref<TReturn> match(const std::string &in) override;
};

/**
 * @brief The EndWith class
 */
template <typename TToken, typename TReturn = typename TToken::TReturnType>
class EndWithToken : public UnaryToken<TToken, TReturn> {
 public:
  explicit EndWithToken(undirect_ref<TToken> inner);
  ref<TReturn> match(const std::string &in) override;
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
template <typename TLeft, typename TRight,
          typename TReturn = typename TLeft::TReturnTypet>
class OrToken : public BinaryToken<TLeft, TRight, TReturn> {
 public:
  explicit OrToken(ref<TLeft> left, ref<TRight> right);
  ref<TReturn> match(const std::string &in) override;
};

/**
 * @brief The AndToken class
 */
template <typename TLeft, typename TRight,
          typename TReturn = typename TLeft::TReturnType>
class AndToken : public BinaryToken<TLeft, TRight, TReturn> {
 public:
  explicit AndToken(ref<TLeft> left, ref<TRight> right);
  ref<TReturn> match(const std::string &in) override;
};

/**
 * @brief The RangeToken class
 */
template <typename TLeft, typename TRight>
class RangeToken : public Token<std::string> {
 public:
  explicit RangeToken(undirect_ref<TLeft> left, undirect_ref<TRight> right);
  ref<std::string> match(const std::string &in) override;

 private:
  undirect_ref<StartWithToken<TLeft>> m_left;
  undirect_ref<EndWithToken<TRight>>  m_right;
};

// ------ Method --------

// Unary
template <typename TToken, typename TReturn>
UnaryToken<TToken, TReturn>::UnaryToken(undirect_ref<TToken> token)
    : Token<TReturn>(), m_token(token) {}

template <typename TToken, typename TReturn>
NotToken<TToken, TReturn>::NotToken(undirect_ref<TToken> token)
    : UnaryToken<TToken, TReturn>(token) {}

template <typename TToken, typename TReturn>
ref<std::string> NotToken<TToken, TReturn>::match(const std::string &in) {
  if (UnaryToken<TToken, TReturn>::token()->match(in)) {
    return ref<std::string>();
  } else {
    return ref<std::string>(new std::string(in));
  }
}

template <typename TToken, typename TReturn>
PlusToken<TToken, TReturn>::PlusToken(undirect_ref<TToken> token)
    : UnaryToken<TToken, std::vector<ref<TReturn>>>(token) {}

template <typename TToken, typename TReturn>
ref<std::vector<ref<TReturn>>> PlusToken<TToken, TReturn>::match(
    const std::string &in) {
  bool res = false;
  bool firstPass = true;

  unsigned int length = in.size();
  std::vector<std::pair<unsigned int, unsigned int>> history;
  ref<std::vector<ref<TReturn>>> result(new std::vector<ref<TReturn>>());

  do {
    unsigned int current;
    unsigned int size;
    unsigned int position;

    if (firstPass) {
      firstPass = false;

      current = 0;
      size = 1;
      position = 1;
    } else {
      std::pair<unsigned int, unsigned int> &last = history.back();
      history.pop_back();
      result->pop_back();

      current = last.first;
      size = (last.second + 1);
      position = current + size;
    }

    bool lastMatch;
    while (position <= length) {
      std::string buffer = in.substr(current, size);
      ref<TReturn> currentRes =
          UnaryToken<TToken, std::vector<ref<TReturn>>>::token()->match(buffer);

      lastMatch = !currentRes.isNull();

      if (!currentRes.isNull()) {
        history.push_back(std::pair<unsigned int, unsigned int>(current, size));
        result->push_back(currentRes);
        current += size;
        size = 1;
      } else {
        size++;
      }

      position = current + size;
    }

    res = lastMatch;
  } while (!res && !(history.empty()));

  if (res) {
    return result;
  } else {
    return ref<std::vector<ref<TReturn>>>();
  }
}

template <typename TToken, typename TReturn>
StarToken<TToken, TReturn>::StarToken(undirect_ref<TToken> token)
    : UnaryToken<TToken, std::vector<ref<TReturn>>>(token), m_inner(+token) {}

template <typename TToken, typename TReturn>
ref<std::vector<ref<TReturn>>> StarToken<TToken, TReturn>::match(
    const std::string &in) {
  ref<std::vector<ref<TReturn>>> result = (m_inner->match(in));

  if (result.isNull()) {
    return ref<std::vector<ref<TReturn>>>(new std::vector<ref<TReturn>>());
  } else {
    return result;
  }
}

template <typename TToken, typename TReturn>
OptionalToken<TToken, TReturn>::OptionalToken(undirect_ref<TToken> token)
    : UnaryToken<TToken, std::vector<ref<TReturn>>>(token) {}

template <typename TToken, typename TReturn>
ref<std::vector<ref<TReturn>>> OptionalToken<TToken, TReturn>::match(
    const std::string &in) {
  ref<TReturn> result =
      (UnaryToken<TToken, std::vector<ref<TReturn>>>::token()->match(in));

  if (result.isNull()) {
    return ref<std::vector<ref<TReturn>>>(new std::vector<ref<TReturn>>());
  } else {
    return ref<std::vector<ref<TReturn>>>(
        new std::vector<ref<TReturn>>({result}));
  }
}

template <typename TToken, typename TReturn>
StartWithToken<TToken, TReturn>::StartWithToken(undirect_ref<TToken> token)
    : UnaryToken<TToken, TReturn>(token) {}

template <typename TToken, typename TReturn>
ref<TReturn> StartWithToken<TToken, TReturn>::match(const std::string &in) {
  if (in.empty()) {
    return ref<TReturn>();
  }

  int exactSize = UnaryToken<TToken, TReturn>::token().exactSize();
  int length = in.size();

  if (exactSize == -1) {
    ref<TReturn> result;

    int biggestSize = UnaryToken<TToken, TReturn>::token().biggestSize();
    int maximum;
    int current = 0;

    if (biggestSize == -1) {
      maximum = length;
    } else {
      maximum = biggestSize;

      if (maximum > length) {
        maximum = length;
      }
    }

    do {
      current++;
      std::string buffer = in.substr(0, current);

      result = UnaryToken<TToken, TReturn>::token()->match(buffer);

      if (result) {
        break;
      }

    } while (current < maximum);

    return result;
  } else {
    int delta = (in.size() - exactSize);

    if (delta < 0) {
      return ref<TReturn>();
    } else {
      ref<TReturn> result = (UnaryToken<TToken, TReturn>::token()->match(
          in.substr(0, exactSize)));

      if (result) {
        return result;
      } else {
        return ref<TReturn>();
      }
    }
  }
}

template <typename TToken, typename TReturn>
EndWithToken<TToken, TReturn>::EndWithToken(undirect_ref<TToken> token)
    : UnaryToken<TToken, TReturn>(token) {}

template <typename TToken, typename TReturn>
ref<TReturn> EndWithToken<TToken, TReturn>::match(const std::string &in) {
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
      std::string buffer = in.substr(length);

      result = UnaryToken<TToken, TReturn>::token()->match(buffer);

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

// Binary
template <typename TLeft, typename TRight, typename TReturn>
BinaryToken<TLeft, TRight, TReturn>::BinaryToken(ref<TLeft> left,
                                                 ref<TRight> right)
    : Token<TReturn>(), m_left(left), m_right(right) {}

template <typename TLeft, typename TRight, typename TReturn>
OrToken<TLeft, TRight, TReturn>::OrToken(ref<TLeft> left, ref<TRight> right)
    : BinaryToken<TLeft, TRight, TReturn>(left, right) {}

template <typename TLeft, typename TRight, typename TReturn>
ref<TReturn> OrToken<TLeft, TRight, TReturn>::match(const std::string &in) {
  ref<TReturn> leftResult =
      (BinaryToken<TLeft, TRight, TReturn>::left()->match(in));

  if (!leftResult.isNull()) {
    return leftResult;
  } else {
    ref<TReturn> rightResult =
        (BinaryToken<TLeft, TRight, TReturn>::right()->match(in));

    if (!rightResult.isNull()) {
      return rightResult;
    } else {
      return ref<TReturn>();
    }
  }
}

template <typename TLeft, typename TRight, typename TReturn>
AndToken<TLeft, TRight, TReturn>::AndToken(ref<TLeft> left, ref<TRight> right)
    : BinaryToken<TLeft, TRight, TReturn>(left, right) {}

template <typename TLeft, typename TRight, typename TReturn>
ref<TReturn> AndToken<TLeft, TRight, TReturn>::match(const std::string &in) {
  ref<TReturn> leftResult =
      BinaryToken<TLeft, TRight, TReturn>::left()->match(in);
  ref<TReturn> rightResult =
      BinaryToken<TLeft, TRight, TReturn>::right()->match(in);

  if (!leftResult.isNull() && !rightResult.isNull()) {
    return leftResult;
  } else {
    return ref<TReturn>();
  }
}

template <typename TLeft, typename TRight>
RangeToken<TLeft, TRight>::RangeToken(undirect_ref<TLeft> left,
                                      undirect_ref<TRight> right)
    : Token<std::string>(), m_left(startWith(left)), m_right(endWith(right)) {}

template <typename TLeft, typename TRight>
ref<std::string> RangeToken<TLeft, TRight>::match(const std::string &in) {
  ref<std::string> result;

  if (m_left->match(in)) {
    if (m_right->match(in)) {
      result = ref<std::string>(new std::string(in));
    }
  }

  return result;
}
}
}
