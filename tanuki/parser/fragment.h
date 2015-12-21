#pragma once

#include <functional>
#include <vector>
#include <cassert>

#include "tanuki/misc/misc.h"
#include "tanuki/misc/exception.h"

#include "rule.h"

namespace tanuki {
template <typename TResult>
class Fragment {
 private:
  template <typename TRef>
  static ref<Fragment<TResult>> select(ref<Fragment<TResult>> self, TRef ref) {
    self->handle(
        [](typename TRef::TDeepType in) -> tanuki::ref<TResult> {
          return ((typename TRef::TValue::TReturnType*)in);
        },
        ref);

    return self;
  }

  template <typename TRef, typename... TRefs>
  static ref<Fragment<TResult>> select(ref<Fragment<TResult>> self, TRef ref,
                                       TRefs... refs) {
    self->handle(
        [](typename TRef::TDeepType in) -> tanuki::ref<TResult> {
          return ((typename TRef::TValue::TReturnType*)in);
        },
        ref);

    return Fragment<TResult>::select<TRefs...>(self, refs...);
  }

 public:
  typedef TResult TReturnType;

  virtual bool greedy() { return true; }
  virtual bool stopAtlengthGreedyFail() { return true; }
  int exactSize() { return -1; }
  int biggestSize() { return -1; }

  Fragment() : m_skipped(false) {}
  virtual ~Fragment() = default;

  template <typename TRef>
  static ref<Fragment<TResult>> select(TRef ref) {
    tanuki::ref<Fragment<TReturnType>> result(new Fragment<TResult>());

    result->handle(
        [](typename TRef::TDeepType in) -> tanuki::ref<TResult> {
          return ((typename TRef::TValue::TReturnType*)in);
        },
        ref);

    return result;
  }

  template <typename TRef, typename... TRefs>
  static ref<Fragment<TResult>> select(TRef ref, TRefs... refs) {
    tanuki::ref<Fragment<TResult>> result(new Fragment<TResult>());

    result->handle(
        [](typename TRef::TDeepType in) -> tanuki::ref<TResult> {
          return ((typename TRef::TValue::TReturnType*)in);
        },
        ref);

    return Fragment<TResult>::select<TRefs...>(result, refs...);
  }

  template <typename... TRefs>
  void handle(std::function<ref<TResult>(
                  std::tuple<typename TRefs::TDeepType...>)> callback,
              TRefs... refs) {
    Rule<TResult, TRefs...>* rule =
        new Rule<TResult, TRefs...>(this, refs..., callback);
    if (isLeftRecursive<TRefs...>(refs...)) {
      if (sizeof...(TRefs) == 1) {
        assert(false && "You try to create a rule : S -> S");
      }

      m_lr_rules.push_back(ref<Matchable<TResult>>(rule));
    } else {
      m_nlr_rules.push_back(ref<Matchable<TResult>>(rule));
    }
  }

  template <typename... TRefs>
  void handle(
      std::function<ref<TResult>(typename TRefs::TDeepType...)> callback,
      TRefs... refs) {
    Rule<TResult, TRefs...>* rule =
        new Rule<TResult, TRefs...>(this, refs..., callback);
    if (isLeftRecursive<TRefs...>(refs...)) {
      if (sizeof...(TRefs) == 1) {
        assert(false && "You try to create a rule : S -> S");
      }

      m_lr_rules.push_back(ref<Matchable<TResult>>(rule));
    } else {
      m_nlr_rules.push_back(ref<Matchable<TResult>>(rule));
    }
  }

  template <typename TRef, typename... TRefs>
  bool isLeftRecursive(TRef ref, TRefs...) {
    return (ref == this);
  }

  template <typename... TRefs>
  bool isLeftRecursive() {
    return false;
  }

