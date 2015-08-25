#include "../src/ast.h"
#include "../src/parser.h"

#include <gtest/gtest.h>
#include <memory>
#include <sstream>

TEST(Parser, Ctor) {
    std::istringstream s("");
    EXPECT_NO_THROW(auto p = Parser(s));
}

TEST(Parser, Token1) {
    std::istringstream s("A.f(x,y) 3.14e-2 + (\"wu\" - 4) /r \t ");
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
    EXPECT_EQ(Parser::TK::END, p.token());
}
