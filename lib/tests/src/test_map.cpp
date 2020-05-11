/******************************************************************************
 * @brief   TWins - unit tests
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "twins_map.hpp"

// -----------------------------------------------------------------------------

TEST(MAP, add_many)
{
    twins::Map<short, int> m;
    EXPECT_EQ(0, m.size());
    const int step = 3;

    for (int i = 1; i < 100; i += step)
        m[i] = i;

    EXPECT_EQ(100/step, m.size());

    for (int i = 1; i < 100; i += step)
        EXPECT_EQ(i, m[i]);

    // for (unsigned i = 0; i < m.bucketsCount(); i++)
    // {
    //     const auto *p = m.bucket(i);
    //     printf("bucket[%u] = %u\n", i, p->size());
    // }
}

TEST(MAP, remove_clear)
{
    twins::Map<short, int> m;
    EXPECT_EQ(0, m.size());

    for (int i = 0; i < 10; i++)
        m[i] = i;

    EXPECT_EQ(10, m.size());
    EXPECT_EQ(5, m[5]);
    EXPECT_TRUE(m.contains(5));
    EXPECT_FALSE(m.contains(111));

    m.remove(5);
    EXPECT_EQ(9, m.size());
    EXPECT_FALSE(m.contains(5));
    EXPECT_EQ(0, m[5]);
    EXPECT_TRUE(m.contains(5));

    m.clear();
    EXPECT_EQ(0, m.size());
}

TEST(MAP, key_bool)
{
    twins::Map<bool, int> m;
    EXPECT_EQ(0, m.size());

    m[false] = 1;
    m[true] = 2;
}
