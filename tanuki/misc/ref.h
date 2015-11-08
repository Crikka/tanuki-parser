#pragma once

#include <string>
#include <stack>
#include <utility>

#include "exception.h"

#include <iostream>
#include <typeinfo>

namespace tanuki {

#define ref_friend_operator(type, op) \
  friend ref<type> operator op(const ref<type> &, const ref<type> &)
#define ref_implement_operator(type, op)                                    \
  ref<type> operator op(const ref<type> &in1, const ref<type> &in2) {       \
    if (in1.isNull() or in2.isNull()) {                                     \
      throw NullReferenceError();                                           \
    }                                                                       \
                                                                            \
    return ref<type>(new type(*(in1.m_intern->on)op * (in2.m_intern->on))); \
  }

#define ref_friend_all_operator(type)                               \
  friend ref<type> operator+(const ref<type> &, const ref<type> &); \
  friend ref<type> operator-(const ref<type> &, const ref<type> &); \
  friend ref<type> operator*(const ref<type> &, const ref<type> &); \
  friend ref<type> operator/(const ref<type> &, const ref<type> &); \
  friend ref<type> operator+(const ref<type> &, type);              \
  friend ref<type> operator-(const ref<type> &, type);              \
  friend ref<type> operator*(const ref<type> &, type);              \
  friend ref<type> operator/(const ref<type> &, type);              \
  friend ref<type> operator+(type, const ref<type> &);              \
  friend ref<type> operator-(type, const ref<type> &);              \
  friend ref<type> operator*(type, const ref<type> &);              \
  friend ref<type> operator/(type, const ref<type> &);

#define ref_implement_all_operator(type)                                   \
  ref<type> operator+(const ref<type> &in1, const ref<type> &in2) {        \
    if (in1.isNull() or in2.isNull()) {                                    \
      throw NullReferenceError();                                          \
    }                                                                      \
                                                                           \
    return ref<type>(new type(*(in1.m_intern->on) + *(in2.m_intern->on))); \
  }                                                                        \
  ref<type> operator-(const ref<type> &in1, const ref<type> &in2) {        \
    if (in1.isNull() or in2.isNull()) {                                    \
      throw NullReferenceError();                                          \
    }                                                                      \
                                                                           \
    return ref<type>(new type(*(in1.m_intern->on) - *(in2.m_intern->on))); \
  }                                                                        \
  ref<type> operator*(const ref<type> &in1, const ref<type> &in2) {        \
    if (in1.isNull() or in2.isNull()) {                                    \
      throw NullReferenceError();                                          \
    }                                                                      \
                                                                           \
    return ref<type>(new type(*(in1.m_intern->on) * *(in2.m_intern->on))); \
  }                                                                        \
  ref<type> operator/(const ref<type> &in1, const ref<type> &in2) {        \
    if (in1.isNull() or in2.isNull()) {                                    \
      throw NullReferenceError();                                          \
    }                                                                      \
                                                                           \
    return ref<type>(new type(*(in1.m_intern->on) / *(in2.m_intern->on))); \
  }                                                                        \
  ref<type> operator+(const ref<type> &in1, type in2) {                    \
    if (in1.isNull()) {                                                    \
      throw NullReferenceError();                                          \
    }                                                                      \
                                                                           \
    return ref<type>(new type(*(in1.m_intern->on) + in2));                 \
  }                                                                        \
  ref<type> operator-(const ref<type> &in1, type in2) {                    \
    if (in1.isNull()) {                                                    \
      throw NullReferenceError();                                          \
    }                                                                      \
                                                                           \
    return ref<type>(new type(*(in1.m_intern->on) - in2));                 \
  }                                                                        \
  ref<type> operator*(const ref<type> &in1, type in2) {                    \
    if (in1.isNull()) {                                                    \
      throw NullReferenceError();                                          \
    }                                                                      \
                                                                           \
    return ref<type>(new type(*(in1.m_intern->on) * in2));                 \
  }                                                                        \
  ref<type> operator/(const ref<type> &in1, type in2) {                    \
    if (in1.isNull()) {                                                    \
      throw NullReferenceError();                                          \
    }                                                                      \
                                                                           \
    return ref<type>(new type(*(in1.m_intern->on) / in2));                 \
  }                                                                        \
  ref<type> operator+(type in1, const ref<type> &in2) {                    \
    if (in2.isNull()) {                                                    \
      throw NullReferenceError();                                          \
    }                                                                      \
                                                                           \
    return ref<type>(new type(in1 + *(in2.m_intern->on)));                 \
  }                                                                        \
  ref<type> operator-(type in1, const ref<type> &in2) {                    \
    if (in2.isNull()) {                                                    \
      throw NullReferenceError();                                          \
    }                                                                      \
                                                                           \
    return ref<type>(new type(in1 - *(in2.m_intern->on)));                 \
  }                                                                        \
  ref<type> operator*(type in1, const ref<type> &in2) {                    \
    if (in2.isNull()) {                                                    \
      throw NullReferenceError();                                          \
    }                                                                      \
                                                                           \
    return ref<type>(new type(in1 * *(in2.m_intern->on)));                 \
  }                                                                        \
  ref<type> operator/(type in1, const ref<type> &in2) {                    \
    if (in2.isNull()) {                                                    \
      throw NullReferenceError();                                          \
    }                                                                      \
                                                                           \
    return ref<type>(new type(in1 / *(in2.m_intern->on)));                 \
  }

template <typename>
class ref;

template <bool, typename>
struct DeepTypeIdentifier {};

template <typename TOn>
class ref {
 private:
  struct has_deep_type {
    template <typename T>
    static std::true_type test(typename T::TReturnType *) {
      return std::true_type();
    }

