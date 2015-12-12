#pragma once

#include <functional>
#include <vector>
#include <tuple>

#include "tanuki/misc/misc.h"

namespace tanuki {
template <typename TResult>
class Fragment;

template <typename TResult>
class Matchable {
 public:
  virtual ~Matchable() = default;

  virtual tanuki::Piece<TResult> consume(const tanuki::String&) = 0;
  virtual void consume(const tanuki::String&,
                       Yielder<Piece<TResult>>& results) = 0;

  bool isLeftRecursive;
};

template <typename TResult, typename... TRefs>
class Rule : public Matchable<TResult> {
 public:
  Rule(Fragment<TResult>* context, TRefs... refs,
       std::function<ref<TResult>(typename TRefs::TDeepType...)> callback)
      : Matchable<TResult>(),
        m_context(context),
        m_refs(this, tanuki::String(), refs...),
        m_refs_with_left_result(this, tanuki::String(), nullptr, refs...),
        m_callbackByExpansion(callback) {}

  Rule(Fragment<TResult>* context, TRefs... refs,
       std::function<ref<TResult>(std::tuple<typename TRefs::TDeepType...>)>
           callback)
      : Matchable<TResult>(),
        m_context(context),
        m_refs(this, tanuki::String(), refs...),
        m_refs_with_left_result(this, tanuki::String(), nullptr, refs...),
        m_callbackByTuple(callback) {}

  tanuki::Piece<TResult> consume(const tanuki::String& in) override {
    std::get<1>(m_refs) = in;

    return tanuki::apply(static_resolve, m_refs);
  }

  void consume(const tanuki::String& in,
               Yielder<Piece<TResult>>& results) override {
    std::get<1>(m_refs_with_left_result) = in;
    std::get<2>(m_refs_with_left_result) = &results;

    tanuki::apply(static_resolve_left_recursive<TRefs...>,
                  m_refs_with_left_result);
  }

  template <typename TRef, typename... TRestRef>
  struct Resolver {
    static tanuki::Piece<TResult> callback(Rule<TResult, TRefs...>* rule,
                                           const tanuki::String& in,
                                           uint32_t initialSize, TRef ref,
                                           TRestRef... rest) {
      tanuki::Piece<TResult> result{0, tanuki::ref<TResult>()};

      tanuki::String skippedIn = in;

      uint32_t toSkip = rule->m_context->shouldSkip(skippedIn);

      while (toSkip > 0) {
        skippedIn = skippedIn.substr(toSkip);

        toSkip = rule->m_context->shouldSkip(skippedIn);
      }

      auto consumed = ref->consume(skippedIn);

      if (consumed) {
        try {
          result = rule->resolve<TRestRef..., typename TRef::TDeepType>(
              skippedIn.substr(consumed.length), initialSize, rest...,
              consumed.result);

        } catch (NoExecuteDefinition&) {
          throw;
        }
      }

      return result;
    }
  };

  template <typename... TRestRef>
  struct Resolver<std::nullptr_t, TRestRef...> {
    static Piece<TResult> callback(Rule<TResult, TRefs...>* rule,
                                   const tanuki::String& in,
                                   uint32_t initialSize, std::nullptr_t,
                                   TRestRef... rest) {
      Piece<TResult> result;

      if (rule->m_callbackByExpansion) {
        result = Piece<TResult>{initialSize - in.size(),
                                rule->m_callbackByExpansion(rest...)};
      } else if (rule->m_callbackByTuple) {
        result =
            Piece<TResult>{initialSize - in.size(),
                           rule->m_callbackByTuple(std::make_tuple(rest...))};
      } else {
        throw NoExecuteDefinition();
      }

      return result;
    }
  };

  static tanuki::Piece<TResult> static_resolve(Rule<TResult, TRefs...>* rule,
                                               const tanuki::String& in,
                                               TRefs... refs) {
    return rule->resolve(in, in.size(), refs..., nullptr);
  }

  template <typename TConsumeRef, typename... TConsumeRefs>
  static void static_resolve_left_recursive(Rule<TResult, TRefs...>* rule,
                                            const tanuki::String& in,
                                            Yielder<Piece<TResult>>* results,
                                            TConsumeRef, TConsumeRefs... refs) {
    StaticResolverLeftRecursive<
        std::is_same<typename TConsumeRef::TDeepType::TValue, TResult>::value,
        TConsumeRefs...>::resolve(rule, in, results, refs...);
  }

  template <bool, typename... TConsumeRefs>
  struct StaticResolverLeftRecursive {};

  template <typename... TConsumeRefs>
  struct StaticResolverLeftRecursive<true, TConsumeRefs...> {
    static void resolve(Rule<TResult, TRefs...>* rule, const tanuki::String& in,
                        Yielder<Piece<TResult>>* results,
                        TConsumeRefs... refs) {
      std::vector<Piece<TResult>> subs;
      do {
        subs.clear();
        for (Piece<TResult> result : *results) {
          Piece<TResult> sub =
              rule->resolve(in.substr(result.length), in.size(), refs...,
                            nullptr, result.result);

          if (sub) {
            subs.push_back(sub);
          }
        }

        for (Piece<TResult> sub : subs) {
          results->push(sub);
        }

      } while (!subs.empty());
    }
  };

  template <typename... TConsumeRefs>
  struct StaticResolverLeftRecursive<false, TConsumeRefs...> {
    static void resolve(Rule<TResult, TRefs...>*, const tanuki::String&,
                        Yielder<Piece<TResult>>*,
                        TConsumeRefs...) { /* Resolve compilation */
    }
  };

  template <typename TRef, typename... TRestRef>
  Piece<TResult> resolve(const tanuki::String& in, uint32_t initialSize,
                         TRef ref, TRestRef... rest) {
    return Resolver<TRef, TRestRef...>::callback(this, in, initialSize, ref,
                                                 rest...);
  }

  std::function<ref<TResult>(typename TRefs::TDeepType...)>
      m_callbackByExpansion;
  std::function<ref<TResult>(std::tuple<typename TRefs::TDeepType...>)>
      m_callbackByTuple;
  std::tuple<Rule<TResult, TRefs...>*, tanuki::String, TRefs...> m_refs;
  std::tuple<Rule<TResult, TRefs...>*, tanuki::String, Yielder<Piece<TResult>>*,
             TRefs...> m_refs_with_left_result;
  Fragment<TResult>* m_context;
};
}
