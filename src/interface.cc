#include "interface.h"
#include "parser.h"
#include "ast.h"

#include <sstream>
#include <utility>
#include <string>

struct ExpressionImpl
{
    std::unique_ptr<Ast> ast_;
    bool hasError_;
    std::string msg_;
};

Expression::Expression()
    : impl_(std::make_shared<ExpressionImpl>())
{
    impl_->hasError_ = true;
    impl_->msg_ = "no expression is given";
}

Expression::Expression(const std::string &expr)
    : impl_(std::make_shared<ExpressionImpl>())
{
    parse(expr);
}

bool Expression::parse(const std::string &expr)
{
    std::istringstream s(expr);
    auto p = Parser(s);
    impl_->ast_ = p.parseExpr();
    if (p.token() != Parser::TK::END) {
        impl_->hasError_ = true;
        impl_->msg_ = "gabages after the end of the expression";
    }
    if (impl_->ast_) {
        impl_->hasError_ = false;
        impl_->msg_ = "no error";
    } else {
        impl_->hasError_ = true;
        impl_->msg_ = p.msg();
    }
    return !impl_->hasError_;
}

Expression::operator bool() const
{
    return !impl_->hasError_;
}