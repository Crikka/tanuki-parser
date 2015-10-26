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

  virtual tanuki::ref<TResult> match(const tanuki::String&) = 0;
  virtual tanuki::Piece<TResult> consume(const tanuki::String&) = 0;

  short weight;
};

template <typename TResult, typename... TRefs>
class Rule : public Matchable<TResult> {
 public:
  Rule(Fragment<TResult>* context, TRefs... refs,
       std::function<ref<TResult>(typename TRefs::TDeepType...)> callback)
      : Matchable<TResult>(),
        m_context(context),
        m_refs(this, tanuki::String(), refs...),
        m_callbackByExpansion(callback) {}

  Rule(Fragment<TResult>* context, TRefs... refs,
       std::function<ref<TResult>(std::tuple<typename TRefs::TDeepType...>)>
           callback)
      : Matchable<TResult>(),
        m_context(context),
        m_refs(this, tanuki::String(), refs...),
        m_callbackByTuple(callback) {}

  tanuki::ref<TResult> match(const tanuki::String& in) override {
    std::get<1>(m_refs) = in;

    return tanuki::apply(static_resolve, m_refs);
  }

  tanuki::Piece<TResult> consume(const tanuki::String& in) override {
    std::get<1>(m_refs) = in;

    return tanuki::apply(static_resolve_consume, m_refs);
  }

 private:
  template <typename TRef, typename... TRestRef>
  struct IsRefCallback {
    static tanuki::ref<TResult> callback(Rule<TResult, TRefs...>* rule,
                                         const tanuki::String& in, TRef ref,
                                         TRestRef... rest) {
      tanuki::String skippedIn = in;

      uint32_t toSkip = rule->m_context->shouldSkip(skippedIn);
      while (toSkip > 0) {
        skippedIn = skippedIn.substr(toSkip);

        toSkip = rule->m_context->shouldSkip(skippedIn);
      }

      auto consumeed = ref->consume(skippedIn);

      if (consumeed.result) {
        try {
          tanuki::ref<TResult> result =
              rule->resolve<TRestRef..., typename TRef::TDeepType>(
                  skippedIn.substr(consumeed.length), rest...,
                  consumeed.result);

          if (result) {
            return result;
          }
        } catch (NoExecuteDefinition&) {
          throw;
        }
      }

      return tanuki::ref<TResult>();
    }
  };

  template <typename TRef, typename... TRestRef>
  struct IsNullCallback {
    static tanuki::ref<TResult> callback(Rule<TResult, TRefs...>* rule,
                                         const tanuki::String& in, TRef,
                                         TRestRef... rest) {
      tanuki::String skippedIn = in;

      if (!skippedIn.empty()) {
        uint32_t toSkip = rule->m_context->shouldSkip(skippedIn);

        while (toSkip > 0) {
          skippedIn = skippedIn.substr(toSkip);

          toSkip = rule->m_context->shouldSkip(skippedIn);
        }
      }

      if (skippedIn.empty()) {
        if (rule->m_callbackByExpansion) {
          return rule->m_callbackByExpansion(rest...);
        } else if (rule->m_callbackByTuple) {
          return rule->m_callbackByTuple(std::make_tuple(rest...));
        } else {
          throw NoExecuteDefinition();
        }
      } else {  // We don't parse all the
        return tanuki::ref<TResult>();
      }
    }
  };

  template <typename TRef, typename... TRestRef>
  struct IsRefCallbackPiece {
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

      auto consumeed = ref->consume(skippedIn);

      if (consumeed.result) {
        try {
          result = rule->resolve_consume<TRestRef..., typename TRef::TDeepType>(
              skippedIn.substr(consumeed.length), initialSize, rest...,
              consumeed.result);
        } catch (NoExecuteDefinition&) {
          throw;
        }
      }

      return result;
    }
  };

  template <typename TRef, typename... TRestRef>
  struct IsNullCallbackPiece {
    static tanuki::Piece<TResult> callback(Rule<TResult, TRefs...>* rule,
                                           const tanuki::String& in,
                                           uint32_t initialSize, TRef,
                                           TRestRef... rest) {
      tanuki::Piece<TResult> result;

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

  static tanuki::ref<TResult> static_resolve(Rule<TResult, TRefs...>* rule,
                                             const tanuki::String& in,
                                             TRefs... refs) {
    return rule->resolve(in, refs..., nullptr);
  }

  static tanuki::Piece<TResult> static_resolve_consume(
      Rule<TResult, TRefs...>* rule, const tanuki::String& in, TRefs... refs) {
    return rule->resolve_consume(in, in.size(), refs..., nullptr);
  }

  template <typename TRef, typename... TRestRef>
  tanuki::ref<TResult> resolve(const tanuki::String& in, TRef ref,
                               TRestRef... rest) {
    typedef typename if_<std::is_same<TRef, std::nullptr_t>::value,
                         IsNullCallback<TRef, TRestRef...>,
                         IsRefCallback<TRef, TRestRef...>>::result result;

    return result::callback(this, in, ref, rest...);
  }

  template <typename TRef, typename... TRestRef>
  tanuki::Piece<TResult> resolve_consume(const tanuki::String& in,
                                         uint32_t initialSize, TRef ref,
                                         TRestRef... rest) {
    typedef typename if_<std::is_same<TRef, std::nullptr_t>::value,
                         IsNullCallbackPiece<TRef, TRestRef...>,
                         IsRefCallbackPiece<TRef, TRestRef...>>::result result;

    return result::callback(this, in, initialSize, ref, rest...);
  }

  std::function<ref<TResult>(typename TRefs::TDeepType...)>
      m_callbackByExpansion;
  std::function<ref<TResult>(std::tuple<typename TRefs::TDeepType...>)>
      m_callbackByTuple;
  std::tuple<Rule<TResult, TRefs...>*, tanuki::String, TRefs...> m_refs;
  Fragment<TResult>* m_context;
};
}
