#pragma once

#include <functional>
#include <vector>

#include "tanuki/misc/misc.h"
#include "tanuki/misc/exception.h"

namespace tanuki {
namespace lexer {
template <typename, typename...>
class Rule;

template <typename TResult>
class Fragment {
 public:
  typedef TResult TReturnType;

  ~Fragment() {
    for (std::function<tanuki::ref<TResult>(const std::string&)>* rule : m_rules) {
      delete rule;
    }
  }

  template <typename TRef, typename... TRestRef>
  Rule<TResult, typename TRef::TDeepType, typename TRestRef::TDeepType...>* on(
      TRef ref, TRestRef... rest) {
    Rule<TResult, typename TRef::TDeepType, typename TRestRef::TDeepType...>*
        rule =
            Rule<TResult, typename TRef::TDeepType,
                 typename TRestRef::TDeepType...>::create(this, ref, rest...);

    m_rules.push_back(rule);

    return rule;
  }

  tanuki::ref<TResult> match(const std::string &input) {
    for (std::function<tanuki::ref<TResult>(const std::string&)>* rule : m_rules) {
      try {
        tanuki::ref<TResult> result = (*rule)(input);

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
    this->m_ignored.push_back(
        [=](const std::string& in) -> bool { return (token->match(in) == true); });

    ignore<TOther...>(other...);
  }

  template <typename TToken>
  void ignore(TToken token) {
    this->m_ignored.push_back(
        [=](const std::string& in) -> bool { return (token->match(in) == true); });
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
  std::vector<std::function<tanuki::ref<TResult>(const std::string&)>*> m_rules;
  std::vector<std::function<bool(const std::string&)>> m_ignored;
};


template<typename T>
tanuki::undirect_ref<Fragment<T>> fragment() {
  return tanuki::undirect_ref<Fragment<T>>(new Fragment<T>);
}
}
}
