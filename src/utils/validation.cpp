#include <locale>

namespace utils {

bool is_identifier_start(const char c) {
    return std::isalpha(c) || c == '_';
}

bool is_identifier_char(const char c) {
    return std::isdigit(c) || is_identifier_start(c);
}

bool invalid_arity(const size_t count) {
    return count >= 255;
}

} // namespace utils