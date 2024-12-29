#include "interpreter/parser.hpp"

#include <bits/ranges_algo.h>

#include <memory>
#include <utility>

#include "const/characters.hpp"
#include "print/printer.hpp"
#include "types/error_code.hpp"
#include "utils/errorx.hpp"
#include "utils/validation.hpp"

void Parser::panic(const int err_code, const std::string& message, const int line) {
    throw err::make(err_code, message, line);
}

Parser Parser::from_tokens(const std::vector<Token>& tokens) {
    auto instance   = Parser();
    instance.tokens = tokens;
    return instance;
}

bool Parser::success() const {
    return errors.empty();
}

Token Parser::current() {
    return tokens[i];
}

bool Parser::is_end() {
    return current().type == TokenType::END;
}

/*
 Synchronize the parser to the next statement.
 This is used to recover from errors and continue parsing the rest of the statements.
 We will discard the tokens until we reach a statement boundary.
 I'm still considering whether left brace should be considered a statement boundary or not.
 */
void Parser::synchronize() {
    advance();
    while(!is_end()) {
        if(previous().type == TokenType::SEMICOLON)
            return;
        switch(current().type) {
        case TokenType::CLASS:
        case TokenType::FUN:
        case TokenType::VAR:
        case TokenType::FOR:
        case TokenType::IF:
        case TokenType::WHILE:
        case TokenType::BREAK:
        case TokenType::CONTINUE:
        case TokenType::RETURN:
            return;
        default:
            advance();
        }
    }
}

Token Parser::previous() {
    if(i == 0)
        return current();
    return tokens[i - 1];
}

Token Parser::consume(const TokenType type, const int err_code, const std::string& message) {
    if(!match(type))
        panic(err_code, message, current().line);
    return previous();
}

Token Parser::advance() {
    if(!is_end())
        i++;
    return previous();
}

bool Parser::check(const TokenType type) {
    if(is_end())
        return false;
    return current().type == type;
}

bool Parser::match(const TokenType type) {
    if(current().type == type) {
        advance();
        return true;
    }
    return false;
}

bool Parser::matches(std::vector<TokenType> types) {
    if(std::ranges::any_of(types, [this](const auto& type) { return current().type == type; })) {
        advance();
        return true;
    }
    return false;
}

ParseResult Parser::parse() {
    return std::make_tuple(program(), errors);
}

std::vector<Stmt> Parser::program() {
    std::vector<Stmt> statements;
    while(!is_end())
        statements.push_back(declaration());
    return statements;
}

Stmt Parser::declaration() {
    try {
        if(match(TokenType::VAR))
            return var_declaration();
        if(match(TokenType::FUN))
            return func_declaration();
        return statement();
    } catch(Error& error) {
        errors.push_back(error);
        synchronize();
        return Stmt{};
    }
}

Stmt Parser::var_declaration() {
    const Token name = consume(TokenType::IDENTIFIER, err::VAR_NAME_MISSING, "Expect variable name.");

    Expr initializer = nullptr;
    if(match(TokenType::EQUAL))
        initializer = expression();

    consume(TokenType::SEMICOLON, err::SEMICOLON_MISSING, "Expect ';' after variable declaration.");

    return VarDeclStmt{name, std::make_shared<Expr>(initializer)};
}

Stmt Parser::func_declaration() {
    const Token name = consume(TokenType::IDENTIFIER, err::NAMED_FUNC_MISSING_NAME, "Expect function name.");
    consume(TokenType::LEFT_PAREN, err::FUNC_PARAMS_MISSING_PAREN, "Expect '(' after function name.");
    std::vector<Token> params;
    if(!check(TokenType::RIGHT_PAREN)) {
        do {
            auto param = consume(TokenType::IDENTIFIER, err::FUNC_PARAM_MISSING_NAME, "Expect parameter name.");
            params.push_back(param);
        } while(match(TokenType::COMMA));
    }
    if(utils::invalid_arity(params.size())) {
        const auto err = err::make(err::TOO_MANY_ARGUMENTS, "Can't have more than 255 parameters.", current().line);
        printer::print_waring(err);
    }
    consume(TokenType::RIGHT_PAREN, err::FUNC_PARAMS_MISSING_PAREN, "Expect ')' after parameters.");
    consume(TokenType::LEFT_BRACE, err::BLOCK_NOT_CLOSED, "Expect '{' before function body.");
    auto body = std::get<BlockStmt>(block_stmt()).statements;
    return FuncDeclStmt{name, std::move(params), std::move(body)};
}

Stmt Parser::statement() {
    if(match(TokenType::IF))
        return if_stmt();
    if(match(TokenType::WHILE))
        return while_stmt();
    if(match(TokenType::LEFT_BRACE))
        return block_stmt();
    if(match(TokenType::FOR))
        return for_stmt();
    if(match(TokenType::BREAK))
        return break_stmt();
    if(match(TokenType::CONTINUE))
        return continue_stmt();
    if(match(TokenType::RETURN))
        return return_stmt();
    return expr_stmt();
}

