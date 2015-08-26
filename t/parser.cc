#include "../src/ast.h"
#include "../src/parser.h"

#include <gtest/gtest.h>
#include <memory>
#include <sstream>

TEST(Parser, Ctor)
{
    std::istringstream s("");
    EXPECT_NO_THROW(auto p = Parser(s));
}

TEST(Parser, Token1)
{
    std::istringstream s("A.f(x,y) 3.14e-2 + (\"wu\" - 4) /r!\t ");
    auto p = Parser(s);
    EXPECT_EQ(Parser::TK::SYMBOL, p.token());
    EXPECT_EQ(Parser::TK::REAL, p.token());
    EXPECT_EQ(Parser::TK::OP, p.token());
    EXPECT_EQ(Parser::TK::BRACKET_OPEN, p.token());
    EXPECT_EQ(Parser::TK::STRING, p.token());
    EXPECT_EQ(Parser::TK::OP, p.token());
    EXPECT_EQ(Parser::TK::REAL, p.token());
    EXPECT_EQ(Parser::TK::BRACKET_CLOSE, p.token());
    EXPECT_EQ(Parser::TK::OP, p.token());
    EXPECT_EQ(Parser::TK::SYMBOL, p.token());
    EXPECT_EQ(Parser::TK::OP, p.token());
    EXPECT_EQ(Parser::TK::END, p.token());
}

TEST(Parser, Token2)
{
    std::istringstream s("A.f(x,y, \")\") 3.14e-2");
    auto p = Parser(s);
    EXPECT_EQ(Parser::TK::SYMBOL, p.token());
    EXPECT_EQ(Parser::TK::REAL, p.token());
    EXPECT_EQ(Parser::TK::END, p.token());
}

TEST(Parser, Token3)
{
    std::istringstream s("A.f(x,y, (((\")\")))) 3.14e-2");
    auto p = Parser(s);
    EXPECT_EQ(Parser::TK::SYMBOL, p.token());
    EXPECT_EQ(Parser::TK::REAL, p.token());
    EXPECT_EQ(Parser::TK::END, p.token());
}

TEST(Parser, Token4)
{
    std::istringstream s("A.f(x,y, (((\")\"))) 3.14e-2");
    auto p = Parser(s);
    EXPECT_EQ(Parser::TK::ERROR, p.token());
}

TEST(Parser, Token5)
{
    std::istringstream s("A.f(x,y, \") 3.14e-2");
    auto p = Parser(s);
    EXPECT_EQ(Parser::TK::ERROR, p.token());
}

TEST(Parser, AtomicExpr1)
{
    std::istringstream s("A.f(x,(y),z).x");
    auto p = Parser(s);
    auto t = p.parseAtomicExpr();
    EXPECT_EQ(Ast::T::SYMBOL, t->t);
    EXPECT_EQ("A.f(x,(y),z).x", t->str);
}

TEST(Parser, AtomicExpr2)
{
    std::istringstream s("3.14e-3");
    auto p = Parser(s);
    auto t = p.parseAtomicExpr();
    EXPECT_EQ(Ast::T::REAL, t->t);
    EXPECT_DOUBLE_EQ(3.14e-3, t->num);
}

TEST(Parser, AtomicExpr3)
{
    std::istringstream s("true && false || 0");
    auto p = Parser(s);
    auto t = p.parseAtomicExpr();
    EXPECT_EQ(Ast::T::BOOLEAN, t->t);
    EXPECT_EQ(true, t->b);
    t = p.parseAtomicExpr();
    EXPECT_EQ(Ast::T::SYMBOL, t->t);
    EXPECT_EQ(Ast::O::LOGICAL_AND, t->op);
    t = p.parseAtomicExpr();
    EXPECT_EQ(Ast::T::BOOLEAN, t->t);
    EXPECT_EQ(false, t->b);
    t = p.parseAtomicExpr();
    EXPECT_EQ(Ast::T::SYMBOL, t->t);
    EXPECT_EQ(Ast::O::LOGICAL_OR, t->op);
    t = p.parseAtomicExpr();
    EXPECT_EQ(Ast::T::OPERATOR, t->t);
    EXPECT_EQ(0, t->num);
}
