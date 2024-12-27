#pragma once

#include "const/characters.hpp"
#include "types/error.hpp"
#include "types/result.hpp"

#include <string>
#include <unordered_map>
#include <vector>

/**
 * Tokenizes the source code into a list of tokens.
 */
class Scanner;

class Scanner {
    std::string        source;
    std::vector<Token> tokens;
    std::vector<Error> errors;

    std::unordered_map<std::string, TokenType> keywords = {
        {keyword::And, TokenType::AND},
        {keyword::Or, TokenType::OR},
        {keyword::If, TokenType::IF},
        {keyword::Else, TokenType::ELSE},
        {keyword::True, TokenType::TRUE},
        {keyword::False, TokenType::FALSE},
        {keyword::Class, TokenType::CLASS},
        {keyword::This, TokenType::THIS},
        {keyword::Super, TokenType::SUPER},
        {keyword::Fun, TokenType::FUN},
        {keyword::Var, TokenType::VAR},
        {keyword::For, TokenType::FOR},
        {keyword::While, TokenType::WHILE},
        {keyword::Break, TokenType::BREAK},
        {keyword::Continue, TokenType::CONTINUE},
        {keyword::Return, TokenType::RETURN},
        {keyword::Nil, TokenType::NIL},
    };

    void collect_err(int err_code, std::string message, int line);

    /** Using two pointers to scan the source.
     * Pointer i is the start of the current lexeme, pointer j is the current character.*/
    int i = -1, j = -1;

    /* The current line number */
    int line = 1;

    Scanner() = default;

    [[nodiscard]]
    bool is_eof(int index) const;

    /* Safe way to get a character, instead of directly get from source */
    [[nodiscard]]
    char get_char_at(int index) const;

    /* Advances the scanner by one character */
    char advance();

    /* Returns the next character without advancing the scanner */
    [[nodiscard]]
    char ahead(int n = 1) const;

    bool ahead_match(char expected);
    void scan_next();

    [[nodiscard]]
    char current() const;

    void add_token(TokenType type, const Literal& literal = nullptr);

    void handle_slash();
    void handle_string();
    void handle_number();
    void handle_identifier();

public:
    ~Scanner() = default;

    ScanResult     scan_tokens();
    static Scanner from_source(const std::string& source);
    bool           success() const;
};
