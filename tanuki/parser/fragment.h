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

  ref<std::vector<Piece<TResult>>> consumeNonLeftRecursive(
      const tanuki::String& input) {
    ref<std::vector<Piece<TResult>>> result(new std::vector<Piece<TResult>>);

    for (ref<Matchable<TResult>> rule : m_rules) {
      try {
        if (!rule->isLeftRecursive) {
          Piece<TResult> inner = rule->consume(input);

          if (inner) {
            result->push_back(inner);
          }
        }
      } catch (NoExecuteDefinition&) {
        throw;
      }
    }

    return result;
  }

 public:
  typedef TResult TReturnType;

  virtual bool greedy() { return true; }
  virtual bool stopAtlengthGreedyFail() { return true; }
  int exactSize() { return -1; }
  int biggestSize() { return -1; }

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
  void handle(
              std::function<ref<TResult>(
                  std::tuple<typename TRefs::TDeepType...>)> callback,
              TRefs... refs) {
    Rule<TResult, TRefs...>* rule =
        new Rule<TResult, TRefs...>(this, refs..., callback);
    rule->isLeftRecursive = isLeftRecursive<TRefs...>(refs...);
    if (rule->isLeftRecursive) {
      if (sizeof...(TRefs) == 1) {
        assert(false && "You try to create a rule : S -> S");
      }
    }

    if (rule->isLeftRecursive) {
      m_recursiveRulesCount++;
    }

    m_rules.push_back(ref<Matchable<TResult>>(rule));
  }

  template <typename... TRefs>
  void handle(
      std::function<ref<TResult>(typename TRefs::TDeepType...)> callback,
      TRefs... refs) {
    Rule<TResult, TRefs...>* rule =
        new Rule<TResult, TRefs...>(this, refs..., callback);
    rule->isLeftRecursive = isLeftRecursive<TRefs...>(refs...);

    if (rule->isLeftRecursive) {
      if (sizeof...(TRefs) == 1) {
        assert(false && "You try to create a rule : S -> S");
      }
    }

    if (rule->isLeftRecursive) {
      m_recursiveRulesCount++;
    }

    m_rules.push_back(ref<Matchable<TResult>>(rule));
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

    if (m_recursiveRulesCount) {
      ref<std::vector<Piece<TResult>>> nonLeftRecursiveResults =
          this->consumeNonLeftRecursive(input);

      Yielder<Piece<TResult>> own;
      own.load(nonLeftRecursiveResults);

      for (Piece<TResult> sub : own) {
        if (sub.length == input.size()) {
          result = sub.result;
          break;
        }
      }

      if (!nonLeftRecursiveResults->empty() && (!(bool)result)) {
        std::vector<ref<Matchable<TResult>>> leftRecursiveRules;

        Yielder<Piece<TResult>>* queues =
            new Yielder<Piece<TResult>>[m_recursiveRulesCount];
        for (int i = 0; i < m_recursiveRulesCount; i++) {
          queues[i].load(nonLeftRecursiveResults);
        }

        for (ref<Matchable<TResult>> rule : m_rules) {
          try {
            if (rule->isLeftRecursive) {
              int current = leftRecursiveRules.size();
              leftRecursiveRules.push_back(rule);
              int initialResultLength;

              do {
                initialResultLength = own.size();

                for (int i = 0; i <= current; i++) {
                  leftRecursiveRules[i]->consume(input, queues[i]);

                  for (Piece<TResult> sub : own) {
                    if (sub.length == input.size()) {
                      result = sub.result;
                      goto out;
                    }
                  }
                }
              } while (initialResultLength < own.size());
            }
          } catch (NoExecuteDefinition&) {
            throw;
          }
        }
      out:

        delete[] queues;
      }
    } else {
      for (ref<Matchable<TResult>> rule : m_rules) {
        try {
          Piece<TResult> inner = rule->consume(input);

          if (inner) {
            if (inner.length == input.size()) {
              result = inner.result;
              break;
            }
          }
        } catch (NoExecuteDefinition&) {
          throw;
        }
      }
    }

    return result;
  }

  tanuki::Piece<TResult> consume(const tanuki::String& input) {
    tanuki::Piece<TResult> result;
    uint32_t best_consume = 0;

    if (m_recursiveRulesCount) {
      ref<std::vector<Piece<TResult>>> nonLeftRecursiveResults =
          this->consumeNonLeftRecursive(input);

      Yielder<Piece<TResult>> own;
      own.load(nonLeftRecursiveResults);

      for (Piece<TResult> sub : own) {
        if (sub.length > best_consume) {
          result = sub;
          best_consume = sub.length;
        }
      }

      if (!nonLeftRecursiveResults->empty()) {
        std::vector<ref<Matchable<TResult>>> leftRecursiveRules;

        Yielder<Piece<TResult>>* queues =
            new Yielder<Piece<TResult>>[m_recursiveRulesCount];
        for (int i = 0; i < m_recursiveRulesCount; i++) {
          queues[i].load(nonLeftRecursiveResults);
        }

        for (ref<Matchable<TResult>> rule : m_rules) {
          try {
            if (rule->isLeftRecursive) {
              int current = leftRecursiveRules.size();
              leftRecursiveRules.push_back(rule);
              int initialResultLength;

              do {
                initialResultLength = own.size();

                for (int i = 0; i <= current; i++) {
                  leftRecursiveRules[i]->consume(input, queues[i]);

                  for (Piece<TResult> sub : own) {
                    if (sub.length > best_consume) {
                      result = sub;
                      best_consume = sub.length;
                    }
                  }
                }
              } while (initialResultLength < own.size());
            }
          } catch (NoExecuteDefinition&) {
            throw;
          }
        }
      out:

        delete[] queues;
      }
    } else {
      for (ref<Matchable<TResult>> rule : m_rules) {
        try {
          Piece<TResult> inner = rule->consume(input);

          if (inner) {
            if (inner.length > best_consume) {
              result = inner;
              best_consume = inner.length;
            }
          }
        } catch (NoExecuteDefinition&) {
          throw;
        }
      }
    }

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

 private:
  std::vector<ref<Matchable<TResult>>> m_rules;
  std::vector<std::function<int(const tanuki::String&)>> m_skipped;
  uint8_t m_recursiveRulesCount = 0;
};

template <typename T>
tanuki::ref<Fragment<T>> fragment() {
  return tanuki::ref<Fragment<T>>(new Fragment<T>);
}
}
