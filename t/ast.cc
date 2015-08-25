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
}
