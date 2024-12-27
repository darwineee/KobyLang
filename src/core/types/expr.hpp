#pragma once

#include "token_t.hpp"

#include <memory>
#include <variant>
#include <vector>

struct Binary;
struct Unary;
struct Grouping;
struct Variable;
struct Assign;
struct Logical;
struct Call;
using Literal = std::variant<std::nullptr_t, double, std::string, bool>;

using Expr = std::variant<Binary, Grouping, Unary, Literal, Variable, Assign, Logical, Call>;

struct Token;

struct Token {
    TokenType   type;
    std::string lexeme;
    Literal     literal;
    int         line;
};

struct Binary {
    std::shared_ptr<Expr> left;
    Token                 op;
    std::shared_ptr<Expr> right;
};

struct Unary {
    Token                 op;
    std::shared_ptr<Expr> right;
};

struct Grouping {
    std::shared_ptr<Expr> expr;
};

struct Variable {
    Token name;
};

struct Assign {
    Token                 name;
    std::shared_ptr<Expr> value;
};

struct Logical {
    std::shared_ptr<Expr> left;
    Token                 op;
    std::shared_ptr<Expr> right;
};

struct Call {
    std::shared_ptr<Expr>              callee;
    Token                              paren;
    std::vector<std::shared_ptr<Expr>> args;
};