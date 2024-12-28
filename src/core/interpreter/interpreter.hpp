#pragma once

#include "parser.hpp"

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

struct Callable;
struct Func;
struct NativeFunc;
using Value = std::variant<std::nullptr_t, double, std::string, bool, std::shared_ptr<Callable>>;

struct ExecSig;
enum class ExecControl {
    NORMAL,
    BREAK,
    CONTINUE,
    RETURN,
};
struct ExecSig {
    ExecControl control = ExecControl::NORMAL;
    Value       value;
};

class Environment;
class Environment {
    std::unordered_map<std::string, Value> variables;
    std::shared_ptr<Environment>           enclosing = nullptr;

public:
    Environment() = default;
    explicit Environment(const std::shared_ptr<Environment>& enclosing) : enclosing(enclosing) {}
    ~Environment() = default;

    bool  contains(const std::string& name) const;
    void  define(const std::string& name, const Value& value);
    void  define(const Token& name, const Value& value);
    Value get(std::string name);
    void  assign(std::string name, const Value& value);
    void  remove(const std::string& name);
};

class Interpreter;
class Interpreter {
    std::shared_ptr<Environment> global_env = std::make_shared<Environment>();
    std::shared_ptr<Environment> env        = global_env;

    static void panic(int err_code, const std::string& message, int line);

    /*
     false and nil is falsy, everything else is truthy
     */
    static bool is_truthy(const Value& value);

    /*
     Check if two values are equal
     */
    static bool is_equal(const Value& left, const Value& right);

    /*
     Ensure that the operand is a number (double)
     */
    static void ensure_num_operands(const Token& op, const std::vector<Value>& operands);
    static bool is_num_operand(const Value& operand);

    ExecSig run(const Stmt& stmt);
    ExecSig runExprStmt(const ExprStmt& stmt);
    ExecSig runIfStmt(const IfStmt& stmt);
    ExecSig runVarDeclStmt(const VarDeclStmt& stmt);
    ExecSig runFuncDeclStmt(const FuncDeclStmt& stmt);
    ExecSig runBlockStmt(const BlockStmt& stmt);
    ExecSig runWhileStmt(const WhileStmt& stmt);
    ExecSig runReturnStmt(const ReturnStmt& stmt);

    static ExecSig runBreakStmt();
    static ExecSig runContinueStmt();

    static Value evaluateLiteralExpr(const Literal& literal);

    [[nodiscard]]
    Value evaluateVariableExpr(const Variable& variable) const;

    Value evaluate(const std::shared_ptr<Expr>& expr);
    Value evaluateBinaryExpr(const Binary& binary);
    Value evaluateGroupingExpr(const Grouping& group);
    Value evaluateUnaryExpr(const Unary& unary);
    Value evaluateAssignExpr(const Assign& assign);
    Value evaluateLogicalExpr(const Logical& logical);
    Value evaluateCallExpr(const Call& call);

    [[nodiscard]]
    Value evaluateLambdaExpr(const Lambda& lambda) const;

    void prelude() const;

public:
    Interpreter() {
        prelude();
    }
    ~Interpreter() = default;

    ExecSig
    executeBlock(const std::vector<std::shared_ptr<Stmt>>& statements, const std::shared_ptr<Environment>& environment);

    ExecSig interpret(const std::vector<Stmt>& statements);
    void    exclude_native_func(const std::vector<std::string>& list) const;
};

struct Callable {
    virtual ExecSig call(Interpreter&, const std::vector<Value>&) const = 0;

    [[nodiscard]]
    virtual size_t arity() const = 0;

    [[nodiscard]]
    virtual std::string to_string() const = 0;

    virtual ~Callable() = default;
};

struct Func : Callable {
    const std::vector<Token>                 params;
    const std::vector<std::shared_ptr<Stmt>> body;
    const std::shared_ptr<Environment>       closure;
    const std::string                        name;

    // using constructor to perform move semantics
    Func(
        std::vector<Token>                 params,
        std::vector<std::shared_ptr<Stmt>> body,
        std::shared_ptr<Environment>       closure,
        std::string                        name)
        : params(std::move(params)), body(std::move(body)), closure(std::move(closure)), name(std::move(name)) {}

    ExecSig call(Interpreter& interpreter, const std::vector<Value>& arguments) const override {
        const auto function_env = std::make_shared<Environment>(closure);
        for(size_t i = 0; i < params.size(); ++i) {
            function_env->define(params[i], arguments[i]);
        }
        return interpreter.executeBlock(body, function_env);
    }

    [[nodiscard]] size_t arity() const override {
        return params.size();
    }

    [[nodiscard]] std::string to_string() const override {
        return "<function " + name + ">";
    }
};

struct LambdaFunc final : Func {
    const std::vector<Token>                 params;
    const std::vector<std::shared_ptr<Stmt>> body;
    const std::shared_ptr<Environment>       closure;

    LambdaFunc(std::vector<Token> params, std::vector<std::shared_ptr<Stmt>> body, std::shared_ptr<Environment> closure)
        : Func(std::move(params), std::move(body), std::move(closure), "lambda") {}
};

struct NativeFunc final : Callable {
    const size_t                                                          _arity;
    const std::function<ExecSig(Interpreter&, const std::vector<Value>&)> func;

    // using constructor to perform move semantics
    NativeFunc(const size_t arity, std::function<ExecSig(Interpreter&, const std::vector<Value>&)> func)
        : _arity(arity), func(std::move(func)) {}

    ExecSig call(Interpreter& interpreter, const std::vector<Value>& arguments) const override {
        return func(interpreter, arguments);
    }

    [[nodiscard]] size_t arity() const override {
        return _arity;
    }

    [[nodiscard]] std::string to_string() const override {
        return "<function native>";
    }
};
