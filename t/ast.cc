#include "../src/ast.h"
#include "../src/parser.h"

#include <gtest/gtest.h>

TEST(Ast, Ctor)
{
    EXPECT_NO_THROW(Ast());
    EXPECT_NO_THROW(Ast(1.0));
    EXPECT_NO_THROW(Ast(Ast::O::CMP_EQ));
    EXPECT_NO_THROW(Ast("Chuanren Wu"));
}

TEST(Ast, Make)
{
    EXPECT_EQ(Ast::T::OPERATOR, Ast::make(Ast::O::CMP_LE)->t);
    EXPECT_EQ(Ast::O::CMP_LE, Ast::make(Ast::O::CMP_LE)->op);
    EXPECT_EQ(Ast::T::BOOLEAN, Ast::make(false)->t);
    EXPECT_EQ("wu", Ast::makeString("wu")->str);
    EXPECT_EQ(Ast::T::STRING, Ast::makeString("wu")->t);
    EXPECT_EQ("wu", Ast::makeSymbol("wu")->str);
    EXPECT_EQ(Ast::T::SYMBOL, Ast::makeSymbol("wu")->t);
}

TEST(Ast, EvalUni1)
{
    std::istringstream s("!true");
    auto p = Parser(s);
    auto t = p.parseExpr();
    EXPECT_TRUE(static_cast<bool>(t));
    std::string msg;
    auto v = eval(t, Ast::Dict(), msg);
    EXPECT_TRUE(static_cast<bool>(v));
    EXPECT_EQ(Ast::T::BOOLEAN, v->t);
    EXPECT_FALSE(v->b);
}

TEST(Ast, EvalUni2)
{
    std::istringstream s("!(false)");
    auto p = Parser(s);
    auto t = p.parseExpr();
    EXPECT_TRUE(static_cast<bool>(t));
    std::string msg;
    auto v = eval(t, Ast::Dict(), msg);
    EXPECT_TRUE(static_cast<bool>(v));
    EXPECT_EQ(Ast::T::BOOLEAN, v->t);
    EXPECT_TRUE(v->b);
}

TEST(Ast, EvalUni3)
{
    std::istringstream s("+2");
    auto p = Parser(s);
    auto t = p.parseExpr();
    EXPECT_TRUE(static_cast<bool>(t));
    std::string msg;
    auto v = eval(t, Ast::Dict(), msg);
    EXPECT_TRUE(static_cast<bool>(v));
    EXPECT_EQ(Ast::T::NUMBER, v->t);
    EXPECT_EQ(2, v->num);
}

TEST(Ast, EvalUni4)
{
    std::istringstream s("-2");
    auto p = Parser(s);
    auto t = p.parseExpr();
    EXPECT_TRUE(static_cast<bool>(t));
    std::string msg;
    auto v = eval(t, Ast::Dict(), msg);
    EXPECT_TRUE(static_cast<bool>(v));
    EXPECT_EQ(Ast::T::NUMBER, v->t);
    EXPECT_EQ(-2, v->num);
}

TEST(Ast, EvalUniFail)
{
    for (const auto str : {
        "+true", "+\"a\"", "!2", "!\"a\"", "-true", "-\"a\""
    }) {
        std::istringstream s(str);
        auto p = Parser(s);
        auto t = p.parseExpr();
        EXPECT_TRUE(static_cast<bool>(t)) << str;
        std::string msg;
        auto d = Ast::Dict();
        auto v = eval(t, d, msg);
        EXPECT_FALSE(static_cast<bool>(v)) << str;
    }
}

TEST(Ast, EvalAdd)
{
    std::istringstream s("1+2+3");
    auto p = Parser(s);
    auto t = p.parseExpr();
    EXPECT_TRUE(static_cast<bool>(t));
    std::string msg;
    auto v = eval(t, Ast::Dict(), msg);
    EXPECT_TRUE(static_cast<bool>(v));
    EXPECT_EQ(Ast::T::NUMBER, v->t);
    EXPECT_EQ(6, v->num);
}

TEST(Ast, EvalAdd2)
{
    std::istringstream s("1+a+3");
    auto p = Parser(s);
    auto t = p.parseExpr();
    EXPECT_TRUE(static_cast<bool>(t));
    std::string msg;
    auto d = Ast::Dict();
    d["a"] = Ast::make(2.0);
    auto v = eval(t, d, msg);
    EXPECT_TRUE(static_cast<bool>(v));
    EXPECT_EQ(Ast::T::NUMBER, v->t);
    EXPECT_EQ(6, v->num);
}

