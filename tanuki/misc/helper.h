#pragma once

#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>

namespace tanuki {
template <typename T>
class OneUse {
 public:
  explicit OneUse(T *data) : m_data(data) {}

  ~OneUse() { delete m_data; }
  T exposeValue() { return (*m_data); }
  T *expose() { return m_data; }
  T *operator->() { return m_data; }

 private:
  T *m_data;
};

template <typename T>
OneUse<T> useOnce(T *data) {
  return OneUse<T>(data);
}

/**
 * Bellow Code from StackOverflow by DRayX
 * ---------------------------------------
 */

template<size_t N>
struct Apply {
    template<typename F, typename T, typename... A>
    static inline auto apply(F && f, T && t, A &&... a) {
        return Apply<N-1>::apply(::std::forward<F>(f), ::std::forward<T>(t),
            ::std::get<N-1>(::std::forward<T>(t)), ::std::forward<A>(a)...
        );
    }
};

template<>
struct Apply<0> {
    template<typename F, typename T, typename... A>
    static inline auto apply(F && f, T &&, A &&... a) {
        return ::std::forward<F>(f)(::std::forward<A>(a)...);
    }
};

template<typename F, typename T>
inline auto apply(F && f, T && t) {
    return Apply< ::std::tuple_size< ::std::decay_t<T>
      >::value>::apply(::std::forward<F>(f), ::std::forward<T>(t));
}
}
