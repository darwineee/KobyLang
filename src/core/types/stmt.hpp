#pragma once

#include "expr.hpp"
#include <memory>
#include <variant>
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

using Stmt = std::variant<
    ExprStmt,
    IfStmt,
    VarDeclStmt,
    FuncDeclStmt,
    BlockStmt,
    WhileStmt,
    BreakStmt,
    ContinueStmt,
    ReturnStmt>;

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