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
    EXPECT_TRUE(static_cast<bool>(t));
    EXPECT_EQ(Ast::T::SYMBOL, t->t);
    EXPECT_EQ("A.f(x,(y),z).x", t->str);
}

TEST(Parser, AtomicExpr2)
{
    std::istringstream s("3.14e-3");
    auto p = Parser(s);
    auto t = p.parseAtomicExpr();
    EXPECT_TRUE(static_cast<bool>(t));
    EXPECT_EQ(Ast::T::REAL, t->t);
    EXPECT_DOUBLE_EQ(3.14e-3, t->num);
}

TEST(Parser, parseDeniableAtomicExpr1)
{
    std::istringstream s("!false");
    auto p = Parser(s);
    auto t = p.parseDeniableAtomicExpr();
    EXPECT_TRUE(static_cast<bool>(t));
    EXPECT_EQ(Ast::T::OPERATOR, t->t);
    EXPECT_EQ(Ast::O::LOGICAL_NOT, t->op);
    EXPECT_FALSE(t->left);
    EXPECT_EQ(Ast::T::BOOLEAN, t->right->t);
    EXPECT_FALSE(t->right->b);
}

#if 0 // will be enabled in the future
TEST(Parser, parseDeniableAtomicExpr2)
{
    std::istringstream s("!(false)");
    auto p = Parser(s);
    auto t = p.parseDeniableAtomicExpr();
    EXPECT_TRUE(static_cast<bool>(t));
    EXPECT_EQ(Ast::T::OPERATOR, t->t);
    EXPECT_EQ(Ast::O::LOGICAL_NOT, t->op);
    EXPECT_FALSE(t->left);
    EXPECT_EQ(Ast::T::BOOLEAN, t->right->t);
    EXPECT_FALSE(t->right->b);
}
#endif

TEST(Parser, parseDeniableAtomicExpr3)
{
    std::istringstream s("-1");
    auto p = Parser(s);
    auto t = p.parseDeniableAtomicExpr();
    EXPECT_TRUE(static_cast<bool>(t));
    EXPECT_EQ(Ast::T::OPERATOR, t->t);
    EXPECT_EQ(Ast::O::MINUS, t->op);
    EXPECT_FALSE(t->left);
    EXPECT_EQ(Ast::T::REAL, t->right->t);
    EXPECT_EQ(1, t->right->num);
}

TEST(Parser, parseDeniableAtomicExpr4)
{
    std::istringstream s("+1");
    auto p = Parser(s);
    auto t = p.parseDeniableAtomicExpr();
    EXPECT_TRUE(static_cast<bool>(t));
    EXPECT_EQ(Ast::T::REAL, t->t);
    EXPECT_EQ(1, t->num);
}

TEST(Parser, parseDeniableAtomicExpr5)
{
    std::istringstream s("true");
    auto p = Parser(s);
    auto t = p.parseDeniableAtomicExpr();
    EXPECT_TRUE(static_cast<bool>(t));
    EXPECT_EQ(Ast::T::BOOLEAN, t->t);
    EXPECT_TRUE(t->b);
}

TEST(Parser, parsePotExpr1)
{
    for (auto str : {"a^b", " a ^ b", "a ^b", "a^ b"}) {
        std::istringstream s(str);
        auto p = Parser(s);
        auto t = p.parsePotExpr();
        EXPECT_TRUE(static_cast<bool>(t));
        EXPECT_EQ(Ast::T::OPERATOR, t->t);
        EXPECT_EQ(Ast::O::POWER, t->op);
        EXPECT_TRUE(static_cast<bool>(t->left));
        EXPECT_EQ(Ast::T::SYMBOL, t->left->t);
        EXPECT_EQ("a", t->left->str);
        EXPECT_TRUE(static_cast<bool>(t->right));
        EXPECT_EQ(Ast::T::SYMBOL, t->right->t);
        EXPECT_EQ("b", t->right->str);
    }
}

TEST(Parser, parsePotExpr2)
{
    for (auto str : {"a^-b", " a ^ -b", "a ^-b", "a^- b"}) {
        std::istringstream s(str);
        auto p = Parser(s);
        auto t = p.parsePotExpr();
        EXPECT_TRUE(static_cast<bool>(t));
        EXPECT_EQ(Ast::T::OPERATOR, t->t);
        EXPECT_EQ(Ast::O::POWER, t->op);
        EXPECT_TRUE(static_cast<bool>(t->left));
        EXPECT_EQ(Ast::T::SYMBOL, t->left->t);
        EXPECT_EQ("a", t->left->str);
        EXPECT_TRUE(static_cast<bool>(t->right));
        EXPECT_EQ(Ast::T::OPERATOR, t->right->t);
        EXPECT_EQ(Ast::O::MINUS, t->right->op);
        EXPECT_FALSE(static_cast<bool>(t->right->left));
        EXPECT_TRUE(static_cast<bool>(t->right->right));
        EXPECT_EQ(Ast::T::SYMBOL, t->right->right->t);
        EXPECT_EQ("b", t->right->right->str);
    }
}
