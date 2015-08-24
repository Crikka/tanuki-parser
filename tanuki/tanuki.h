#pragma once

#define use_tanuki\
  using tanuki::lexer::constant;\
  using tanuki::lexer::regex;\
  using tanuki::lexer::integer;\
  using tanuki::lexer::space;\
  using tanuki::lexer::tab;\
  using tanuki::lexer::blank;\
  \
  using tanuki::lexer::Fragment;\
  \
  using tanuki::ref;\

#include <iostream>

#include "lexer/lexer.h"
#include "exception"

namespace tanuki {
}
