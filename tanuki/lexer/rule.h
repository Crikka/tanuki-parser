#pragma once

#include <functional>
#include <vector>
#include <tuple>

#include "tanuki/misc/misc.h"

namespace tanuki {
namespace lexer {
template <typename>
class Fragment;

template <typename TResult>
class Matchable {
 public:
  virtual tanuki::ref<TResult> match(const std::string&) = 0;
  virtual tanuki::Collect<TResult> collect(const std::string&) = 0;
};

template <typename TResult, typename... TRefs>
class Rule : public Matchable<TResult> {
 public:
  Rule(tanuki::ref<Fragment<TResult>> context, TRefs... refs,
       std::function<ref<TResult>(typename TRefs::TDeepType...)> callback)
      : Matchable<TResult>(),
        m_context(context),
        m_refs(this, std::string(), refs...),
        m_callbackByExpansion(callback) {}

  Rule(tanuki::ref<Fragment<TResult>> context, TRefs... refs,
       std::function<ref<TResult>(std::tuple<typename TRefs::TDeepType...>)>
           callback)
      : Matchable<TResult>(),
        m_context(context),
        m_refs(this, std::string(), refs...),
        m_callbackByTuple(callback) {}

  tanuki::ref<TResult> match(const std::string& in) override {
    std::get<1>(m_refs) = in;

    return tanuki::apply(static_resolve, m_refs);
  }

  tanuki::Collect<TResult> collect(const std::string& in) override {
    std::get<1>(m_refs) = in;

    return tanuki::apply(static_resolve_collect, m_refs);
  }

 private:
  template <typename TRef, typename... TRestRef>
  struct IsRefCallback {
    tanuki::ref<TResult> operator()(Rule<TResult, TRefs...>* rule,
                                    const std::string& in, TRef ref,
                                    TRestRef... rest) {
      std::string skippedIn = in;

      int start;

      do {
        start = 0;

        for (int i = 0; i < skippedIn.size(); i++) {
          if (rule->m_context->shouldSkip(skippedIn.substr(0, i))) {
            start = i;
            break;
          }
        }

        skippedIn = skippedIn.substr(start);
      } while (start != 0);

      auto collected = ref->collect(skippedIn);

      while (!collected.empty()) {
        auto& last = collected.top();

        try {
          tanuki::ref<TResult> result =
              rule->resolve<TRestRef..., typename TRef::TDeepType>(
                  skippedIn.substr(last.first), rest..., last.second);

          if (result) {
            return result;
          }
        } catch (NoExecuteDefinition&) {
          throw;
        }

        collected.pop();
      }

      return tanuki::ref<TResult>();
    }
  };

  template <typename TRef, typename... TRestRef>
  struct IsNullCallback {
    tanuki::ref<TResult> operator()(Rule<TResult, TRefs...>* rule,
                                    const std::string& in, TRef ref,
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
    tanuki::Collect<TResult> operator()(Rule<TResult, TRefs...>* rule,
                                        const std::string& in, int initialSize,
                                        TRef ref, TRestRef... rest) {
      tanuki::Collect<TResult> result;

      std::string skippedIn = in;

      int start;

      do {
        start = 0;

        for (int i = 0; i < skippedIn.size(); i++) {
          if (rule->m_context->shouldSkip(skippedIn.substr(0, i))) {
            start = i;
            break;
          }
        }

        skippedIn = skippedIn.substr(start);
      } while (start != 0);

      auto collected = ref->collect(skippedIn);

      while (!collected.empty()) {
        auto& last = collected.top();
        try {
          tanuki::Collect<TResult> followes =
              rule->resolve_collect<TRestRef..., typename TRef::TDeepType>(
                  skippedIn.substr(last.first), initialSize, rest...,
                  last.second);

          while (!followes.empty()) {
            const std::pair<int, tanuki::ref<TResult>>& follow = followes.top();
            result.push(follow);

            followes.pop();
          }
        } catch (NoExecuteDefinition&) {
          throw;
        }

        collected.pop();
      }

      return result;
    }
  };

  template <typename TRef, typename... TRestRef>
  struct IsNullCallbackCollect {
    tanuki::Collect<TResult> operator()(Rule<TResult, TRefs...>* rule,
                                        const std::string& in, int initialSize,
                                        TRef, TRestRef... rest) {
      tanuki::Collect<TResult> result;

      if (rule->m_callbackByExpansion) {
        result.push(std::make_pair(initialSize - in.size(),
                                   rule->m_callbackByExpansion(rest...)));
      } else if (rule->m_callbackByTuple) {
        result.push(
            std::make_pair(initialSize - in.size(),
                           rule->m_callbackByTuple(std::make_tuple(rest...))));
      } else {
        throw NoExecuteDefinition();
      }

      return result;
    }
  };

  static tanuki::ref<TResult> static_resolve(Rule<TResult, TRefs...>* rule,
                                             const std::string& in,
                                             TRefs... refs) {
    return rule->resolve(in, refs..., nullptr);
  }

  static tanuki::Collect<TResult> static_resolve_collect(
      Rule<TResult, TRefs...>* rule, const std::string& in, TRefs... refs) {
    return rule->resolve_collect(in, in.size(), refs..., nullptr);
  }

  template <typename TRef, typename... TRestRef>
  tanuki::ref<TResult> resolve(const std::string& in, TRef ref,
                               TRestRef... rest) {
    return
        typename if_<std::is_same<TRef, std::nullptr_t>::value,
                     IsNullCallback<TRef, TRestRef...>,
                     IsRefCallback<TRef, TRestRef...>>::result()(this, in, ref,
                                                                 rest...);
  }

  template <typename TRef, typename... TRestRef>
  tanuki::Collect<TResult> resolve_collect(const std::string& in,
                                           int initialSize, TRef ref,
                                           TRestRef... rest) {
    return typename if_<
        std::is_same<TRef, std::nullptr_t>::value,
        IsNullCallbackCollect<TRef, TRestRef...>,
        IsRefCallbackCollect<TRef, TRestRef...>>::result()(this, in,
                                                           initialSize, ref,
                                                           rest...);
  }

  std::function<ref<TResult>(typename TRefs::TDeepType...)>
      m_callbackByExpansion;
  std::function<ref<TResult>(std::tuple<typename TRefs::TDeepType...>)>
      m_callbackByTuple;
  ref<Fragment<TResult>> m_context;
  std::tuple<Rule<TResult, TRefs...>*, std::string, TRefs...> m_refs;
};
}
}
