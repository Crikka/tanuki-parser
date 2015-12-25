#pragma once

#include <functional>
#include <vector>
#include <tuple>

#include "tanuki/misc/misc.h"

namespace tanuki {
template <typename TResult>
class Fragment;

template <bool, typename TResult, typename... TRefs>
struct ResolverLeftRecursive {
  static void resolve(...) {}
};

template <size_t N, typename TResult, typename... TRefs>
struct Resolver;

template <typename TResult, typename... TRefs>
struct MetaInfo {
 private:
  template <size_t, typename IResult, typename... IRefs>
  struct Info {
    typedef std::tuple<IRefs...> TupleRefs;
    typedef typename std::tuple_element<0, TupleRefs>::type FirstElement;
    typedef typename FirstElement::TDeepType::TValue FirstValue;

    typedef typename std::is_same<FirstValue, IResult>::type BeginWith;
    typedef typename FirstElement::TDeepType FType;
  };

  template <typename IResult, typename... IRefs>
  struct Info<0, IResult, IRefs...> {
    typedef std::false_type BeginWith;
    typedef void FType;
  };

  typedef Info<sizeof...(TRefs), TResult, TRefs...> info;

 public:
  typedef typename info::BeginWith BeginWith;
  typedef typename info::FType FType;
};

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
 private:
  typedef MetaInfo<TResult, TRefs...> Info;

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
    return Resolver<sizeof...(TRefs), TResult, TRefs...>::callback(this, in,
                                                                   in.size());
  }

  void consume(const tanuki::String& in,
               Yielder<Piece<TResult>>& results) override {
    ResolverLeftRecursive<Info::BeginWith::value, TResult, TRefs...>::resolve(
        this, in, &results);
  }

 private:
  std::function<ref<TResult>(typename TRefs::TDeepType...)>
      m_callbackByExpansion;
  std::function<ref<TResult>(std::tuple<typename TRefs::TDeepType...>)>
      m_callbackByTuple;
  std::tuple<TRefs...> m_refs;
  Fragment<TResult>* m_context;

  template <size_t, typename, typename...>
  friend struct Resolver;
};

template <typename TResult, typename... TRefs>
struct ResolverLeftRecursive<true, TResult, TRefs...> {
 private:
  typedef MetaInfo<TResult, TRefs...> Info;

 public:
  static void resolve(Rule<TResult, TRefs...>* rule, const tanuki::String& in,
                      Yielder<Piece<TResult>>* results) {
    constexpr size_t length = sizeof...(TRefs)-1;

    std::vector<Piece<TResult>> subs;

    do {
      subs.clear();
      for (Piece<TResult> result : *results) {
        Piece<TResult> sub =
            Resolver<length, TResult, TRefs...>::callback(
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

template <size_t N, typename TResult, typename... TRefs>
struct Resolver {
  template <typename... TRefsResults>
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

        result = Resolver<N - 1, TResult, TRefs...>::callback(
            rule, skippedIn.substr(consumed.length), initialSize, results...,
            consumed.result);

      } catch (NoExecuteDefinition&) {
        throw;
      }
    }

    return result;
  }
};

template <typename TResult, typename... TRefs>
struct Resolver<0, TResult, TRefs...> {
  template <typename... TRefsResults>
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
      result =
          Piece<TResult>{initialSize - skippedIn.size(),
                         rule->m_callbackByTuple(std::make_tuple(results...))};
    } else {
      throw NoExecuteDefinition();
    }

    return result;
  }
};
}
