#pragma once

#include "types/error.hpp"
#include "types/token_t.hpp"

#include <memory>
#include <vector>

struct ExprStmt;
struct IfStmt;
struct VarDeclStmt;
struct FuncDeclStmt;
struct BlockStmt;
struct WhileStmt;
struct BreakStmt;
struct ContinueStmt;
struct ReturnStmt;

using Stmt = std::
    variant<ExprStmt, IfStmt, VarDeclStmt, FuncDeclStmt, BlockStmt, WhileStmt, BreakStmt, ContinueStmt, ReturnStmt>;

struct Binary;
struct Unary;
struct Grouping;
struct Variable;
struct Assign;
struct Logical;
struct Call;
struct Lambda;
using Literal = std::variant<std::nullptr_t, double, std::string, bool>;

using Expr = std::variant<Binary, Grouping, Unary, Literal, Variable, Assign, Logical, Call, Lambda>;

struct Token;

using ScanResult  = std::tuple<std::vector<Token>, std::vector<Error>>;
using ParseResult = std::tuple<std::vector<Stmt>, std::vector<Error>>;

struct ExprStmt {
    std::shared_ptr<Expr> expr;
};

struct IfStmt {
    std::shared_ptr<Expr> condition;
    std::shared_ptr<Stmt> then_branch;
    std::shared_ptr<Stmt> else_branch;
};

struct VarDeclStmt {
    std::string           name;
    std::shared_ptr<Expr> initializer;
};

struct FuncDeclStmt {
    std::string                        name;
    std::vector<Token>                 params;
    std::vector<std::shared_ptr<Stmt>> body;
};

struct BlockStmt {
    std::vector<std::shared_ptr<Stmt>> statements;
};

struct WhileStmt {
    std::shared_ptr<Expr> condition;
    std::shared_ptr<Stmt> body;
};

struct BreakStmt {};
struct ContinueStmt {};

struct ReturnStmt {
    std::shared_ptr<Expr> value;
};

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

struct Lambda {
    std::vector<Token>                 params;
    std::vector<std::shared_ptr<Stmt>> body;
};

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
    Token consume(TokenType type, int err_code, const std::string& message);
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
    Expr lambda();

public:
    ~Parser() = default;
    static Parser from_tokens(const std::vector<Token>& tokens);

    ParseResult parse();

    [[nodiscard]]
    bool success() const;
};