TEST(Ast, EvalAdd3)
{
    std::istringstream s("1+a+3");
    auto p = Parser(s);
    auto t = p.parseExpr();
    EXPECT_TRUE(static_cast<bool>(t));
    std::string msg;
    auto d = Ast::Dict();
    d["a"] = Ast::makeString("a");
    auto v = eval(t, d, msg);
    EXPECT_TRUE(static_cast<bool>(v));
    EXPECT_EQ(Ast::T::STRING, v->t);
    EXPECT_EQ("1a3", v->str);
}

TEST(Ast, EvalAddFail)
{
    for (const auto str : {
        "1+a+3", "true+1", "2+true", "true+\"false\"", "\"t\"+true"
    }) {
        std::istringstream s(str);
        auto p = Parser(s);
        auto t = p.parseExpr();
        EXPECT_TRUE(static_cast<bool>(t)) << str;
        std::string msg;
        auto d = Ast::Dict();
        auto v = eval(t, d, msg);
        EXPECT_FALSE(static_cast<bool>(v)) << str;
    }
}

TEST(Ast, EvalSub)
{
    std::istringstream s("1-2-3");
    auto p = Parser(s);
    auto t = p.parseExpr();
    EXPECT_TRUE(static_cast<bool>(t));
    std::string msg;
    auto d = Ast::Dict();
    auto v = eval(t, d, msg);
    EXPECT_TRUE(static_cast<bool>(v));
    EXPECT_EQ(Ast::T::NUMBER, v->t);
    EXPECT_EQ(-4, v->num);
}

TEST(Ast, EvalSub2)
{
    std::istringstream s("1--2-3");
    auto p = Parser(s);
    auto t = p.parseExpr();
    EXPECT_TRUE(static_cast<bool>(t));
    std::string msg;
    auto d = Ast::Dict();
    auto v = eval(t, d, msg);
    EXPECT_TRUE(static_cast<bool>(v));
    EXPECT_EQ(Ast::T::NUMBER, v->t);
    EXPECT_EQ(0, v->num);
}

TEST(Ast, EvalSubFail)
{
    for (const auto str : {
        "1-\"s\"", "\"s\"-1", "true-1", "2-true", "true-\"false\"",
        "\"t\"-true"
    } ) {
        std::istringstream s(str);
        auto p = Parser(s);
        auto t = p.parseExpr();
        EXPECT_TRUE(static_cast<bool>(t)) << str;
        std::string msg;
        auto d = Ast::Dict();
        auto v = eval(t, d, msg);
        EXPECT_FALSE(static_cast<bool>(v)) << str;
    }
}

TEST(Ast, EvalMul)
{
    std::istringstream s("2*2*3");
    auto p = Parser(s);
    auto t = p.parseExpr();
    EXPECT_TRUE(static_cast<bool>(t));
    std::string msg;
    auto v = eval(t, Ast::Dict(), msg);
    EXPECT_TRUE(static_cast<bool>(v));
    EXPECT_EQ(Ast::T::NUMBER, v->t);
    EXPECT_EQ(12, v->num);
}

TEST(Ast, EvalMul2)
{
    std::istringstream s("2*a*3");
    auto p = Parser(s);
    auto t = p.parseExpr();
    EXPECT_TRUE(static_cast<bool>(t));
    std::string msg;
    auto d = Ast::Dict();
    d["a"] = Ast::makeString("a");
    auto v = eval(t, d, msg);
    EXPECT_TRUE(static_cast<bool>(v));
    EXPECT_EQ(Ast::T::STRING, v->t);
    EXPECT_EQ("aaaaaa", v->str);
}

TEST(Ast, EvalMulFail)
{
    for (const auto str : {
        "true*1", "2*true", "true*\"false\"", "\"t\"*true"
    } ) {
        std::istringstream s(str);
        auto p = Parser(s);
        auto t = p.parseExpr();
        EXPECT_TRUE(static_cast<bool>(t)) << str;
        std::string msg;
        auto d = Ast::Dict();
        auto v = eval(t, d, msg);
        EXPECT_FALSE(static_cast<bool>(v)) << str;
    }
}

