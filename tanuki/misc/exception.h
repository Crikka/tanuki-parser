#pragma once

#include <exception>

class ParseError : public std::exception {};
class NoExecuteDefinition : public std::exception {};
class NullReferenceError : public std::exception {};
