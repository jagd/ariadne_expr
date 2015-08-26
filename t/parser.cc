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
    EXPECT_EQ(Parser::TK::NUMBER, p.token());
    EXPECT_EQ(Parser::TK::OP, p.token());
    EXPECT_EQ(Parser::TK::BRACKET_OPEN, p.token());
    EXPECT_EQ(Parser::TK::STRING, p.token());
    EXPECT_EQ(Parser::TK::OP, p.token());
    EXPECT_EQ(Parser::TK::NUMBER, p.token());
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
    EXPECT_EQ(Parser::TK::NUMBER, p.token());
    EXPECT_EQ(Parser::TK::END, p.token());
}

TEST(Parser, Token3)
{
    std::istringstream s("A.f(x,y, (((\")\")))) 3.14e-2");
    auto p = Parser(s);
    EXPECT_EQ(Parser::TK::SYMBOL, p.token());
    EXPECT_EQ(Parser::TK::NUMBER, p.token());
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
    EXPECT_EQ(Ast::T::NUMBER, t->t);
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
    EXPECT_FALSE(static_cast<bool>(t->left)) << p.msg();
    EXPECT_EQ(Ast::T::BOOLEAN, t->right->t);
    EXPECT_FALSE(t->right->b);
}

TEST(Parser, parseDeniableAtomicExpr2)
{
    std::istringstream s("!(false)");
    auto p = Parser(s);
    auto t = p.parseDeniableAtomicExpr();
    EXPECT_TRUE(static_cast<bool>(t)) << p.msg();
    EXPECT_EQ(Ast::T::OPERATOR, t->t);
    EXPECT_EQ(Ast::O::LOGICAL_NOT, t->op);
    EXPECT_FALSE(static_cast<bool>(t->left)) << p.msg();
    EXPECT_TRUE(static_cast<bool>(t->right)) << p.msg();
    EXPECT_EQ(Ast::T::BOOLEAN, t->right->t);
    EXPECT_FALSE(t->right->b);
}

TEST(Parser, parseDeniableAtomicExpr3)
{
    std::istringstream s("-1");
    auto p = Parser(s);
    auto t = p.parseDeniableAtomicExpr();
    EXPECT_TRUE(static_cast<bool>(t));
    EXPECT_EQ(Ast::T::OPERATOR, t->t);
    EXPECT_EQ(Ast::O::MINUS, t->op);
    EXPECT_FALSE(static_cast<bool>(t->left)) << p.msg();
    EXPECT_EQ(Ast::T::NUMBER, t->right->t);
    EXPECT_EQ(1, t->right->num);
}