TEST(Ast, EvalDiv)
{
    std::istringstream s("1/2/3");
    auto p = Parser(s);
    auto t = p.parseExpr();
    EXPECT_TRUE(static_cast<bool>(t));
    std::string msg;
    auto d = Ast::Dict();
    auto v = eval(t, d, msg);
    EXPECT_TRUE(static_cast<bool>(v));
    EXPECT_EQ(Ast::T::NUMBER, v->t);
    EXPECT_DOUBLE_EQ(1.0/2.0/3.0, v->num);
}

TEST(Ast, EvalDivFail)
{
    for (const auto str : {
        "1/\"s\"", "\"s\"/1", "true/1", "2/true", "true/\"false\"",
        "\"t\"/true", "1/0"
    } ) {
        std::istringstream s(str);
        auto p = Parser(s);
        auto t = p.parseExpr();
        EXPECT_TRUE(static_cast<bool>(t)) << str;
        std::string msg;
        auto d = Ast::Dict();
        auto v = eval(t, d, msg);
        EXPECT_FALSE(static_cast<bool>(v)) << str;
    }
}

TEST(Ast, EvalMod)
{
    std::istringstream s("24%10%3");
    auto p = Parser(s);
    auto t = p.parseExpr();
    EXPECT_TRUE(static_cast<bool>(t));
    std::string msg;
    auto d = Ast::Dict();
    auto v = eval(t, d, msg);
    EXPECT_TRUE(static_cast<bool>(v));
    EXPECT_EQ(Ast::T::NUMBER, v->t);
    EXPECT_DOUBLE_EQ(1.0, v->num);
}

TEST(Ast, EvalModFail)
{
    for (const auto str : {
        "1%\"s\"", "\"s\"%1", "true%1", "2%true", "true%\"false\"",
        "\"t\"%true", "1%0"
    } ) {
        std::istringstream s(str);
        auto p = Parser(s);
        auto t = p.parseExpr();
        EXPECT_TRUE(static_cast<bool>(t)) << str;
        std::string msg;
        auto d = Ast::Dict();
        auto v = eval(t, d, msg);
        EXPECT_FALSE(static_cast<bool>(v)) << str;
    }
}

TEST(Ast, EvalPow)
{
    std::istringstream s("-2^30");
    auto p = Parser(s);
    auto t = p.parseExpr();
    EXPECT_TRUE(static_cast<bool>(t));
    std::string msg;
    auto d = Ast::Dict();
    auto v = eval(t, d, msg);
    EXPECT_TRUE(static_cast<bool>(v));
    EXPECT_EQ(Ast::T::NUMBER, v->t);
    EXPECT_EQ(-1073741824, v->num);
}

TEST(Ast, EvalPowFail)
{
    for (const auto str : {
        "1^\"s\"", "\"s\"^1", "true^1", "2^true", "true^\"false\"", "\"t\"^true"
    } ) {
        std::istringstream s(str);
        auto p = Parser(s);
        auto t = p.parseExpr();
        EXPECT_TRUE(static_cast<bool>(t)) << str;
        std::string msg;
        auto d = Ast::Dict();
        auto v = eval(t, d, msg);
        EXPECT_FALSE(static_cast<bool>(v)) << str;
    }
}

TEST(Ast, EvalAnd1)
{
    std::istringstream s("true && true");
    auto p = Parser(s);
    auto t = p.parseExpr();
    EXPECT_TRUE(static_cast<bool>(t));
    std::string msg;
    auto d = Ast::Dict();
    auto v = eval(t, d, msg);
    EXPECT_TRUE(static_cast<bool>(v));
    EXPECT_EQ(Ast::T::BOOLEAN, v->t);
    EXPECT_TRUE(v->b);
}

TEST(Ast, EvalAnd2)
{
    for (const auto str : {"false&&true", "true&&false", "false&&false"}) {
        std::istringstream s(str);
        auto p = Parser(s);
        auto t = p.parseExpr();
        EXPECT_TRUE(static_cast<bool>(t));
        std::string msg;
        auto d = Ast::Dict();
        auto v = eval(t, d, msg);
        EXPECT_TRUE(static_cast<bool>(v));
        EXPECT_EQ(Ast::T::BOOLEAN, v->t);
        EXPECT_FALSE(v->b);
    }
}


TEST(Ast, EvalAndFail)
{
    for (const auto str : {
        "1&&\"s\"", "\"s\"&&1", "true&&1", "2&&true", "true&&\"false\"",
        "\"t\"&&true", "1&&2"
    } ) {
        std::istringstream s(str);
        auto p = Parser(s);
        auto t = p.parseExpr();
        EXPECT_TRUE(static_cast<bool>(t)) << str;
        std::string msg;
        auto d = Ast::Dict();
        auto v = eval(t, d, msg);
        EXPECT_FALSE(static_cast<bool>(v)) << str;
    }
}

