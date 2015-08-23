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
class Rule : public std::function<tanuki::ref<TResult>(std::string&)> {
 public:
  ref<Fragment<TResult>> &execute(std::function<TResult*(TArgs...)> callback) {
    this->m_callbackByExpansion = callback;

    return m_context;
  }
  ref<Fragment<TResult>> &execute(
      std::function<TResult*(std::tuple<TArgs...>)> callback) {
    this->m_callbackByTuple = callback;

    return m_context;
  }

  // private:
  Rule() : std::function<ref<TResult>(std::string&)>() {}

  template <typename TRef, typename... TRestRef>
  static Rule<TResult, typename TRef::TValue::TReturnValue,
              typename TRestRef::TValue::TReturnValue...>*
  create(tanuki::ref<Fragment<TResult>> context, TRef& ref, TRestRef&... rest) {
    Rule<TResult, typename TRef::TValue::TReturnValue,
         typename TRestRef::TValue::TReturnValue...>* rule =
        new Rule<TResult, typename TRef::TValue::TReturnValue,
                 typename TRestRef::TValue::TReturnValue...>();

    std::function<tanuki::ref<TResult>(std::string&)> buffer =
        [=](std::string& in)
            -> tanuki::ref<TResult> { return rule->resolve(in, ref, rest..., nullptr); };

    rule->swap(buffer);
    rule->m_context = context;

    return rule;
  }

  template <typename TRef, typename... TRestRef>
  struct IsRefCallback {
    tanuki::ref<TResult> operator()(Rule<TResult, TArgs...>* rule, const std::string& in,
                       TRef ref, TRestRef... rest) {
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

          if (res == true) {
            auto keep = res;
            maxSizeMatch = 1;

            // Greedy time
            i++;
            while (i <= length) {
              buffer = (in.substr(start, i));
              res = ref->match(buffer);
              if (res == true) {
                keep = res;
                maxSizeMatch = i;
              } else {
                res = keep;
                break;
              }

              i++;
            };

            try {
              return rule->resolve<TRestRef...>(
                  in.substr(start + maxSizeMatch), rest...,
                  useOnce(res.release()).exposeValue());
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
    tanuki::ref<TResult> operator()(Rule<TResult, TArgs...>* rule, const std::string& in,
                       TRef ref, TRestRef... rest) {
      if (rule->m_callbackByExpansion) {
        return tanuki::ref<TResult>(rule->m_callbackByExpansion(rest...));
      } else if (rule->m_callbackByTuple) {
        return tanuki::ref<TResult>(rule->m_callbackByTuple(std::make_tuple(rest...)));
      } else {
        throw NoExecuteDefinition();
      }
    }
  };

  template <typename TRef, typename... TRestRef>
  tanuki::ref<TResult> resolve(const std::string& in, TRef ref, TRestRef... rest) {
    typedef typename if_<std::is_same<TRef, std::nullptr_t>::value,
                         IsNullCallback<TRef, TRestRef...>,
                         IsRefCallback<TRef, TRestRef...>>::result Callback;

    return Callback()(this, in, ref, rest...);
  }

  std::function<TResult*(TArgs...)> m_callbackByExpansion;
  std::function<TResult*(std::tuple<TArgs...>)> m_callbackByTuple;
  ref<Fragment<TResult>> m_context;
};
}
}
