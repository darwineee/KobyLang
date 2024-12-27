#pragma once

enum class TokenType {
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    PLUS,
    MINUS,
    COMMA,
    DOT,
    SEMICOLON,
    STAR,
    SLASH,

    BANG,
    BANG_EQUAL,
    EQUAL,
    EQUAL_EQUAL,
    GREATER,
    GREATER_EQUAL,
    LESS,
    LESS_EQUAL,

    ARROW,

    IDENTIFIER,
    STRING,
    NUMBER,

    AND,
    OR,
    IF,
    ELSE,
    TRUE,
    FALSE,
    CLASS,
    THIS,
    SUPER,
    FUN,
    VAR,
    FOR,
    WHILE,
    BREAK,
    CONTINUE,
    RETURN,
    NIL,

    END
};
