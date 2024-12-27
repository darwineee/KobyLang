#include "interpreter/scanner.hpp"

#include "const/characters.hpp"
#include "types/error_code.hpp"
#include "types/token_t.hpp"
#include "utils/validation.hpp"

#include <iostream>

Scanner Scanner::from_source(const std::string& source) {
    auto instance   = Scanner();
    instance.source = source;
    return instance;
}

bool Scanner::success() const {
    return errors.empty();
}

void Scanner::collect_err(const int err_code, std::string message, const int line) {
    errors.emplace_back(err_code, std::format("[line {}] Error: {}", line, message));
}

bool Scanner::is_eof(const int index) const {
    return index >= source.size();
}

char Scanner::get_char_at(const int index) const {
    return is_eof(index) ? delimiter::ENDOF : source[index];
}

char Scanner::advance() {
    j++;
    i = j;
    return source[i];
}

char Scanner::ahead(const int n) const {
    const auto next = j + n;
    return get_char_at(next);
}

bool Scanner::ahead_match(const char expected) {
    if(ahead() == delimiter::ENDOF || ahead() != expected)
        return false;
    j++;
    return true;
}

char Scanner::current() const {
    return get_char_at(j);
}

void Scanner::scan_next() {
    j++;
}

void Scanner::add_token(const TokenType type, const Literal& literal) {
    const auto token = Token{
        .type    = type,
        .lexeme  = type == TokenType::END ? EMPTY : source.substr(i, j - i + 1),
        .literal = literal,
        .line    = line,
    };
    tokens.push_back(token);
}

void Scanner::handle_slash() {
    // Skip comment
    if(ahead_match(symbol::Slash)) {
        while(ahead() != delimiter::NEWLINE && ahead() != delimiter::ENDOF)
            advance();
    } else
        add_token(TokenType::SLASH);
}

void Scanner::handle_string() {
    while(ahead() != surround::DoubleQuote && ahead() != delimiter::ENDOF) {
        scan_next();
    }

    if(ahead() == delimiter::ENDOF) {
        collect_err(err::UNTERMINATED_STRING, "Unterminated string.", line);
        return;
    }

    scan_next();
    const std::string str = source.substr(i + 1, j - i - 1);
    add_token(TokenType::STRING, str);
}

void Scanner::handle_number() {
    while(isdigit(ahead()))
        scan_next();
    if(ahead() == symbol::Dot && isdigit(ahead(2))) {
        scan_next();
        while(isdigit(ahead()))
            scan_next();
    }
    const std::string str = source.substr(i, j - i + 1);
    double            val = std::stod(str);
    add_token(TokenType::NUMBER, val);
}

void Scanner::handle_identifier() {
    while(utils::is_identifier_char(ahead()))
        scan_next();
    //Allows "?" at the end of identifiers
    if (ahead() == symbol::Question)
        scan_next();
    if(const std::string str = source.substr(i, j - i + 1); keywords.contains(str))
        add_token(keywords[str]);
    else
        add_token(TokenType::IDENTIFIER);
}

ScanResult Scanner::scan_tokens() {
    while(ahead() != delimiter::ENDOF) {
        switch(const auto c = advance()) {
        // Count new line
        case delimiter::NEWLINE:
            line++;
            break;
        case delimiter::TAB:
        case delimiter::SPACE:
            break;

            // Case symbol
        case symbol::LeftParen:
            add_token(TokenType::LEFT_PAREN);
            break;
        case symbol::RightParen:
            add_token(TokenType::RIGHT_PAREN);
            break;
        case symbol::LeftBrace:
            add_token(TokenType::LEFT_BRACE);
            break;
        case symbol::RightBrace:
            add_token(TokenType::RIGHT_BRACE);
            break;
        case symbol::Plus:
            add_token(TokenType::PLUS);
            break;
        case symbol::Minus:
            add_token(TokenType::MINUS);
            break;
        case symbol::Comma:
            add_token(TokenType::COMMA);
            break;
        case symbol::Dot:
            add_token(TokenType::DOT);
            break;
        case symbol::Semicolon:
            add_token(TokenType::SEMICOLON);
            break;
        case symbol::Star:
            add_token(TokenType::STAR);
            break;
        case symbol::Slash:
            handle_slash();
            break;

        // Case operators
        case op::Bang:
            if(ahead_match(op::Equal))
                add_token(TokenType::BANG_EQUAL);
            else
                add_token(TokenType::BANG);
            break;
        case op::Equal:
            if(ahead_match(op::Equal))
                add_token(TokenType::EQUAL_EQUAL);
            else
                add_token(TokenType::EQUAL);
            break;
        case op::Greater:
            if(ahead_match(op::Equal))
                add_token(TokenType::GREATER_EQUAL);
            else
                add_token(TokenType::GREATER);
            break;
        case op::Less:
            if(ahead_match(op::Equal))
                add_token(TokenType::LESS_EQUAL);
            else
                add_token(TokenType::LESS);
            break;

        case surround::DoubleQuote:
            handle_string();
            break;

        default:
            if(isdigit(c))
                handle_number();
            else if(utils::is_identifier_start(c))
                handle_identifier();
            else
                collect_err(
                    err::LEXICAL_ERROR,
                    std::string("Unexpected character: ") + source[j],
                    line);
        }
    }
    add_token(TokenType::END);
    return {tokens, errors};
}