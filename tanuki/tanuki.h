#pragma once

#define use_tanuki              \
  using tanuki::constant;       \
  using tanuki::integer;        \
  using tanuki::startWith;      \
  using tanuki::endWith;        \
  using tanuki::range;          \
  using tanuki::anyOf;          \
  using tanuki::anyIn;          \
  using tanuki::word;           \
  using tanuki::space;          \
  using tanuki::tab;            \
  using tanuki::blank;          \
  using tanuki::lineTerminator; \
  using tanuki::digit;          \
  using tanuki::letter;         \
  using tanuki::fragment;       \
                                \
  using tanuki::Fragment;       \
                                \
  using tanuki::ref;            \
  using tanuki::dereference;    \
  using tanuki::autoref;        \
  using tanuki::master;         \
                                \
  using tanuki::operator"" _ref;

#include <iostream>

#include "parser/parser.h"
#include "exception"

namespace tanuki {}
