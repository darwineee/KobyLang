#include "interpreter/interpreter.hpp"
#include "types/error.hpp"
#include "types/error_code.hpp"

#include <format>

void Environment::define(const std::string& name, const Value& value) {
    variables[name] = value;
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