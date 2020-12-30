/******************************************************************************
 * @brief   TWins - unit tests
 * @author  Mariusz Midor
 *          https://bitbucket.org/marmidr/twins
 *****************************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "twins_queue.hpp"

// -----------------------------------------------------------------------------

TEST(QUEUE, push_pop_2int)
{
    twins::Queue<int> qi;

    EXPECT_EQ(0, qi.size());
    EXPECT_EQ(nullptr, qi.pop());

    qi.push(1);
    qi.push(2);
    EXPECT_EQ(2, qi.size());

    EXPECT_EQ(1, *qi.pop());
    EXPECT_EQ(1, qi.size());

    EXPECT_EQ(2, *qi.pop());
    EXPECT_EQ(0, qi.size());

    EXPECT_EQ(nullptr, qi.pop());
}

TEST(QUEUE, push_pop_100int)
{
    twins::Queue<int> qi;

    for (int i = 0; i < 100; i++)
        qi.push(i);

    EXPECT_EQ(100, qi.size());
    EXPECT_EQ(0, *qi.pop());

    qi.clear();
    EXPECT_EQ(nullptr, qi.pop());
    EXPECT_EQ(0, qi.size());
}

TEST(QUEUE, push_pop_string)
{
    twins::Queue<std::string> qstr;
    // test for proper content destruction to catch memory leak
    std::string s;
    s.resize(100);

    EXPECT_FALSE(s.empty());
    EXPECT_EQ(0, qstr.size());

    // push copy
    qstr.push(s);
    EXPECT_FALSE(s.empty());
    EXPECT_EQ(1, qstr.size());

    // move-push
    qstr.push(std::move(s));
    EXPECT_EQ(2, qstr.size());
    EXPECT_TRUE(s.empty());
}
