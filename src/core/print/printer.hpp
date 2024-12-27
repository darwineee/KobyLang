#pragma once
#include <memory>

namespace printer {

void print(const Value& value);

void print(const Error& error);

void print_err(const std::vector<Error>& errors);

void print_err(const ScanResult& result);

void print_err(const ParseResult& result);

} // namespace printer
