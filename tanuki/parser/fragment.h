#pragma once

#include <functional>
#include <vector>

#include "tanuki/misc/misc.h"
#include "tanuki/misc/exception.h"

#include "rule.h"

namespace tanuki {
template <typename TResult>
class Fragment {
 public:
  typedef TResult TReturnType;

  virtual bool greedy() { return true; }
  virtual bool stopAtFirstGreedyFail() { return true; }
  int exactSize() { return -1; }
  int biggestSize() { return -1; }

  virtual ~Fragment() = default;

  template <typename... TRefs>
  void handle(std::function<ref<TResult>(
                  std::tuple<typename TRefs::TDeepType...>)> callback,
              TRefs... refs) {
    handle<TRefs...>(-1, callback, refs...);
  }

  template <typename... TRefs>
  void handle(short weight,
              std::function<ref<TResult>(
                  std::tuple<typename TRefs::TDeepType...>)> callback,
              TRefs... refs) {
    Rule<TResult, TRefs...>* rule =
        new Rule<TResult, TRefs...>(this, refs..., callback);

    m_rules.push_back(ref<Matchable<TResult>>(rule));
    rule->weight = weight;
  }

  template <typename... TRefs>
  void handle(
      std::function<ref<TResult>(typename TRefs::TDeepType...)> callback,
      TRefs... refs) {
    handle<TRefs...>(-1, callback, refs...);
  }

  template <typename... TRefs>
  void handle(
      short weight,
      std::function<ref<TResult>(typename TRefs::TDeepType...)> callback,
      TRefs... refs) {
    Rule<TResult, TRefs...>* rule =
        new Rule<TResult, TRefs...>(this, refs..., callback);

    m_rules.push_back(ref<Matchable<TResult>>(rule));
    rule->weight = weight;
  }

  tanuki::ref<TResult> match(const tanuki::String& input) {
    tanuki::ref<TResult> result;
    int lastWeight = 0;

    for (ref<Matchable<TResult>>& rule : m_rules) {
      try {
        tanuki::ref<TResult> inner = rule->match(input);

        if (!inner.isNull()) {
          if (rule->weight == -1) {
            result = inner;
            break;
          } else if (rule->weight > lastWeight) {
            lastWeight = rule->weight;
            result = inner;
          }
        }
      } catch (NoExecuteDefinition&) {
        throw;
      }
    }

    return result;
  }

  tanuki::Collect<TResult> collect(const tanuki::String& input) {
    tanuki::Collect<TResult> result;
    int lastWeight = 0;

    for (ref<Matchable<TResult>>& rule : m_rules) {
      try {
        tanuki::Collect<TResult> inner = rule->collect(input);

        if (!inner.second.isNull()) {
          if (rule->weight == -1) {
            result = inner;
            break;
          } else if (rule->weight > lastWeight) {
            lastWeight = rule->weight;
            result = inner;
          }
        }
      } catch (NoExecuteDefinition&) {
        throw;
      }
    }

    return result;
  }

  template <typename TToken, typename... TOther>
  void skip(TToken token, TOther... other) {
    this->m_skipped.push_back([token](const tanuki::String& in) -> bool {
      return (token->match(in) == true);
    });

    skip<TOther...>(other...);
  }

  template <typename TToken>
  void skip(TToken token) {
    this->m_skipped.push_back([token](const tanuki::String& in) -> bool {
      return (token->match(in) == true);
    });
  }

  bool shouldSkip(const tanuki::String& in) {
    bool res = false;

    for (const std::function<bool(const tanuki::String&)>& skipped :
         m_skipped) {
      if (skipped(in)) {
        res = true;
        break;
      }
    }

    return res;
  }

 private:
  std::vector<ref<Matchable<TResult>>> m_rules;
  std::vector<std::function<bool(const tanuki::String&)>> m_skipped;
};

template <typename T>
tanuki::undirect_ref<Fragment<T>> fragment() {
  return tanuki::undirect_ref<Fragment<T>>(new Fragment<T>);
}
}
