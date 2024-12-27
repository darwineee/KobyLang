#pragma once

#include "types/error.hpp"
#include "types/result.hpp"
#include "types/stmt.hpp"
#include "types/token_t.hpp"

#include <vector>

/**
 * Parses the list of tokens into an abstract syntax tree.
 */
class Parser;

class Parser {
    std::vector<Token> tokens;
    std::vector<Error> errors;

    /**
     * Current processing token index
     */
    int i = 0;

    /**
     * Tracking the depth of the loop
     */
    int loop_depth = 0;

    Parser() = default;

    static void panic(int err_code, const std::string& message, int line);

    Token advance();
    bool  check(TokenType type);
    bool  match(TokenType type);
    bool  matches(std::vector<TokenType> types);

    Token current();
    Token previous();
    Token consume(TokenType type, int err_code, std::string message);
    bool  is_end();
    void  synchronize();

    std::vector<Stmt> program();

    Stmt declaration();
    Stmt var_declaration();
    Stmt func_declaration();
    Stmt statement();
    Stmt if_stmt();
    Stmt expr_stmt();
    Stmt block_stmt();
    Stmt while_stmt();
    Stmt for_stmt();
    Stmt break_stmt();
    Stmt continue_stmt();
    Stmt return_stmt();

    Expr expression();
    Expr assignment();
    Expr logical_or();
    Expr logical_and();
    Expr equality();
    Expr comparison();
    Expr term();
    Expr factor();
    Expr unary();
    Expr call();
    /* Collect arguments for the function call and create Call expression */
    Expr arguments(std::shared_ptr<Expr> callee);
    Expr primary();

public:
    ~Parser() = default;
    static Parser from_tokens(const std::vector<Token>& tokens);

    ParseResult   parse();

    [[nodiscard]]
    bool success() const;
};