    template <typename>
    static std::false_type test(...) {
      return std::false_type();
    }

    typedef decltype(test<TOn>(nullptr)) type;
  };

  struct Intern {
    TOn *on;
    unsigned int count;
  };

  enum State : char { normal = 0, master = 1, slave = 2 };

 public:
  typedef typename DeepTypeIdentifier<has_deep_type::type::value, TOn>::type
      TDeepType;
  typedef TOn TValue;


  ref() : m_intern(nullptr), m_state(normal) {}

  ref(TOn *on) : m_state(normal) {
    this->m_intern = new Intern();

    this->m_intern->on = on;
    this->m_intern->count = 1;
  }

  ref(const ref<TOn> &other) : m_intern(other.m_intern) {
    this->m_state = (other.m_state == master ? slave : normal);

    if (!isNull()) {
      this->m_intern->count++;
    }
  }

  ref(ref<TOn> &&other) : m_intern(other.m_intern) {
    this->m_state = (other.m_state == master ? slave : normal);

    if (!isNull()) {
      this->m_intern->count++;
    }
  }

  ref<TOn> &operator=(const ref<TOn> &other) {
    this->m_state = (other.m_state == master ? slave : normal);

    if (isNull()) {
      if (!other.isNull()) {
        this->m_intern = other.m_intern;

        this->m_intern->count++;
      }
    } else {
      m_intern->count--;

      if (m_intern != other.m_intern) {
        if (m_intern->count == 0) {
          delete m_intern->on;
          delete m_intern;
          m_intern = nullptr;
        }

        m_intern = other.m_intern;
      }

      if (m_intern != nullptr) {
        m_intern->count++;
      }
    }

    return *this;
  }

  explicit operator TOn *() { return this->release(); }

  virtual ~ref() {
    if (m_state != slave && !isNull()) {
      m_intern->count--;

      if ((m_intern->count == 0) || (m_state == master)) {
        delete m_intern->on;
        delete m_intern;
      }
    }
  }

  TOn *operator->() const { return m_intern->on; }
  bool isNull() const { return (m_intern == nullptr); }
  TOn *release() {
    if (isNull()) {
      return nullptr;
    } else {
      TOn *res = m_intern->on;
      this->m_intern = nullptr;

      return res;
    }
  }

  operator bool() const { return (!isNull()); }

  bool greedy() {
    if (this->isNull()) {
      return false;
    } else {
      return this->expose()->greedy();
    }
  }

  bool stopAtFirstGreedyFail() {
    if (this->isNull()) {
      return false;
    } else {
      return this->expose()->stopAtFirstGreedyFail();
    }
  }

  int exactSize() {
    if (this->isNull()) {
      return -1;
    } else {
      return this->expose()->exactSize();
    }
  }

  int biggestSize() {
    if (this->isNull()) {
      return -1;
    } else {
      return this->expose()->biggestSize();
    }
  }

 protected:
  TOn *expose() { return (m_intern->on); }

 private:
  Intern *m_intern;
  State m_state;

  ref_friend_all_operator(int);
  ref_friend_all_operator(float);
  ref_friend_all_operator(double);
  ref_friend_all_operator(long);
  ref_friend_all_operator(long long);

  template <typename T>
  friend T *dereference(ref<T>);

  template <typename T>
  friend ref<T> autoref(T *);

  template <typename T>
  friend void master(ref<T> &);
};

template <typename TOn>
struct DeepTypeIdentifier<true, TOn> {
  typedef ref<typename TOn::TReturnType> type;
};

template <typename TOn>
struct DeepTypeIdentifier<false, TOn> {
  typedef void type;
};

ref<int> operator"" _ref(unsigned long long int in);
ref<std::string> operator"" _ref(const char *in);
ref<double> operator"" _ref(long double in);
ref<char> operator"" _ref(char in);

template <typename T>
T *dereference(ref<T> ref) {
  if (ref.isNull()) {
    throw NullReferenceError();
  }

  return ref.m_intern->on;
}

template <typename T>
ref<T> autoref(T *data) {
  ref<T> result(data);
  result.m_intern->count++;

  return result;
}

template <typename T>
void master(ref<T> &ref) {
  ref.m_state = tanuki::ref<T>::State::master;
}

template <typename TReturn>
struct Piece {
  operator bool() { return ((bool) result); }

  uint32_t length;
  ref<TReturn> result;
};

template <typename TToken>
struct Optional {
  operator bool() { return ((bool) token); }

  TToken token;
};
}