  tanuki::ref<TResult> match(const tanuki::String& input) {
    ref<TResult> result;

    ref<std::vector<Piece<TResult>>> nonLeftRecursiveResults(
        new std::vector<Piece<TResult>>);

    for (ref<Matchable<TResult>> rule : m_nlr_rules) {
      Piece<TResult> inner = rule->consume(input);

      if (inner) {
        nonLeftRecursiveResults->push_back(inner);
      }
    }

    Yielder<Piece<TResult>> own;
    own.load(nonLeftRecursiveResults);

    for (Piece<TResult> sub : own) {
      if (sub.length == input.size()) {
        result = sub.result;
        break;
      }
    }

    if (!m_lr_rules.empty() && !(bool)result) {
      int leftRecursiveCount = m_lr_rules.size();

      Yielder<Piece<TResult>>* queues =
          new Yielder<Piece<TResult>>[leftRecursiveCount];
      for (int i = 0; i < leftRecursiveCount; i++) {
        queues[i].load(nonLeftRecursiveResults);
      }

      int initialResultLength, current;

      do {
        initialResultLength = own.size();
        current = 0;

        for (ref<Matchable<TResult>> rule : m_lr_rules) {
          rule->consume(input, queues[current]);

          for (Piece<TResult> sub : own) {
            if (sub.length == input.size()) {
              result = sub.result;
              goto out;
            }
          }

          current++;
        }

      } while (initialResultLength < own.size());
    out:

      delete[] queues;
    }

    return result;
  }

  tanuki::Piece<TResult> consume(const tanuki::String& input) {
    tanuki::Piece<TResult> result{0, ref<TResult>()};

    ref<std::vector<Piece<TResult>>> nonLeftRecursiveResults(
        new std::vector<Piece<TResult>>);

    for (ref<Matchable<TResult>> rule : m_nlr_rules) {
      Piece<TResult> inner = rule->consume(input);

      if (inner) {
        nonLeftRecursiveResults->push_back(inner);
      }
    }

    Yielder<Piece<TResult>> own;
    own.load(nonLeftRecursiveResults);

    for (Piece<TResult> sub : own) {
      if (result.length < sub.length) {
        result = sub;

        if (sub.length == input.size()) {
          goto end;
        }
      }
    }

    if (!m_lr_rules.empty()) {
      int leftRecursiveCount = m_lr_rules.size();

      Yielder<Piece<TResult>>* queues =
          new Yielder<Piece<TResult>>[leftRecursiveCount];
      for (int i = 0; i < leftRecursiveCount; i++) {
        queues[i].load(nonLeftRecursiveResults);
      }

      int initialResultLength, current;

      do {
        initialResultLength = own.size();
        current = 0;

        for (ref<Matchable<TResult>> rule : m_lr_rules) {
          rule->consume(input, queues[current]);

          for (Piece<TResult> sub : own) {
            if (result.length < sub.length) {
              result = sub;

              if (sub.length == input.size()) {
                goto out;
              }
            }
          }

          current++;
        }

      } while (initialResultLength < own.size());
    out:

      delete[] queues;
    }

  end:

    return result;
  }

  template <typename TToken, typename... TOther>
  void skip(TToken token, TOther... other) {
    this->m_skipped.push_back([token](const tanuki::String& in) -> int {
      Piece<typename TToken::TValue::TReturnType> result = token->consume(in);

      if (result.result) {
        return result.length;
      } else {
        return 0;
      }
    });

    skip<TOther...>(other...);
  }

  template <typename TToken>
  void skip(TToken token) {
    this->m_skipped.push_back([token](const tanuki::String& in) -> int {
      Piece<typename TToken::TValue::TReturnType> result = token->consume(in);

      if (result.result) {
        return result.length;
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

  bool skipAtEnd;

 private:
  std::vector<ref<Matchable<TResult>>> m_lr_rules;
  std::vector<ref<Matchable<TResult>>> m_nlr_rules;
  std::vector<std::function<int(const tanuki::String&)>> m_skipped;
};

template <typename T>
tanuki::ref<Fragment<T>> fragment() {
  return tanuki::ref<Fragment<T>>(new Fragment<T>);
}
}
