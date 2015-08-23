#pragma once

namespace tanuki {
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
  ref(ref<TOn> &&other) : m_intern(other.m_intern) { this->m_intern->count++; }
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
        if (m_intern != nullptr) {
          m_intern->count++;
        }
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

 private:
  Intern *m_intern;

  template <typename T>
  friend bool operator==(bool, const ref<T> &ref);
  template <typename T>
  friend bool operator==(const ref<T> &ref, bool);
  template <typename T>
  friend bool operator==(T, const ref<T> &ref);
  template <typename T>
  friend bool operator==(const ref<T> &ref, T);
};

template <typename TOn>
bool operator==(bool member1, const ref<TOn> &ref) {
  return (member1 != ref.isNull());
}

template <typename TOn>
bool operator==(const ref<TOn> &ref, bool member2) {
  return (ref.isNull() != member2);
}

template <typename TOn>
bool operator==(TOn member1, const ref<TOn> &ref) {
  if (ref.isNull()) {
    return false;
  }

  return (member1 == *(ref.m_intern->on));
}

template <typename TOn>
bool operator==(const ref<TOn> &ref, TOn member2) {
  if (ref.isNull()) {
    return false;
  }

  return (*(ref.m_intern->on) == member2);
}
}
