#include "interpreter/interpreter.hpp"
#include "types/error.hpp"
#include "types/error_code.hpp"
#include "utils/errorx.hpp"

#include <format>

bool Environment::contains(const std::string& name) const {
    return variables.contains(name);
}

void Environment::define(const std::string& name, const Value& value) {
    if(variables.contains(name))
        throw err::make(
            err::DUPLICATE_VAR,
            std::format("variable/function '{}' already declared in this scope.", name),
            -1);
    variables[name] = value;
}

void Environment::define(const Token& name, const Value& value) {
    if(variables.contains(name.lexeme))
        throw err::make(
            err::DUPLICATE_VAR,
            std::format("variable/function '{}' already declared in this scope.", name.lexeme),
            name.line);
    variables[name.lexeme] = value;
}

Value Environment::get(std::string name) {
    if(variables.contains(name))
        return variables[name];
    if(enclosing)
        return enclosing->get(name);
    throw Error(err::UNDEFINED_VAR, std::format("Undefined variable '{}'.", name));
}

void Environment::assign(std::string name, const Value& value) {
    if(variables.contains(name)) {
        variables[name] = value;
        return;
    }
    if(enclosing) {
        enclosing->assign(name, value);
        return;
    }
    throw Error(err::UNDEFINED_VAR, std::format("Undefined variable '{}'.", name));
}

void Environment::remove(const std::string& name) {
    if(variables.contains(name)) {
        variables.erase(name);
    }
}