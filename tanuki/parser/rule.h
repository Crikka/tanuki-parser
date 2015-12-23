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
};

template <typename TResult, typename... TRefs>
class Rule : public Matchable<TResult> {
 public:
  Rule(Fragment<TResult>* context, TRefs... refs,
       std::function<ref<TResult>(typename TRefs::TDeepType...)> callback)
      : Matchable<TResult>(),
        m_context(context),
        m_refs(refs...),
        m_callbackByExpansion(callback) {}

  Rule(Fragment<TResult>* context, TRefs... refs,
       std::function<ref<TResult>(std::tuple<typename TRefs::TDeepType...>)>
           callback)
      : Matchable<TResult>(),
        m_context(context),
        m_refs(refs...),
        m_callbackByTuple(callback) {}

  tanuki::Piece<TResult> consume(const tanuki::String& in) override {
    return Resolver<sizeof...(TRefs)>::callback(this, in, in.size());
  }

  template <size_t N, typename... TRefsResults>
  struct Resolver {
    static tanuki::Piece<TResult> callback(Rule<TResult, TRefs...>* rule,
                                           const tanuki::String& in,
                                           uint32_t initialSize,
                                           TRefsResults... results) {
      constexpr size_t current_ref = sizeof...(TRefsResults);

      tanuki::Piece<TResult> result{0, tanuki::ref<TResult>()};

      tanuki::String skippedIn = in;

      uint32_t toSkip = rule->m_context->shouldSkip(skippedIn);

      while (toSkip > 0) {
        skippedIn = skippedIn.substr(toSkip);

        toSkip = rule->m_context->shouldSkip(skippedIn);
      }

      auto consumed = std::get<current_ref>(rule->m_refs)->consume(skippedIn);

      if (consumed) {
        try {
          typedef std::tuple<TRefs...> TupleRefs;
          typedef typename std::tuple_element<current_ref,
                                              TupleRefs>::type::TDeepType NType;

          result = Resolver<N - 1, TRefsResults..., NType>::callback(
              rule, skippedIn.substr(consumed.length), initialSize, results...,
              consumed.result);

        } catch (NoExecuteDefinition&) {
          throw;
        }
      }

      return result;
    }
  };

  template <typename... TRefsResults>
  struct Resolver<0, TRefsResults...> {
    static tanuki::Piece<TResult> callback(Rule<TResult, TRefs...>* rule,
                                           const tanuki::String& in,
                                           uint32_t initialSize,
                                           TRefsResults... results) {
      tanuki::String skippedIn = in;

      if (rule->m_context->skipAtEnd) {
        uint32_t toSkip = rule->m_context->shouldSkip(skippedIn);

        while (toSkip > 0) {
          skippedIn = skippedIn.substr(toSkip);

          toSkip = rule->m_context->shouldSkip(skippedIn);
        }
      }

      Piece<TResult> result;

      if (rule->m_callbackByExpansion) {
        result = Piece<TResult>{initialSize - skippedIn.size(),
                                rule->m_callbackByExpansion(results...)};
      } else if (rule->m_callbackByTuple) {
        result = Piece<TResult>{
            initialSize - skippedIn.size(),
            rule->m_callbackByTuple(std::make_tuple(results...))};
      } else {
        throw NoExecuteDefinition();
      }

      return result;
    }
  };

  void consume(const tanuki::String& in,
               Yielder<Piece<TResult>>& results) override {
    ResolverLeftRecursive<HasFirstRef<TRefs...>::result::value>::resolve(
        this, in, &results);
  }

  template <typename TFirst, typename...>
  struct HasFirstRef {
    typedef typename std::is_same<typename TFirst::TDeepType::TValue,
                                  TResult>::type result;
  };

  template <bool, typename... TConsumeRefs>
  struct ResolverLeftRecursive {};

  template <typename... TConsumeRefs>
  struct ResolverLeftRecursive<true, TConsumeRefs...> {
    static void resolve(Rule<TResult, TRefs...>* rule, const tanuki::String& in,
                        Yielder<Piece<TResult>>* results) {
      typedef std::tuple<TRefs...> TupleRefs;
      typedef typename std::tuple_element<0, TupleRefs>::type::TDeepType NType;
      constexpr size_t length = sizeof...(TRefs)-1;

      std::vector<Piece<TResult>> subs;

      do {
        subs.clear();
        for (Piece<TResult> result : *results) {
          Piece<TResult> sub = Resolver<length, NType>::callback(
              rule, in.substr(result.length), in.size(), result.result);

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
  struct ResolverLeftRecursive<false, TConsumeRefs...> {
    static void resolve(Rule<TResult, TRefs...>*, const tanuki::String&,
                        Yielder<Piece<TResult>>*,
                        TConsumeRefs...) { /* Resolve compilation */
    }
  };

  std::function<ref<TResult>(typename TRefs::TDeepType...)>
      m_callbackByExpansion;
  std::function<ref<TResult>(std::tuple<typename TRefs::TDeepType...>)>
      m_callbackByTuple;
  std::tuple<TRefs...> m_refs;
  Fragment<TResult>* m_context;
};
}
