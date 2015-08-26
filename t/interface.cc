#include "../src/interface.h"

#include <gtest/gtest.h>

TEST(Interface, MixedTest)
{
    for (auto str : {"a+1"}) {
        Expression e(str);
        EXPECT_TRUE(e) << str;
    }
}
