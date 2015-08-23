#pragma once


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
}