TEST(Ast, EvalOr1)
{
    std::istringstream s("false && false");
    auto p = Parser(s);
    auto t = p.parseExpr();
    EXPECT_TRUE(static_cast<bool>(t));
    std::string msg;
    auto d = Ast::Dict();
    auto v = eval(t, d, msg);
    EXPECT_TRUE(static_cast<bool>(v));
    EXPECT_EQ(Ast::T::BOOLEAN, v->t);
    EXPECT_FALSE(v->b);
}

TEST(Ast, EvalOr2)
{
    for (const auto str : {"false||true", "true||false", "true||true"}) {
        std::istringstream s(str);
        auto p = Parser(s);
        auto t = p.parseExpr();
        EXPECT_TRUE(static_cast<bool>(t));
        std::string msg;
        auto d = Ast::Dict();
        auto v = eval(t, d, msg);
        EXPECT_TRUE(static_cast<bool>(v));
        EXPECT_EQ(Ast::T::BOOLEAN, v->t);
        EXPECT_TRUE(v->b);
    }
}

TEST(Ast, EvalOrFail)
{
    for (const auto str : {
        "1||\"s\"", "\"s\"||1", "true||1", "2||true", "true||\"false\"",
        "\"t\"||true", "1||2"
    } ) {
        std::istringstream s(str);
        auto p = Parser(s);
        auto t = p.parseExpr();
        EXPECT_TRUE(static_cast<bool>(t)) << str;
        std::string msg;
        auto d = Ast::Dict();
        auto v = eval(t, d, msg);
        EXPECT_FALSE(static_cast<bool>(v)) << str;
    }
}

TEST(Ast, EvalEq1)
{
    for (const auto str : {
        "5==4+1", "true == true", "false == false", "\"a1\"==\"a\"+1",
        "5!=4+3", "!true != true", "false != true"
    }) {
        std::istringstream s(str);
        auto p = Parser(s);
        auto t = p.parseExpr();
        EXPECT_TRUE(static_cast<bool>(t)) << str;
        std::string msg;
        auto d = Ast::Dict();
        auto v = eval(t, d, msg);
        EXPECT_TRUE(static_cast<bool>(v)) << str;
        EXPECT_EQ(Ast::T::BOOLEAN, v->t) << str;
        EXPECT_TRUE(v->b) << str;
    }
}

TEST(Ast, EvalEq2)
{
    for (const auto str : {
        "5!=4+1", "true != true", "false != false", "\"a1\"!=\"a\"+1",
        "5==4+3", "!true == true", "false == true"
    }) {
        std::istringstream s(str);
        auto p = Parser(s);
        auto t = p.parseExpr();
        EXPECT_TRUE(static_cast<bool>(t));
        std::string msg;
        auto d = Ast::Dict();
        auto v = eval(t, d, msg);
        EXPECT_TRUE(static_cast<bool>(v));
        EXPECT_EQ(Ast::T::BOOLEAN, v->t);
        EXPECT_FALSE(v->b);
    }
}

TEST(Ast, EvalEqFail)
{
    for (const auto str : {
        "1 == \"1\"",  "1 == true", "\"true\"==true",
        "1 != \"1\"",  "1 != true", "\"true\"!=true"
    } ) {
        std::istringstream s(str);
        auto p = Parser(s);
        auto t = p.parseExpr();
        EXPECT_TRUE(static_cast<bool>(t)) << str;
        std::string msg;
        auto d = Ast::Dict();
        auto v = eval(t, d, msg);
        EXPECT_FALSE(static_cast<bool>(v)) << str;
    }
}

TEST(Ast, EvalCmp1)
{
    for (const auto str : {
        "4>3", "\"b\">\"a\"", "\"a\"*3>\"a\"",
        "3<4", "\"a\"<\"b\"", "\"a\"<\"a\"*3",
        "4>=3", "\"b\">=\"a\"", "\"a\"*3>=\"a\"",
        "3<=4", "\"a\"<=\"b\"", "\"a\"<=\"a\"*3"
    }) {
        std::istringstream s(str);
        auto p = Parser(s);
        auto t = p.parseExpr();
        EXPECT_TRUE(static_cast<bool>(t)) << str;
        std::string msg;
        auto d = Ast::Dict();
        auto v = eval(t, d, msg);
        EXPECT_TRUE(static_cast<bool>(v)) << str;
        EXPECT_EQ(Ast::T::BOOLEAN, v->t) << str;
        EXPECT_TRUE(v->b) << str;
    }
}

