#ifndef ARIADNE_PARSER_INTERFACE_H
#define ARIADNE_PARSER_INTERFACE_H

#include <memory>
#include <utility>
#include <string>

#if 1
enum class parameter_type
{PT_UNDEFINED, PT_CHAR, PT_INTEGER, PT_REAL, PT_STRING};
#endif

struct ExpressionImpl;
class Expression {
public:
    Expression();
    Expression(const std::string &expr);
    operator bool() const;
    bool parse(const std::string &expr);
private:
    std::shared_ptr<ExpressionImpl> impl_;
};

#endif
