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
    if (impl_->ast_) {
        if (!p.eof()) {
            impl_->hasError_ = true;
            impl_->msg_ = "unprocessed components on the end,"
                "maybe there is more than one expression given";
            return false;
        }
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

std::set<std::string> Expression::symbols() const
{
    impl_->hasError_ = false;
    return ::symbols(impl_->ast_);
}

const std::string Expression::msg() const
{
    return impl_->msg_;
}
