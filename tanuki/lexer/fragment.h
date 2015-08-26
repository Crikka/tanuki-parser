#pragma once

#include <functional>
#include <vector>

#include "tanuki/misc/misc.h"
#include "tanuki/misc/exception.h"

namespace tanuki {
namespace lexer {
template <typename>
class Matchable;

template <typename, typename...>
class Rule;

template <typename TResult>
class Fragment {
 public:
  typedef TResult TReturnType;

  virtual bool greedy() { return true; }
  virtual bool stopAtFirstGreedyFail() { return true; }
  int exactSize() { return -1; }
  int biggestSize() { return -1; }

  ~Fragment() {
    for (Matchable<TResult>* rule : m_rules) {
      delete rule;
    }
  }

  template <typename... TRefs>
  void handle(std::function<ref<TResult>(std::tuple<typename TRefs::TDeepType...>)> callback, TRefs... refs) {
    Rule<TResult, TRefs...>* rule = new Rule<TResult, TRefs...>(this, refs..., callback);

    m_rules.push_back(rule);
  }

  template <typename... TRefs>
  void handle(std::function<ref<TResult>(typename TRefs::TDeepType...)> callback, TRefs... refs) {
    Rule<TResult, TRefs...>* rule = new Rule<TResult, TRefs...>(this, refs..., callback);

    m_rules.push_back(rule);
  }

  tanuki::ref<TResult> match(const std::string& input) {
    for (Matchable<TResult>* rule : m_rules) {
      try {
        tanuki::ref<TResult> result = rule->match(input);

        if (!result.isNull()) {
          return result;
        }
      } catch (NoExecuteDefinition&) {
        throw;
      }
    }

    return tanuki::ref<TResult>();
  }

  template <typename TToken, typename... TOther>
  void ignore(TToken token, TOther... other) {
    this->m_ignored.push_back([=](const std::string& in) -> bool {
      return (token->match(in) == true);
    });

    ignore<TOther...>(other...);
  }

  template <typename TToken>
  void ignore(TToken token) {
    this->m_ignored.push_back([=](const std::string& in) -> bool {
      return (token->match(in) == true);
    });
  }

  bool shouldIgnore(const std::string& in) {
    bool res = false;

    for (const std::function<bool(const std::string&)>& ignored : m_ignored) {
      if (ignored(in)) {
        res = true;
        break;
      }
    }

    return res;
  }

 private:
  std::vector<Matchable<TResult>*> m_rules;
  std::vector<std::function<bool(const std::string&)>> m_ignored;
};

template <typename T>
tanuki::undirect_ref<Fragment<T>> fragment() {
  return tanuki::undirect_ref<Fragment<T>>(new Fragment<T>);
}
}
}
