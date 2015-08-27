#include "interface.h"
#include "parser.h"
#include "ast.h"

#include <sstream>
#include <utility>
#include <string>

#include <parameter.h> // ariadne code

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
            impl_->msg_ = "unprocessed components on the end, "
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

std::pair<std::shared_ptr<parameter>, std::string>
Expression::eval(const Expression::Dict &dict)
{
    std::shared_ptr<parameter> rp;
    impl_->hasError_ = false;
    if (!impl_->ast_) {
        impl_->hasError_ = true;
        impl_->msg_ ="parse failed or no given expression";
        return std::make_pair(rp, impl_->msg_);
    }
    Ast::Dict d;
    for (const auto &i : dict) {
        switch (i.second->getType()) {
            case PT_REAL:
                d[i.first] = Ast::make(i.second->getValueReal());
                break;
            case PT_STRING:
                d[i.first] = Ast::makeString(i.second->getValueString());
                break;
            // case PT_BOOL:break;
            default:
                impl_->hasError_ = true;
                impl_->msg_ = "unrecognizable parameter type";
                return std::make_pair(rp, impl_->msg_);
        }
    }
    auto r = ::eval(impl_->ast_, d, impl_->msg_ );
    if (!r) {
        if (impl_->ast_) {
            impl_->hasError_ = true;
            return std::make_pair(rp, impl_->msg_);
        }
    }
    switch (r->t) {
        case Ast::T::NUMBER:
            rp = std::make_shared<parameter>();
            rp->setValueReal(r->num);
            break;
        case Ast::T::STRING:
            rp = std::make_shared<parameter>();
            rp->setValueString(r->str);
            break;
        case Ast::T::BOOLEAN:
            rp = std::make_shared<parameter>();
            rp->setValueReal(r->b);
            break;
        default:
            break;
    }
    return std::make_pair(rp, impl_->msg_);
}
