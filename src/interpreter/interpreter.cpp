#include "interpreter/interpreter.hpp"

#include "const/prelude_func.hpp"
#include "types/error_code.hpp"
#include "utils/errorx.hpp"
#include "utils/templ.hpp"
#include "utils/to_string.hpp"

#include <chrono>
#include <iostream>
#include <memory>

void Interpreter::panic(const int err_code, const std::string& message, const int line) {
    throw err::make(err_code, message, line);
}

void Interpreter::prelude() const {
    NativeFunc now_func{0, [](Interpreter&, const std::vector<Value>&) -> ExecSig {
                       using namespace std::chrono;
                       const auto now = duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
                       return ExecSig{.value = Value(static_cast<double>(now))};
                   }};
    NativeFunc put_func{1, [](Interpreter&, const std::vector<Value>& args) -> ExecSig {
                         std::cout << utils::to_string(args[0]) << std::endl;
                         return ExecSig{};
                     }};
    global_env->define(prelude::NOW, Value(std::make_shared<NativeFunc>(now_func)));
    global_env->define(prelude::PUT, Value(std::make_shared<NativeFunc>(put_func)));
}

void Interpreter::exclude_native_func(const std::vector<std::string>& list) const {
    for(auto const& name : list)
        global_env->remove(name);
}

bool Interpreter::is_truthy(const Value& value) {
    if(std::holds_alternative<bool>(value))
        return std::get<bool>(value);
    if(std::holds_alternative<nullptr_t>(value))
        return false;
    return true;
}

bool Interpreter::is_equal(const Value& left, const Value& right) {
    if(std::holds_alternative<nullptr_t>(left) && std::holds_alternative<nullptr_t>(right))
        return true;
    if(std::holds_alternative<nullptr_t>(left) || std::holds_alternative<nullptr_t>(right))
        return false;
    return left == right;
}

bool Interpreter::is_num_operand(const Value& operand) {
    return std::holds_alternative<double>(operand);
}

void Interpreter::ensure_num_operands(const Token& op, const std::vector<Value>& operands) {
    for(const auto& operand : operands) {
        if(!is_num_operand(operand))
            panic(err::OPERAND_INVALID, "Operand must be a number.", op.line);
    }
}

ExecSig Interpreter::interpret(const std::vector<Stmt>& statements) {
    auto res = ExecSig{};
    for(const auto& stmt : statements)
        res = run(stmt);
    return res;
}

ExecSig Interpreter::run(const Stmt& stmt) {
    return std::visit<ExecSig>(
        overloaded{
            [this](const ExprStmt& expr_stmt) { return runExprStmt(expr_stmt); },
            [this](const VarDeclStmt& var_decl_stmt) { return runVarDeclStmt(var_decl_stmt); },
            [this](const FuncDeclStmt& func_decl_stmt) { return runFuncDeclStmt(func_decl_stmt); },
            [this](const BlockStmt& block_stmt) { return runBlockStmt(block_stmt); },
            [this](const IfStmt& if_stmt) { return runIfStmt(if_stmt); },
            [this](const WhileStmt& while_stmt) { return runWhileStmt(while_stmt); },
            [this](const BreakStmt) { return runBreakStmt(); },
            [this](const ContinueStmt) { return runContinueStmt(); },
            [this](const ReturnStmt& return_stmt) { return runReturnStmt(return_stmt); },
        },
        stmt);
}

ExecSig Interpreter::runExprStmt(const ExprStmt& stmt) {
    return ExecSig{.value = evaluate(stmt.expr)};
}

ExecSig Interpreter::runIfStmt(const IfStmt& stmt) {
    auto res = ExecSig{};
    if(is_truthy(evaluate(stmt.condition)))
        res = run(*stmt.then_branch);
    else if(stmt.else_branch)
        res = run(*stmt.else_branch);
    return res;
}

ExecSig Interpreter::runVarDeclStmt(const VarDeclStmt& stmt) {
    Value value = nullptr;
    if(stmt.initializer)
        value = evaluate(stmt.initializer);

    env->define(stmt.name, value);
    return ExecSig{.value = value};
}

ExecSig Interpreter::runFuncDeclStmt(const FuncDeclStmt& stmt) {
    // TODO change to use closure env scope
    const auto func = std::make_shared<Func>(stmt.params, stmt.body, env, stmt.name);
    env->define(stmt.name, Value(func));
    return ExecSig{};
}

ExecSig Interpreter::runBlockStmt(const BlockStmt& stmt) {
    return executeBlock(stmt.statements, std::make_shared<Environment>(Environment{env}));
}

ExecSig Interpreter::executeBlock(
    const std::vector<std::shared_ptr<Stmt>>& statements,
    const std::shared_ptr<Environment>&       environment) {

    const auto current_env = env;
    env                    = environment;
    auto res               = ExecSig{};
    for(const auto& stmt : statements) {
        res = run(*stmt);
        // handle case break/continue/return nested in block
        if(res.control == ExecControl::BREAK || res.control == ExecControl::CONTINUE ||
           res.control == ExecControl::RETURN) {
            break;
        }
    }
    env = current_env;
    return res;
}

ExecSig Interpreter::runWhileStmt(const WhileStmt& stmt) {
    auto result = ExecSig{};
    while(is_truthy(evaluate(stmt.condition))) {
        auto res = run(*stmt.body);
        if(res.control == ExecControl::BREAK) {
            break;
        }
        if(res.control == ExecControl::CONTINUE) {
            continue;
        }
        if(res.control == ExecControl::RETURN) {
            return res;
        }
        result = res;
    }
    return result;
}