TEST(Parser, parseDeniableAtomicExpr4)
{
    std::istringstream s("+1");
    auto p = Parser(s);
    auto t = p.parseDeniableAtomicExpr();
    EXPECT_TRUE(static_cast<bool>(t));
    EXPECT_EQ(Ast::T::NUMBER, t->t);
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

TEST(Parser, parseMulDivModExpr1)
{
    std::istringstream s("a^-b%-c^-d");
    auto p = Parser(s);
    auto t = p.parseMulDivModExpr();

    EXPECT_TRUE(static_cast<bool>(t));
    EXPECT_EQ(Ast::T::OPERATOR, t->t);
    EXPECT_EQ(Ast::O::MODULUS, t->op);

    const auto &l = t->left;
    EXPECT_TRUE(static_cast<bool>(l));
    EXPECT_EQ(Ast::T::OPERATOR, l->t);
    EXPECT_EQ(Ast::O::POWER, l->op);
    EXPECT_TRUE(static_cast<bool>(l->left));
    EXPECT_EQ(Ast::T::SYMBOL, l->left->t);
    EXPECT_EQ("a", l->left->str);
    EXPECT_TRUE(static_cast<bool>(l->right));
    EXPECT_EQ(Ast::T::OPERATOR, l->right->t);
    EXPECT_EQ(Ast::O::MINUS, l->right->op);
    EXPECT_FALSE(static_cast<bool>(l->right->left));
    EXPECT_TRUE(static_cast<bool>(l->right->right));
    EXPECT_EQ(Ast::T::SYMBOL, l->right->right->t);
    EXPECT_EQ("b", l->right->right->str);

    const auto &r = t->right;
    EXPECT_TRUE(static_cast<bool>(r));
    EXPECT_EQ(Ast::T::OPERATOR, r->t);
    EXPECT_EQ(Ast::O::MINUS, r->op);
    EXPECT_FALSE(static_cast<bool>(r->left));

    const auto &rr = r->right;
    EXPECT_TRUE(static_cast<bool>(rr));
    EXPECT_EQ(Ast::T::OPERATOR, rr->t);
    EXPECT_EQ(Ast::O::POWER, rr->op);
    EXPECT_TRUE(static_cast<bool>(rr->left));
    EXPECT_EQ(Ast::T::SYMBOL, rr->left->t);
    EXPECT_EQ("c", rr->left->str);
    EXPECT_TRUE(static_cast<bool>(rr->right));
    EXPECT_EQ(Ast::T::OPERATOR, rr->right->t);
    EXPECT_EQ(Ast::O::MINUS, rr->right->op);
    EXPECT_FALSE(static_cast<bool>(rr->right->left));
    EXPECT_TRUE(static_cast<bool>(rr->right->right));
    EXPECT_EQ(Ast::T::SYMBOL, rr->right->right->t);
    EXPECT_EQ("d", rr->right->right->str);
}

TEST(Parser, parseMulDivModExpr2)
{
    for (auto str : {"a*-b", " a * -b", "a *-b", "a*- b"}) {
        std::istringstream s(str);
        auto p = Parser(s);
        auto t = p.parseMulDivModExpr();
        EXPECT_TRUE(static_cast<bool>(t));
        EXPECT_EQ(Ast::T::OPERATOR, t->t);
        EXPECT_EQ(Ast::O::MULTIPLY, t->op);
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

TEST(Parser, parseMulDivModExpr3)
{
    for (auto str : {"a/-b", " a / -b", "a /-b", "a/- b"}) {
        std::istringstream s(str);
        auto p = Parser(s);
        auto t = p.parseMulDivModExpr();
        EXPECT_TRUE(static_cast<bool>(t));
        EXPECT_EQ(Ast::T::OPERATOR, t->t);
        EXPECT_EQ(Ast::O::DIVISION, t->op);
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

TEST(Parser, parsePlusMinusExpr1)
{
    for (auto str : {"a+-b", " a + -b", "a +-b", "a+- b"}) {
        std::istringstream s(str);
        auto p = Parser(s);
        auto t = p.parsePlusMinusExpr();
        EXPECT_TRUE(static_cast<bool>(t));
        EXPECT_EQ(Ast::T::OPERATOR, t->t);
        EXPECT_EQ(Ast::O::PLUS, t->op);
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

TEST(Parser, parsePlusMinusExpr2)
{
    for (auto str : {"a--b", " a - -b", "a --b", "a-- b"}) {
        std::istringstream s(str);
        auto p = Parser(s);
        auto t = p.parsePlusMinusExpr();
        EXPECT_TRUE(static_cast<bool>(t));
        EXPECT_EQ(Ast::T::OPERATOR, t->t);
        EXPECT_EQ(Ast::O::MINUS, t->op);
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

TEST(Parser, parseCmpExpr)
{
    const char *expr[] = {"a==-b", "a!=-b", "a<-b", "a<=-b", "a>-b", "a>=-b"};
    const Ast::O op[] = {Ast::O::CMP_EQ, Ast::O::CMP_NE, Ast::O::CMP_LT,
                         Ast::O::CMP_LE, Ast::O::CMP_GT, Ast::O::CMP_GE};
    // avoid to introduce boost::zip
    for (int i = 0; i < 6; ++i) {
        std::istringstream s(&expr[i][0]);
        auto p = Parser(s);
        auto t = p.parseCmpExpr();
        EXPECT_TRUE(static_cast<bool>(t));
        EXPECT_EQ(Ast::T::OPERATOR, t->t);
        EXPECT_EQ(op[i], t->op);
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

TEST(Parser, parseExpr)
{
    for (auto str : {"a+c^(2*b.x.f(y+x))!=3||b||c", "a&&b||c^2^3^4*5"}) {
        std::istringstream s(str);
        auto p = Parser(s);
        auto t = p.parseExpr();
        EXPECT_TRUE(static_cast<bool>(t));
    }
}
