#pragma once

#include "interpreter/interpreter.hpp"
#include "types/token_t.hpp"

#include <string>

namespace utils {

std::string to_string(TokenType type);

std::string to_string(const Value& value);

} // namespace utils