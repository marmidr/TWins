/******************************************************************************
 * @brief   TWins - unit tests
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "twins_map.hpp"
#include <string>

// -----------------------------------------------------------------------------

const char *itostr(int i)
{
    static char buff[10];
    snprintf(buff, sizeof(buff), "%d", i);
    return buff;
}

TEST(MAP, add_many)
{
    twins::Map<short, long long> m;
    EXPECT_EQ(0, m.size());
    const int step = 3;
    const int nodes = 300;

    for (int i = 1; i < nodes; i += step)
        m[i] = i;

    EXPECT_EQ(nodes/step, m.size());
    EXPECT_EQ(16, m.bucketsCount());

    for (int i = 1; i < nodes; i += step)
        EXPECT_EQ(i, m[i]);

    fprintf(stderr, "distro: %d\n", m.distribution());
    EXPECT_GE(m.distribution(), 90);

    // for (unsigned i = 0; i < m.bucketsCount(); i++)
    // {
    //     const auto *p = m.bucket(i);
    //     printf("bucket[%u] = %u\n", i, p->size());
    // }
}

TEST(MAP, add_many_strings)
{
    twins::Map<short, std::string> m;
    EXPECT_EQ(0, m.size());
    const int nodes = 100;

    for (int i = 0; i < nodes; i ++)
        m[i] = itostr(i);

    EXPECT_EQ(nodes, m.size());
    EXPECT_EQ(16, m.bucketsCount());
    EXPECT_STREQ("42", m[42].c_str());
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

TEST(MAP, key_arithmetic)
{
    {
        twins::Map<bool, int> m;
        EXPECT_EQ(0, m.size());

        m[false] = 1;
        m[true] = 2;
        EXPECT_EQ(2, m[true]);
    }

    {
        twins::Map<double, bool> m;

        EXPECT_FALSE(m[3.14]);
        m[3.14] = true;
        EXPECT_TRUE(m[3.14]);
    }
}

TEST(MAP, key_enum)
{
    {
        enum Valves {VLV_None, VLV_Forward, VLV_Backward} ;
        twins::Map<Valves, bool> m;

        EXPECT_FALSE(m[VLV_Forward]);
        m[VLV_Forward] = true;
        EXPECT_TRUE(m[VLV_Forward]);
    }

    {
        enum class Valves {None, Forward, Backward} ;
        twins::Map<Valves, int> m;
        m[Valves::Forward] = true;
    }
}

TEST(MAP, key_cstr)
{
    {
        twins::Map<const char*, bool> m;

        EXPECT_FALSE(m["Göbekli Tepe"]);
        m["Göbekli Tepe"] = true;
        EXPECT_TRUE(m["Göbekli Tepe"]);
    }

    {
        twins::Map<char*, bool> m;
        char key[10]; strcpy(key, "*key");

        EXPECT_FALSE(m[key]);
        m[key] = true;
        EXPECT_TRUE(m[key]);
    }
}
