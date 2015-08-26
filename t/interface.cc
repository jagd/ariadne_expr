#include "../src/interface.h"

#include <gtest/gtest.h>

TEST(Interface, MixedTest)
{
    for (auto str : {"a+1", "a.f(x,y)^3 <= 1^2", "(name > a || (x != 3))"}) {
        Expression e(str);
        EXPECT_TRUE(e) << str;
    }
}
