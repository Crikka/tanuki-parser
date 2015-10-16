#pragma once

#include <functional>
#include <vector>

#include "tanuki/misc/misc.h"
#include "tanuki/misc/exception.h"

#include "rule.h"

namespace tanuki {
template <typename TResult>
class Fragment {
 private:
  template <typename TRef>
  static ref<Fragment<TResult>> select(
      ref<Fragment<TResult>> self, TRef ref) {
    self->handle([](typename TRef::TDeepType in) -> tanuki::ref<TResult> {
      return ((typename TRef::TValue::TReturnType*)in);
    }, ref);

    return self;
  }

  template <typename TRef, typename... TRefs>
  static ref<Fragment<TResult>> select(
      ref<Fragment<TResult>> self, TRef ref, TRefs... refs) {
    self->handle([](typename TRef::TDeepType in) -> tanuki::ref<TResult> {
      return ((typename TRef::TValue::TReturnType*)in);
    }, ref);

    return Fragment<TResult>::select<TRefs...>(self, refs...);
  }

 public:
  typedef TResult TReturnType;

  virtual bool greedy() { return true; }
  virtual bool stopAtFirstGreedyFail() { return true; }
  int exactSize() { return -1; }
  int biggestSize() { return -1; }

  virtual ~Fragment() = default;

  template <typename TRef>
  static ref<Fragment<TResult>> select(TRef ref) {
    tanuki::ref<Fragment<TReturnType>> result(new Fragment<TResult>());

    result->handle([](typename TRef::TDeepType in) -> tanuki::ref<TResult> {
      return ((typename TRef::TValue::TReturnType*)in);
    }, ref);

    return result;
  }

  template <typename TRef, typename... TRefs>
  static ref<Fragment<TResult>> select(TRef ref, TRefs... refs) {
    tanuki::ref<Fragment<TResult>> result(new Fragment<TResult>());

    result->handle([](typename TRef::TDeepType in) -> tanuki::ref<TResult> {
      return ((typename TRef::TValue::TReturnType*)in);
    }, ref);

    return Fragment<TResult>::select<TRefs...>(result, refs...);
  }

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
    this->m_skipped.push_back([token](const tanuki::String& in) -> int {
      Collect<typename TToken::TValue::TReturnType> result = token->collect(in);

      if (result.second) {
        return result.first;
      } else {
        return 0;
      }
    });

    skip<TOther...>(other...);
  }

  template <typename TToken>
  void skip(TToken token) {
    this->m_skipped.push_back([token](const tanuki::String& in) -> int {
      Collect<typename TToken::TValue::TReturnType> result = token->collect(in);

      if (result.second) {
        return result.first;
      } else {
        return 0;
      }
    });
  }

  int shouldSkip(const tanuki::String& in) {
    int res = 0;

    for (const std::function<int(const tanuki::String&)>& skipped : m_skipped) {
      int current = skipped(in);

      if (current > 0) {
        res = current;
        break;
      }
    }

    return res;
  }

 private:
  std::vector<ref<Matchable<TResult>>> m_rules;
  std::vector<std::function<int(const tanuki::String&)>> m_skipped;
};

template <typename T>
tanuki::ref<Fragment<T>> fragment() {
  return tanuki::ref<Fragment<T>>(new Fragment<T>);
}
}
