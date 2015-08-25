#include "../src/ast.h"

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