TEST(Ast, EvalCmp2)
{
    for (const auto str : {
        "4<3", "\"b\"<\"a\"", "\"a\"*3<\"a\"",
        "3>4", "\"a\">\"b\"", "\"a\">\"a\"*3",
        "4<=3", "\"b\"<=\"a\"", "\"a\"*3<=\"a\"",
        "3>=4", "\"a\">=\"b\"", "\"a\">=\"a\"*3"
    }) {
        std::istringstream s(str);
        auto p = Parser(s);
        auto t = p.parseExpr();
        EXPECT_TRUE(static_cast<bool>(t));
        std::string msg;
        auto d = Ast::Dict();
        auto v = eval(t, d, msg);
        EXPECT_TRUE(static_cast<bool>(v)) << str;
        EXPECT_EQ(Ast::T::BOOLEAN, v->t) << str;
        EXPECT_FALSE(v->b) << str;
    }
}

TEST(Ast, EvalCmqFail)
{
    for (const auto str : {
        "true > false", "true >= false", "true < false", "true <= false",
        "1<\"2\"", "1>\"2\"", "1<=\"2\"", "1>=\"2\"",
        "true<\"2\"", "false>\"2\"", "false<=\"2\"", "true>=\"2\""
    } ) {
        std::istringstream s(str);
        auto p = Parser(s);
        auto t = p.parseExpr();
        EXPECT_TRUE(static_cast<bool>(t)) << str;
        std::string msg;
        auto d = Ast::Dict();
        auto v = eval(t, d, msg);
        EXPECT_FALSE(static_cast<bool>(v)) << str;
    }
}

TEST(Ast, MixedTest1)
{
    std::istringstream s("0--(-2^(3+(7*(2+2))-1)+1)*2 == -2147483646");
    auto p = Parser(s);
    auto t = p.parseExpr();
    EXPECT_TRUE(static_cast<bool>(t));
    std::string msg;
    auto d = Ast::Dict();
    auto v = eval(t, d, msg);
    EXPECT_TRUE(static_cast<bool>(v));
    EXPECT_EQ(Ast::T::BOOLEAN, v->t);
    EXPECT_TRUE(v->b);
}

TEST(Ast, MixedTest2)
{
    std::istringstream s("\"a\"+(-2^(3+(7*(2+2))-1)+1)*2");
    auto p = Parser(s);
    auto t = p.parseExpr();
    EXPECT_TRUE(static_cast<bool>(t));
    std::string msg;
    auto d = Ast::Dict();
    auto v = eval(t, d, msg);
    EXPECT_TRUE(static_cast<bool>(v));
    EXPECT_EQ(Ast::T::STRING, v->t);
    EXPECT_EQ("a-2147483646", v->str);
}

TEST(Ast, parseMixedSymbol1)
{
    std::istringstream s("a.f()+2");
    auto p = Parser(s);
    auto t = p.parseExpr();

    std::string msg;
    auto d = Ast::Dict();
    d["a.f()"] = Ast::make(1.0);
    auto v = eval(t, d, msg);
    EXPECT_TRUE(static_cast<bool>(v));
    EXPECT_EQ(Ast::T::NUMBER, v->t);
    EXPECT_EQ(3, v->num);
}

TEST(Ast, parseMixedSymbol2)
{
    std::istringstream s("a.function()+2");
    auto p = Parser(s);
    auto t = p.parseExpr();

    std::string msg;
    auto d = Ast::Dict();
    d["a.function()"] = Ast::make(2.0);
    auto v = eval(t, d, msg);
    EXPECT_TRUE(static_cast<bool>(v));
    EXPECT_EQ(Ast::T::NUMBER, v->t);
    EXPECT_EQ(4, v->num);
}

TEST(Ast, parseMixedSymbol3)
{
    std::istringstream s("a.function(1,1)+2");
    auto p = Parser(s);
    auto t = p.parseExpr();

    std::string msg;
    auto d = Ast::Dict();
    d["a.function(1,1)"] = Ast::make(2.0);
    auto v = eval(t, d, msg);
    EXPECT_TRUE(static_cast<bool>(v));
    EXPECT_EQ(Ast::T::NUMBER, v->t);
    EXPECT_EQ(4, v->num);
}
