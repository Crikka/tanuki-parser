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
  virtual tanuki::Collect<TResult> collect(const tanuki::String&) = 0;

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

  tanuki::Collect<TResult> collect(const tanuki::String& in) override {
    std::get<1>(m_refs) = in;

    return tanuki::apply(static_resolve_collect, m_refs);
  }

 private:
  template <typename TRef, typename... TRestRef>
  struct IsRefCallback {
    static tanuki::ref<TResult> callback(Rule<TResult, TRefs...>* rule,
                                         const tanuki::String& in, TRef ref,
                                         TRestRef... rest) {
      tanuki::String skippedIn = in;

      int toSkip = rule->m_context->shouldSkip(skippedIn);

      while (toSkip > 0) {
        skippedIn = skippedIn.substr(toSkip);

        toSkip = rule->m_context->shouldSkip(skippedIn);
      }

      auto collected = ref->collect(skippedIn);

      if (collected.second) {
        try {
          tanuki::ref<TResult> result =
              rule->resolve<TRestRef..., typename TRef::TDeepType>(
                  skippedIn.substr(collected.first), rest..., collected.second);

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
                                         const tanuki::String& in, TRef ref,
                                         TRestRef... rest) {
      if (in.empty() || rule->m_context->shouldSkip(in)) {
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
  struct IsRefCallbackCollect {
    static tanuki::Collect<TResult> callback(Rule<TResult, TRefs...>* rule,
                                             const tanuki::String& in,
                                             int initialSize, TRef ref,
                                             TRestRef... rest) {
      tanuki::Collect<TResult> result(
          std::make_pair(0, tanuki::ref<TResult>()));

      tanuki::String skippedIn = in;

      int toSkip = rule->m_context->shouldSkip(skippedIn);

      while (toSkip > 0) {
        skippedIn = skippedIn.substr(toSkip);

        toSkip = rule->m_context->shouldSkip(skippedIn);
      }

      auto collected = ref->collect(skippedIn);

      if (collected.second) {
        try {
          result = rule->resolve_collect<TRestRef..., typename TRef::TDeepType>(
              skippedIn.substr(collected.first), initialSize, rest...,
              collected.second);
        } catch (NoExecuteDefinition&) {
          throw;
        }
      }

      return result;
    }
  };

  template <typename TRef, typename... TRestRef>
  struct IsNullCallbackCollect {
    static tanuki::Collect<TResult> callback(Rule<TResult, TRefs...>* rule,
                                             const tanuki::String& in,
                                             int initialSize, TRef,
                                             TRestRef... rest) {
      tanuki::Collect<TResult> result;

      if (rule->m_callbackByExpansion) {
        result = std::make_pair(initialSize - in.size(),
                                rule->m_callbackByExpansion(rest...));
      } else if (rule->m_callbackByTuple) {
        result =
            std::make_pair(initialSize - in.size(),
                           rule->m_callbackByTuple(std::make_tuple(rest...)));
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

  static tanuki::Collect<TResult> static_resolve_collect(
      Rule<TResult, TRefs...>* rule, const tanuki::String& in, TRefs... refs) {
    return rule->resolve_collect(in, in.size(), refs..., nullptr);
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
  tanuki::Collect<TResult> resolve_collect(const tanuki::String& in,
                                           int initialSize, TRef ref,
                                           TRestRef... rest) {
    typedef
        typename if_<std::is_same<TRef, std::nullptr_t>::value,
                     IsNullCallbackCollect<TRef, TRestRef...>,
                     IsRefCallbackCollect<TRef, TRestRef...>>::result result;

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
