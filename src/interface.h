#ifndef ARIADNE_PARSER_INTERFACE_H
#define ARIADNE_PARSER_INTERFACE_H

#include <memory>
#include <utility>
#include <string>

#if 1
enum class parameter_type
{PT_UNDEFINED, PT_CHAR, PT_INTEGER, PT_REAL, PT_STRING};
#endif

class Ast;
std::pair<std::unique_ptr<Ast>, std::string> parse(const std::string &expr);

#endif