Stmt Parser::if_stmt() {
    consume(TokenType::LEFT_PAREN, err::IF_COND_MISSING_PAREN, "Expect '(' after 'if'.");
    const auto condition = std::make_shared<Expr>(expression());
    consume(TokenType::RIGHT_PAREN, err::IF_COND_MISSING_PAREN, "Expect ')' after condition.");
    const auto            then_branch = std::make_shared<Stmt>(statement());
    std::shared_ptr<Stmt> else_branch = nullptr;
    if(match(TokenType::ELSE)) {
        else_branch = std::make_shared<Stmt>(statement());
    }
    return IfStmt{condition, then_branch, else_branch};
}

Stmt Parser::expr_stmt() {
    Expr value = expression();
    consume(TokenType::SEMICOLON, err::SEMICOLON_MISSING, "Expect ';' after value.");
    return ExprStmt{std::make_shared<Expr>(value)};
}

Stmt Parser::block_stmt() {
    std::vector<std::shared_ptr<Stmt>> statements;
    while(!check(TokenType::RIGHT_BRACE) && !is_end())
        statements.push_back(std::make_shared<Stmt>(declaration()));
    consume(TokenType::RIGHT_BRACE, err::BLOCK_NOT_CLOSED, "Expect '}' after block.");
    return BlockStmt{statements};
}

Stmt Parser::while_stmt() {
    loop_depth++;
    consume(TokenType::LEFT_PAREN, err::WHILE_COND_MISSING_PAREN, "Expect '(' after 'while'.");
    Expr condition = expression();
    consume(TokenType::RIGHT_PAREN, err::WHILE_COND_MISSING_PAREN, "Expect ')' after condition.");
    Stmt body = statement();
    loop_depth--;
    return WhileStmt{std::make_shared<Expr>(condition), std::make_shared<Stmt>(body)};
}

Stmt Parser::for_stmt() {
    consume(TokenType::LEFT_PAREN, err::FOR_COND_MISSING_PAREN, "Expect '(' after 'for'.");

    std::shared_ptr<Stmt> initializer = nullptr;
    if(match(TokenType::VAR))
        initializer = std::make_shared<Stmt>(var_declaration());
    else if(!match(TokenType::SEMICOLON))
        initializer = std::make_shared<Stmt>(expr_stmt());

    auto condition = std::make_shared<Expr>(Literal{true});
    if(!check(TokenType::SEMICOLON))
        condition = std::make_shared<Expr>(expression());
    consume(TokenType::SEMICOLON, err::SEMICOLON_MISSING, "Expect ';' after loop condition.");

    std::shared_ptr<Expr> increment = nullptr;
    if(!check(TokenType::RIGHT_PAREN))
        increment = std::make_shared<Expr>(expression());

    consume(TokenType::RIGHT_PAREN, err::FOR_COND_MISSING_PAREN, "Expect ')' after for clauses.");

    Stmt stmt = statement();

    if(increment != nullptr)
        stmt = BlockStmt{{std::make_shared<Stmt>(stmt), std::make_shared<Stmt>(ExprStmt{increment})}};

    // desugaring for loop to while loop, so we don't need to keep track of the loop depth
    stmt = WhileStmt{condition, std::make_shared<Stmt>(stmt)};
    if(initializer != nullptr)
        stmt = BlockStmt{{initializer, std::make_shared<Stmt>(stmt)}};

    return stmt;
}

Stmt Parser::break_stmt() {
    if(loop_depth == 0)
        panic(err::BREAK_OUTSIDE_LOOP, "Break statement can only be used inside a loop.", current().line);

    consume(TokenType::SEMICOLON, err::SEMICOLON_MISSING, "Expect ';' after 'break'.");
    return BreakStmt{};
}

Stmt Parser::continue_stmt() {
    if(loop_depth == 0)
        panic(err::CONTINUE_OUTSIDE_LOOP, "Continue statement can only be used inside a loop.", current().line);

    consume(TokenType::SEMICOLON, err::SEMICOLON_MISSING, "Expect ';' after 'continue'.");
    return ContinueStmt{};
}

Stmt Parser::return_stmt() {
    Expr value = nullptr;
    if(!check(TokenType::SEMICOLON))
        value = expression();
    consume(TokenType::SEMICOLON, err::SEMICOLON_MISSING, "Expect ';' after return value.");
    return ReturnStmt{std::make_shared<Expr>(value)};
}

Expr Parser::expression() {
    return assignment();
}

Expr Parser::assignment() {
    Expr expr = logical_or();

    if(match(TokenType::EQUAL)) {
        Token equals = previous();
        Expr  value  = assignment();

        if(std::holds_alternative<Variable>(expr)) {
            const Token name = std::get<Variable>(expr).name;
            return Assign{name, std::make_shared<Expr>(value)};
        }
        throw Error(err::INVALID_ASSIGNMENT_TARGET, "Invalid assignment target.");
    }

    return expr;
}

Expr Parser::logical_or() {
    Expr expr = logical_and();

    while(match(TokenType::OR)) {
        const Token op    = previous();
        Expr        right = logical_and();

        expr = Logical{
            .left  = std::make_shared<Expr>(expr),
            .op    = op,
            .right = std::make_shared<Expr>(right),
        };
    }

    return expr;
}

