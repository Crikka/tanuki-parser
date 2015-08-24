#pragma once

#include <functional>
#include <vector>
#include <tuple>

#include "tanuki/misc/misc.h"

namespace tanuki {
namespace lexer {
template <typename>
class Fragment;

template <typename TResult, typename... TArgs>
class Rule : public std::function<tanuki::ref<TResult>(const std::string&)> {
 public:
  ref<Fragment<TResult>>& execute(
      std::function<ref<TResult>(TArgs...)> callback) {
    this->m_callbackByExpansion = callback;

    return m_context;
  }
  ref<Fragment<TResult>>& execute(
      std::function<ref<TResult>(std::tuple<TArgs...>)> callback) {
    this->m_callbackByTuple = callback;

    return m_context;
  }

  template <typename TRef, typename... TRestRef>
  static Rule<TResult, typename TRef::TDeepType,
              typename TRestRef::TDeepType...>*
  create(tanuki::ref<Fragment<TResult>> context, TRef& ref, TRestRef&... rest) {
    Rule<TResult, typename TRef::TDeepType, typename TRestRef::TDeepType...>*
        rule = new Rule<TResult, typename TRef::TDeepType,
                        typename TRestRef::TDeepType...>();

    std::function<tanuki::ref<TResult>(const std::string&)> buffer =
        [=](const std::string& in) -> tanuki::ref<TResult> {
          return rule->resolve(in, ref, rest..., nullptr);
        };

    rule->swap(buffer);
    rule->m_context = context;

    return rule;
  }

 private:
  Rule() : std::function<ref<TResult>(const std::string&)>() {}

  template <typename TRef, typename... TRestRef>
  struct IsRefCallback {
    tanuki::ref<TResult> operator()(Rule<TResult, TArgs...>* rule,
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
            auto keep = res;
            maxSizeMatch = i;

            // Greedy time
            i++;
            while (i <= length) {
              buffer = (in.substr(start, i));
              res = ref->match(buffer);
              if (res) {
                keep = res;
                maxSizeMatch = i;
              } else {
                res = keep;
                break;
              }

              i++;
            };

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
    tanuki::ref<TResult> operator()(Rule<TResult, TArgs...>* rule,
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
      } else { // We don't parse all the
        return tanuki::ref<TResult>();
      }
    }
  };

  template <typename TRef, typename... TRestRef>
  tanuki::ref<TResult> resolve(const std::string& in, TRef ref,
                               TRestRef... rest) {
    return
        typename if_<std::is_same<TRef, std::nullptr_t>::value,
                     IsNullCallback<TRef, TRestRef...>,
                     IsRefCallback<TRef, TRestRef...>>::result()(this, in, ref,
                                                                 rest...);
  }

  std::function<ref<TResult>(TArgs...)> m_callbackByExpansion;
  std::function<ref<TResult>(std::tuple<TArgs...>)> m_callbackByTuple;
  ref<Fragment<TResult>> m_context;
};
}
}
