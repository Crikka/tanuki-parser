#pragma once

#include <string>

#include "exception.h"

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

template <typename TOn>
class ref {
 public:
  typedef TOn TValue;

 private:
  struct Intern {
    TOn *on;
    unsigned int count;
  };

 public:
  ref() : m_intern(nullptr) {}

  ref(TOn *on) {
    this->m_intern = new Intern();

    this->m_intern->on = on;
    this->m_intern->count = 1;
  }

  ref(const ref<TOn> &other) : m_intern(other.m_intern) {
    this->m_intern->count++;
  }
  ref(ref<TOn> &&other) : m_intern(other.m_intern) {
    if (!isNull()) {
      this->m_intern->count++;
    }
  }
  ref<TOn> &operator=(const ref<TOn> &other) {
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

  ~ref() {
    if (!isNull()) {
      m_intern->count--;
      if (m_intern->count == 0) {
        delete m_intern->on;
        delete m_intern;
        m_intern = nullptr;
      }
    }
  }

  TOn *operator->() const { return m_intern->on; }
  bool isNull() const { return (m_intern == nullptr); }
  TOn *release() {
    TOn *res = m_intern->on;
    this->m_intern = nullptr;
    return res;
  }

  operator bool() const { return (!isNull()); }
  bool operator==(TOn other) {
    if (isNull()) {
      return false;
    }

    return (*(m_intern->on) == other);
  }

 protected:
  TOn *expose() { return (m_intern->on); }

 private:
  Intern *m_intern;

  ref_friend_all_operator(int);
  ref_friend_all_operator(float);
  ref_friend_all_operator(double);
  ref_friend_all_operator(long);
  ref_friend_all_operator(long long);
};

ref_implement_all_operator(int);
ref_implement_all_operator(float);
ref_implement_all_operator(double);
ref_implement_all_operator(long);
ref_implement_all_operator(long long);

template <typename TOn>
class undirect_ref : public ref<TOn> {
 public:
  typedef ref<typename TOn::TReturnType> TDeepType;
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

 public:
  undirect_ref() : ref<TOn>() {}

  undirect_ref(TOn *on) : ref<TOn>(on) {}
};

ref<int> operator"" _ref(unsigned long long int in) {
  return ref<int>(new int(in));
}

ref<std::string> operator"" _ref(const char *in) {
  return ref<std::string>(new std::string(in));
}

ref<double> operator"" _ref(long double in) {
  return ref<double>(new double(in));
}
}