Expr Parser::logical_and() {
    Expr expr = equality();

    while(match(TokenType::AND)) {
        const Token op    = previous();
        Expr        right = equality();

        expr = Logical{
            .left  = std::make_shared<Expr>(expr),
            .op    = op,
            .right = std::make_shared<Expr>(right),
        };
    }

    return expr;
}

Expr Parser::equality() {
    Expr expr = comparison();

    while(matches({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL})) {
        const Token op    = previous();
        Expr        right = comparison();

        expr = Binary{
            .left  = std::make_shared<Expr>(expr),
            .op    = op,
            .right = std::make_shared<Expr>(right),
        };
    }

    return expr;
}

Expr Parser::comparison() {
    Expr expr = term();
    while(matches({TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL})) {
        const Token op    = previous();
        Expr        right = term();

        expr = Binary{
            .left  = std::make_shared<Expr>(expr),
            .op    = op,
            .right = std::make_shared<Expr>(right),
        };
    }
    return expr;
}

Expr Parser::term() {
    Expr expr = factor();
    while(matches({TokenType::MINUS, TokenType::PLUS})) {
        const Token op    = previous();
        Expr        right = factor();

        expr = Binary{
            .left  = std::make_shared<Expr>(expr),
            .op    = op,
            .right = std::make_shared<Expr>(right),
        };
    }
    return expr;
}

Expr Parser::factor() {
    Expr expr = unary();
    while(matches({TokenType::SLASH, TokenType::STAR, TokenType::PERCENT})) {
        const Token op    = previous();
        Expr        right = unary();

        expr = Binary{
            .left  = std::make_shared<Expr>(expr),
            .op    = op,
            .right = std::make_shared<Expr>(right),
        };
    }
    return expr;
}

Expr Parser::unary() {
    if(matches({TokenType::BANG, TokenType::MINUS})) {
        const Token op    = previous();
        Expr        right = unary();

        return Unary{
            .op    = op,
            .right = std::make_shared<Expr>(right),
        };
    }
    return call();
}

Expr Parser::call() {
    Expr expr = primary();
    while(match(TokenType::LEFT_PAREN)) {
        expr = arguments(std::make_shared<Expr>(expr));
    }
    return expr;
}

Expr Parser::arguments(std::shared_ptr<Expr> callee) {
    std::vector<std::shared_ptr<Expr>> args;
    if(!check(TokenType::RIGHT_PAREN)) {
        do {
            args.push_back(std::make_shared<Expr>(expression()));
        } while(match(TokenType::COMMA));
    }
    if(utils::invalid_arity(args.size())) {
        const auto err = err::make(err::TOO_MANY_ARGUMENTS, "Can't have more than 255 arguments.", current().line);
        printer::print_waring(err);
    }
    auto paren = consume(TokenType::RIGHT_PAREN, err::CALL_NOT_CLOSED, "Expect ')' after arguments.");
    return Call{std::move(callee), std::move(paren), std::move(args)};
}

Expr Parser::primary() {
    if(match(TokenType::FALSE))
        return Literal{false};
    if(match(TokenType::TRUE))
        return Literal{true};
    if(match(TokenType::NIL))
        return Literal{nullptr};

    if(matches({TokenType::NUMBER, TokenType::STRING}))
        return Literal{previous().literal};

    if(match(TokenType::LEFT_PAREN)) {
        Expr expr = expression();
        consume(TokenType::RIGHT_PAREN, err::EXPR_NOT_CLOSED, "Error at ')': Expect expression.");
        return Grouping{std::make_shared<Expr>(expr)};
    }

    if(match(TokenType::ARROW))
        return lambda();

    if(match(TokenType::IDENTIFIER))
        return Variable{previous()};

    // The parsing process is designed to always decades the expression to the lowest level
    // That's mean if the parser can not detect any primary expression, it will be an error.
    // Should never reach here
    throw Error(err::UNKNOWN_PARSING_ERROR, "Parsing progress reached to an unknown state.");
}

Expr Parser::lambda() {
    consume(TokenType::LEFT_PAREN, err::FUNC_PARAMS_MISSING_PAREN, "Expect '(' after 'lambda'.");
    std::vector<Token> params;
    if(!check(TokenType::RIGHT_PAREN)) {
        do {
            auto param = consume(TokenType::IDENTIFIER, err::FUNC_PARAM_MISSING_NAME, "Expect parameter name.");
            params.push_back(param);
        } while(match(TokenType::COMMA));
    }
    if(utils::invalid_arity(params.size())) {
        const auto err = err::make(err::TOO_MANY_ARGUMENTS, "Can't have more than 255 parameters.", current().line);
        printer::print_waring(err);
    }
    consume(TokenType::RIGHT_PAREN, err::FUNC_PARAMS_MISSING_PAREN, "Expect ')' after parameters.");
    consume(TokenType::LEFT_BRACE, err::BLOCK_NOT_CLOSED, "Expect '{' before lambda body.");
    auto body = std::get<BlockStmt>(block_stmt()).statements;
    return Lambda{std::move(params), std::move(body)};
}