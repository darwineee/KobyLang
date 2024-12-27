#include "const/characters.hpp"
#include "interpreter/interpreter.hpp"
#include "types/token_t.hpp"
#include "utils/templ.hpp"

#include <cmath>
#include <iomanip>
#include <sstream>
#include <string>

namespace utils {

/* Used to validate scanner result */
std::string to_string(const TokenType type) {
    switch(type) {
    case TokenType::LEFT_PAREN:
        return "LEFT_PAREN";
    case TokenType::RIGHT_PAREN:
        return "RIGHT_PAREN";
    case TokenType::LEFT_BRACE:
        return "LEFT_BRACE";
    case TokenType::RIGHT_BRACE:
        return "RIGHT_BRACE";
    case TokenType::PLUS:
        return "PLUS";
    case TokenType::MINUS:
        return "MINUS";
    case TokenType::COMMA:
        return "COMMA";
    case TokenType::DOT:
        return "DOT";
    case TokenType::SEMICOLON:
        return "SEMICOLON";
    case TokenType::STAR:
        return "STAR";
    case TokenType::SLASH:
        return "SLASH";
    case TokenType::BANG:
        return "BANG";
    case TokenType::BANG_EQUAL:
        return "BANG_EQUAL";
    case TokenType::EQUAL:
        return "EQUAL";
    case TokenType::EQUAL_EQUAL:
        return "EQUAL_EQUAL";
    case TokenType::GREATER:
        return "GREATER";
    case TokenType::GREATER_EQUAL:
        return "GREATER_EQUAL";
    case TokenType::LESS:
        return "LESS";
    case TokenType::LESS_EQUAL:
        return "LESS_EQUAL";
    case TokenType::IDENTIFIER:
        return "IDENTIFIER";
    case TokenType::STRING:
        return "STRING";
    case TokenType::NUMBER:
        return "NUMBER";
    case TokenType::AND:
        return "AND";
    case TokenType::OR:
        return "OR";
    case TokenType::IF:
        return "IF";
    case TokenType::ELSE:
        return "ELSE";
    case TokenType::TRUE:
        return "TRUE";
    case TokenType::FALSE:
        return "FALSE";
    case TokenType::CLASS:
        return "CLASS";
    case TokenType::THIS:
        return "THIS";
    case TokenType::SUPER:
        return "SUPER";
    case TokenType::FUN:
        return "FUN";
    case TokenType::VAR:
        return "VAR";
    case TokenType::FOR:
        return "FOR";
    case TokenType::WHILE:
        return "WHILE";
    case TokenType::RETURN:
        return "RETURN";
    case TokenType::NIL:
        return "NIL";
    case TokenType::END:
        return "END";
    case TokenType::ARROW:
        return "ARROW";
    default:
        return "?";
    }
}

std::string to_string(const Value& value) {
    return std::visit<std::string>(
        overloaded{
            [](std::nullptr_t) { return keyword::Nil; },
            [](const double num) {
                char buffer[32];
                int  len = num == std::floor(num) ? snprintf(buffer, sizeof(buffer), "%.0f", num)
                                                  : snprintf(buffer, sizeof(buffer), "%.6f", num);
                while(len > 0 && buffer[len - 1] == '0')
                    len--;
                return std::string(buffer, len);
            },
            [](const std::string& str) { return str; },
            [](const bool boolean) { return boolean ? keyword::True : keyword::False; },
            [](const std::shared_ptr<Callable>& callable) { return callable->to_string(); },
        },
        value);
}

} // namespace utils