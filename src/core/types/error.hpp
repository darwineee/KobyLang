#pragma once

#include <stdexcept>
#include <string>

struct Error;

struct Error final : std::runtime_error {
    int code;
    Error(const int code, const std::string& message) : std::runtime_error(message), code(code) {}
};