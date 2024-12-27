#pragma once

#include "interpreter/interpreter.hpp"
#include "interpreter/parser.hpp"
#include "types/error.hpp"

#include <vector>

namespace printer {

void print(const Value& value);

void print_waring(const Error& error);

void print_err(const Error& error);

void print_err(const std::vector<Error>& errors);

void print_res_err(const ScanResult& result);

void print_res_err(const ParseResult& result);

} // namespace printer
