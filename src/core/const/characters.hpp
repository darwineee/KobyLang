#pragma once
#include <string>

namespace symbol {

constexpr char LeftParen  = '(';
constexpr char RightParen = ')';
constexpr char LeftBrace  = '{';
constexpr char RightBrace = '}';
constexpr char Plus       = '+';
constexpr char Minus      = '-';
constexpr char Comma      = ',';
constexpr char Dot        = '.';
constexpr char Semicolon  = ';';
constexpr char Star       = '*';
constexpr char Slash      = '/';

constexpr char Question    = '?';

} // namespace symbol

namespace op {

constexpr char Bang    = '!';
constexpr char Equal   = '=';
constexpr char Greater = '>';
constexpr char Less    = '<';

constexpr std::string BangEqual    = "!=";
constexpr std::string EqualEqual   = "==";
constexpr std::string GreaterEqual = ">=";
constexpr std::string LessEqual    = "<=";

constexpr std::string Arrow = "->";

} // namespace op

namespace keyword {

constexpr std::string And      = "and";
constexpr std::string Or       = "or";
constexpr std::string If       = "if";
constexpr std::string Else     = "else";
constexpr std::string True     = "true";
constexpr std::string False    = "false";
constexpr std::string Class    = "class";
constexpr std::string This     = "this";
constexpr std::string Super    = "super";
constexpr std::string Fun      = "fun";
constexpr std::string Var      = "var";
constexpr std::string For      = "for";
constexpr std::string While    = "while";
constexpr std::string Break    = "break";
constexpr std::string Continue = "continue";
constexpr std::string Return   = "return";
constexpr std::string Nil      = "nil";

} // namespace keyword

namespace delimiter {

constexpr char SPACE           = ' ';
constexpr char TAB             = '\t';
constexpr char NEWLINE         = '\n';
constexpr char CARRIAGE_RETURN = '\r';
constexpr char ENDOF           = EOF;

} // namespace delimiter

namespace surround {

constexpr char SingleQuote = '\'';
constexpr char DoubleQuote = '"';

} // namespace surround

constexpr std::string EMPTY;