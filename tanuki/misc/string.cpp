#include "string.h"

#include <cstring>
#include <cstdlib>

#include <iostream>

namespace tanuki {
String::String() {
  this->m_master = true;
  this->m_length = 0;
  this->m_shared = new Shared();
  this->m_shared->m_count = 1;
}

String::String(const char* data) {
  this->m_master = true;
  this->m_length = strlen(data);
  this->m_shared = new Shared();
  this->m_shared->m_count = 1;

  if (m_length > 0) {
    this->m_shared->m_data = (char*)malloc(sizeof(char) * m_length);
    memcpy(m_shared->m_data, data, m_length);
  }
}

String::String(const String& other) {
  this->m_master = other.m_master;
  this->m_length = other.m_length;

  this->m_shared = other.m_shared;
  this->m_shared->m_count++;
}

String::~String() {
  m_shared->m_count--;

  if (m_shared->m_count == 0) {
    if (m_master && (m_length > 0)) {
      free(m_shared->m_data);
    }

    delete m_shared;
  }
}

char String::operator[](int index) const { return m_shared->m_data[index]; }

String String::substr(int from, int length) const {
  if (length == -1) {
    length = m_length;
  }

  length -= from;

  String result;
  result.m_master = false;
  result.m_length = length;

  if (length > 0) {
    result.m_shared->m_data = (m_shared->m_data + from);
  }

  return result;
}

int String::size() const { return m_length; }

bool String::empty() const { return m_length == 0; }

std::string String::toStdString() const {
  char data[m_length + 1];

  if (m_length > 0) {
    memcpy(data, m_shared->m_data, m_length);
  }

  data[m_length] = '\0';

  return std::string(data);
}

String& String::operator=(const String& other) {
  m_shared->m_count--;

  if (m_shared->m_count == 0) {
    if (m_master && (m_length > 0)) {
      free(m_shared->m_data);
    }

    delete m_shared;
  }

  this->m_master = other.m_master;
  this->m_length = other.m_length;
  this->m_shared = other.m_shared;
  this->m_shared->m_count++;

  return *this;
}

bool String::operator==(const std::string& other) const {
  if (m_length == 0) {
    return other.empty();
  }

  return !memcmp(m_shared->m_data, other.c_str(), m_length);
}
}
