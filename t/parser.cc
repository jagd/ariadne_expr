#include "../src/ast.h"
#include "../src/parser.h"

#include <gtest/gtest.h>
#include <memory>
#include <sstream>

TEST(Parser, Ctor) {
    std::istringstream s("");
    EXPECT_NO_THROW(auto p = Parser(s));
}