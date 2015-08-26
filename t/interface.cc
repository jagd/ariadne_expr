#include "../src/interface.h"

#include <gtest/gtest.h>

TEST(Interface, MixedTest)
{
    for (auto str : {
        "1+2+3", "a1+1", "a.f(x,y)^3 <= 1^2", "(name > \"a\"+3 ||" " (x != 3))"
    }) {
        Expression e(str);
        EXPECT_TRUE(e) << str;
    }
}

TEST(Interface, MixedFailedTest)
{
    for (auto str : {
        "a 1", "b!2", "1**2", "1+2)", "1^^2", "f(", "(1+2", "\"" ""}
        ) {
        Expression e(str);
        EXPECT_FALSE(e) << str;
    }
}
