#pragma once

#include <string>

namespace tanuki {
class String {
 public:
  String();
  String(const char *data);
  String(const std::string &other);
  String(const String &other);
  ~String();
  char operator[](int index) const;
  String substr(int from, int length = -1) const;
  int size() const;
  bool empty() const;
  std::string toStdString() const;

  String &operator=(const String &other);
  bool operator==(const std::string &other) const;

 private:
  struct Shared {
      int m_count;
      char *m_data;
  };

  Shared *m_shared;
  int m_length;
  bool m_master;
};
}