ExecSig Interpreter::runBreakStmt() {
    return ExecSig{ExecControl::BREAK};
}

ExecSig Interpreter::runContinueStmt() {
    return ExecSig{ExecControl::CONTINUE};
}

ExecSig Interpreter::runReturnStmt(const ReturnStmt& stmt) {
    Value value = nullptr;
    if(stmt.value)
        value = evaluate(stmt.value);
    return ExecSig{ExecControl::RETURN, value};
}

Value Interpreter::evaluate(const std::shared_ptr<Expr>& expr) {
    return std::visit<Value>(
        overloaded{
            [this](const Binary& binary) { return evaluateBinaryExpr(binary); },
            [this](const Grouping& grouping) { return evaluateGroupingExpr(grouping); },
            [this](const Literal& literal) { return evaluateLiteralExpr(literal); },
            [this](const Unary& unary) { return evaluateUnaryExpr(unary); },
            [this](const Variable& variable) { return evaluateVariableExpr(variable); },
            [this](const Assign& assign) { return evaluateAssignExpr(assign); },
            [this](const Logical& logical) { return evaluateLogicalExpr(logical); },
            [this](const Call& call) { return evaluateCallExpr(call); },
            [this](const Lambda& lambda) { return evaluateLambdaExpr(lambda); },
        },
        *expr);
}

Value Interpreter::evaluateVariableExpr(const Variable& variable) const {
    return env->get(variable.name.lexeme);
}

Value Interpreter::evaluateAssignExpr(const Assign& assign) {
    const Value value = evaluate(assign.value);
    env->assign(assign.name.lexeme, value);
    return value;
}

Value Interpreter::evaluateLiteralExpr(const Literal& literal) {
    return std::visit([](const auto& val) -> Value { return Value(val); }, literal);
}

Value Interpreter::evaluateGroupingExpr(const Grouping& group) {
    return evaluate(group.expr);
}

Value Interpreter::evaluateLogicalExpr(const Logical& logical) {
    Value left = evaluate(logical.left);
    // short-circuiting
    if(logical.op.type == TokenType::OR) {
        if(is_truthy(left))
            return left;
    } else {
        if(!is_truthy(left))
            return left;
    }
    return evaluate(logical.right);
}

Value Interpreter::evaluateCallExpr(const Call& call) {
    const Value callee = evaluate(call.callee);
    if(!std::holds_alternative<std::shared_ptr<Callable>>(callee))
        panic(err::NOT_CALLABLE, "Can only call functions.", call.paren.line);

    auto& callable = std::get<std::shared_ptr<Callable>>(callee);
    if(call.args.size() != callable->arity())
        panic(
            err::ARGUMENT_COUNT_MISMATCH,
            std::format("Expected {} arguments but got {}.", callable->arity(), call.args.size()),
            call.paren.line);

    std::vector<Value> arguments;
    for(const auto& arg : call.args)
        arguments.push_back(evaluate(arg));

    return callable->call(*this, arguments).value;
}

Value Interpreter::evaluateLambdaExpr(const Lambda& lambda) const {
    return std::make_shared<LambdaFunc>(LambdaFunc{lambda.params, lambda.body, env});
}

Value Interpreter::evaluateUnaryExpr(const Unary& unary) {
    const Value right = evaluate(unary.right);

    switch(unary.op.type) {
    case TokenType::MINUS:
        ensure_num_operands(unary.op, {right});
        return -std::get<double>(right);

    case TokenType::BANG:
        return !is_truthy(right);

    default:
        return nullptr;
    }
}

Value Interpreter::evaluateBinaryExpr(const Binary& binary) {
    const Value left  = evaluate(binary.left);
    const Value right = evaluate(binary.right);
    switch(binary.op.type) {
    case TokenType::MINUS:
        ensure_num_operands(binary.op, {left, right});
        return std::get<double>(left) - std::get<double>(right);

    case TokenType::SLASH:
        ensure_num_operands(binary.op, {left, right});
        return std::get<double>(left) / std::get<double>(right);

    case TokenType::STAR:
        ensure_num_operands(binary.op, {left, right});
        return std::get<double>(left) * std::get<double>(right);

    case TokenType::PLUS:
        if(is_num_operand(left) && is_num_operand(right))
            return std::get<double>(left) + std::get<double>(right);
        return utils::to_string(left) + utils::to_string(right);

    case TokenType::GREATER:
        ensure_num_operands(binary.op, {left, right});
        return std::get<double>(left) > std::get<double>(right);

    case TokenType::GREATER_EQUAL:
        ensure_num_operands(binary.op, {left, right});
        return std::get<double>(left) >= std::get<double>(right);

    case TokenType::LESS:
        ensure_num_operands(binary.op, {left, right});
        return std::get<double>(left) < std::get<double>(right);

    case TokenType::LESS_EQUAL:
        ensure_num_operands(binary.op, {left, right});
        return std::get<double>(left) <= std::get<double>(right);

    case TokenType::BANG_EQUAL:
        return !is_equal(left, right);

    case TokenType::EQUAL_EQUAL:
        return is_equal(left, right);

    default:
        return nullptr;
    }
}
