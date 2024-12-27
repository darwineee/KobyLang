#include "types/error.hpp"

#include <format>

namespace err {

Error make(const int err_code, const std::string& message, int line) {
    return Error{err_code, std::format("{}\n[line {}]", message, line)};
}

} // namespace err