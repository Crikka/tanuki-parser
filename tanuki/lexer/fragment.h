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
  typedef TResult TReturnValue;

  ~Fragment() {
    for (std::function<tanuki::ref<TResult>(std::string&)>* rule : m_rules) {
      delete rule;
    }
  }

  template <typename TRef, typename... TRestRef>
  Rule<TResult, typename TRef::TValue::TReturnValue,
       typename TRestRef::TValue::TReturnValue...>*
  on(TRef ref, TRestRef... rest) {
    Rule<TResult, typename TRef::TValue::TReturnValue,
         typename TRestRef::TValue::TReturnValue...>* rule =
        Rule<TResult, typename TRef::TValue::TReturnValue,
             typename TRestRef::TValue::TReturnValue...>::create(this, ref,
                                                                 rest...);
    m_rules.push_back(rule);

    return rule;
  }

  tanuki::ref<TResult> match(std::string input) {
    for (std::function<tanuki::ref<TResult>(std::string&)>* rule : m_rules) {
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
        [=](std::string& in) -> bool { return (token->match(in) == true); });

    ignore<TOther...>(other...);
  }

  template <typename TToken>
  void ignore(TToken token) {
    this->m_ignored.push_back(
        [=](std::string& in) -> bool { return (token->match(in) == true); });
  }

  bool shouldIgnore(std::string& in) {
    bool res = false;

    for (const std::function<bool(std::string&)>& ignored : m_ignored) {
      if (ignored(in)) {
        res = true;
        break;
      }
    }

    return res;
  }

 private:
  std::vector<std::function<tanuki::ref<TResult>(std::string&)>*> m_rules;
  std::vector<std::function<bool(std::string&)>> m_ignored;
};
}
}
