#include "types/error.hpp"

#include <format>

namespace err {

Error make(const int err_code, const std::string& message, int line) {
    return Error{err_code, std::format("[line {}] {}", line, message)};
}

} // namespace err