#include "ast.h"
#include <memory>

Ast::Ast() : t(Ast::T::UNKNOWN)
{
}

Ast::Ast(double v) : t(Ast::T::REAL), val(v)
{
}

Ast::Ast(const std::string &s) : t(Ast::T::UNKNOWN), str(s)
{
}

Ast::Ast(Ast::O o) : t(Ast::T::OPERATOR), op(o)
{
}

std::unique_ptr<Ast> Ast::make(Ast::O o)
{
    return std::unique_ptr<Ast>(new Ast(o));
}

std::unique_ptr<Ast> Ast::make(double v)
{
    return std::unique_ptr<Ast>(new Ast(v));
}

std::unique_ptr<Ast> Ast::makeString(const std::string &s)
{
    auto r = std::unique_ptr<Ast>(new Ast(s));
    r->t =Ast::T::STRING;
    return std::unique_ptr<Ast>(std::move(r));
}

std::unique_ptr<Ast> Ast::makeSymbol(const std::string &s)
{
    auto r = std::unique_ptr<Ast>(new Ast(s));
    r->t =Ast::T::SYMBOL;
    return std::unique_ptr<Ast>(std::move(r));
}
