#pragma once

namespace err {

constexpr int NONE = 0;

// Tokenizer errors: 1-100
constexpr int LEXICAL_ERROR       = 1;
constexpr int UNTERMINATED_STRING = 2;

// Parser errors: 101-200
constexpr int UNKNOWN_PARSING_ERROR     = 101;
constexpr int EXPR_NOT_CLOSED           = 102;
constexpr int BLOCK_NOT_CLOSED          = 103;
constexpr int VAR_NAME_MISSING          = 104;
constexpr int SEMICOLON_MISSING         = 105;
constexpr int INVALID_ASSIGNMENT_TARGET = 106;
constexpr int IF_COND_MISSING_PAREN     = 107;
constexpr int WHILE_COND_MISSING_PAREN  = 108;
constexpr int FOR_COND_MISSING_PAREN    = 109;
constexpr int BREAK_OUTSIDE_LOOP        = 110;
constexpr int CONTINUE_OUTSIDE_LOOP     = 111;
constexpr int CALL_NOT_CLOSED           = 112;
constexpr int TOO_MANY_ARGUMENTS        = 113;
constexpr int NAMED_FUNC_MISSING_NAME   = 114;
constexpr int FUNC_PARAMS_MISSING_PAREN = 115;
constexpr int FUNC_PARAM_MISSING_NAME   = 116;

// Interpreter errors: 201-300
constexpr int OPERAND_INVALID = 201;
constexpr int UNDEFINED_VAR   = 202;
constexpr int ARGUMENT_COUNT_MISMATCH = 203;
constexpr int NOT_CALLABLE = 204;

} // namespace err
