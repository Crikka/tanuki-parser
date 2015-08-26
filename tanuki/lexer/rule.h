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

 private:
  template <typename TRef, typename... TRestRef>
  struct IsRefCallback {
    tanuki::ref<TResult> operator()(Rule<TResult, TRefs...>* rule,
                                    const std::string& in, TRef ref,
                                    TRestRef... rest) {
      int length = in.size();
      int maxSizeMatch;
      int start = 0;

      for (int i = 1; i <= length; i++) {
        std::string buffer(in.substr(start, i));

        if (rule->m_context->shouldIgnore(buffer)) {
          start += i;
          length = (in.size() - start);
          i = 0;
        } else {
          auto res = ref->match(buffer);

          if (res) {
            bool greedy = ref->greedy();
            maxSizeMatch = i;

            if (greedy) {
              auto keep = res;
              bool stopAtFirstGreedyFail = ref->stopAtFirstGreedyFail();

              // Greedy time
              i++;
              while (i <= length) {
                buffer = (in.substr(start, i));
                res = ref->match(buffer);
                if (res) {
                  keep = res;
                  maxSizeMatch = i;
                } else if (stopAtFirstGreedyFail) {
                  break;
                }

                i++;
              };

              res = keep;
            }

            try {
              return rule->resolve<TRestRef..., typename TRef::TDeepType>(
                  in.substr(start + maxSizeMatch), rest..., res);
            } catch (NoExecuteDefinition&) {
              throw;
            }
          }
        }
      }

      return tanuki::ref<TResult>();
    }
  };

  template <typename TRef, typename... TRestRef>
  struct IsNullCallback {
    tanuki::ref<TResult> operator()(Rule<TResult, TRefs...>* rule,
                                    const std::string& in, TRef ref,
                                    TRestRef... rest) {
      if (in.empty() || rule->m_context->shouldIgnore(in)) {
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

  static tanuki::ref<TResult> static_resolve(Rule<TResult, TRefs...>* rule,
                                             const std::string& in,
                                             TRefs... refs) {
    return rule->resolve(in, refs..., nullptr);
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

  std::function<ref<TResult>(typename TRefs::TDeepType...)>
      m_callbackByExpansion;
  std::function<ref<TResult>(std::tuple<typename TRefs::TDeepType...>)>
      m_callbackByTuple;
  ref<Fragment<TResult>> m_context;
  std::tuple<Rule<TResult, TRefs...>*, std::string, TRefs...> m_refs;
};
}
}
