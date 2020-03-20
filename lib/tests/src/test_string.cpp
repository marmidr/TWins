/******************************************************************************
 * @brief   TWins - unit tests
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "twins.hpp"

// -----------------------------------------------------------------------------

struct test_ios_t
{
    int chunks = 0;
    int chunks_max = 0;
} ios_stat;

static twins::IOs ios =
{
    writeStr : [](const char *s)
    {
        return printf("%s", s);
    },
    writeStrFmt : [](const char *fmt, va_list ap)
    {
        return vprintf(fmt, ap);
    },
    flush : []()
    {
        fflush(stdout);
    },
    malloc : [](uint32_t sz)
    {
        ios_stat.chunks ++;
        if (ios_stat.chunks > ios_stat.chunks_max)
            ios_stat.chunks_max = ios_stat.chunks;

        return malloc(sz);
    },
    mfree : [](void *ptr)
    {
        ios_stat.chunks --;
        free(ptr);
    }
};

// -----------------------------------------------------------------------------

TEST(STRING, clear)
{
    ios_stat = {};
    twins::init(&ios);

    {
        twins::String s;

        EXPECT_STREQ("", s.cstr());
        EXPECT_EQ(0, s.size());
        EXPECT_EQ(0, s.utf8Len());
        EXPECT_EQ(0, ios_stat.chunks);

        s.clear();
        EXPECT_EQ("", s.cstr());
        EXPECT_EQ(0, s.size());
        EXPECT_EQ(0, s.utf8Len());
        EXPECT_EQ(0, ios_stat.chunks);
    }

    EXPECT_EQ(0, ios_stat.chunks_max);
}

TEST(STRING, append_1)
{
    ios_stat = {};
    twins::init(&ios);

    {
        twins::String s;
        s.append(nullptr);
        s.append("Cześć", 0);
        s.append("Cześć");

        EXPECT_STREQ("Cześć", s.cstr());
        EXPECT_EQ(7, s.size());
        EXPECT_EQ(5, s.utf8Len());
        EXPECT_EQ(1, ios_stat.chunks);

        s.clear();
        EXPECT_EQ(1, ios_stat.chunks);
        EXPECT_EQ(0, s.size());
        EXPECT_EQ(0, s.utf8Len());
    }

    EXPECT_EQ(0, ios_stat.chunks);
    EXPECT_EQ(1, ios_stat.chunks_max);
}

TEST(STRING, append_2)
{
    ios_stat = {};
    twins::init(&ios);

    {
        twins::String s;
        s.append("12345");
        s.append("ABCDE", 6); // force buffer growth

        EXPECT_EQ(35, s.size());
        EXPECT_EQ(35, s.utf8Len());
        EXPECT_EQ(1, ios_stat.chunks);

        s.clear();
        s.append('X', -5);
        s.append('X');
        EXPECT_EQ(1, ios_stat.chunks);
        EXPECT_EQ(1, s.size());
        EXPECT_EQ(1, s.utf8Len());
    }

    EXPECT_EQ(0, ios_stat.chunks);
    EXPECT_EQ(2, ios_stat.chunks_max);
}

TEST(STRING, append_fmt__fits_in_buffer)
{
    ios_stat = {};
    twins::init(&ios);

    {
        twins::String s;
        s.append("12345");
        s.appendFmt(nullptr, "Fun()", __LINE__);
        EXPECT_EQ(5, s.size());

        s.appendFmt("%s:%4u", "Fun()", 2048);
        EXPECT_EQ(15, s.size());
        EXPECT_TRUE(strstr(s.cstr(), ":2048"));
        EXPECT_EQ(1, ios_stat.chunks);
    }

    EXPECT_EQ(0, ios_stat.chunks);
    EXPECT_EQ(1, ios_stat.chunks_max);
}

TEST(STRING, append_fmt__buffer_to_small)
{
    ios_stat = {};
    twins::init(&ios);

    {
        twins::String s;
        s.append("12345", 6);
        EXPECT_EQ(30, s.size());

        s.appendFmt("%s:%4u", "Fun()", 2048); // buffer must be expanded
        EXPECT_TRUE(strstr(s.cstr(), ":2048"));
        EXPECT_EQ(40, s.size());
        EXPECT_EQ(1, ios_stat.chunks);
    }

    EXPECT_EQ(0, ios_stat.chunks);
    EXPECT_EQ(2, ios_stat.chunks_max);
}

TEST(STRING, trim_no_ellipsis)
{
    ios_stat = {};
    twins::init(&ios);

    {
        twins::String s;
        s.append("► Service Menu");

        // beyound text
        auto sz = s.size();
        s.trim(s.size());
        EXPECT_EQ(sz, s.size());

        // inside text
        s.trim(10);
        EXPECT_EQ(10, s.utf8Len());
        EXPECT_STREQ("► Service ", s.cstr());
    }

    EXPECT_EQ(0, ios_stat.chunks);
}

TEST(STRING, trim_ellipsis_1)
{
    ios_stat = {};
    twins::init(&ios);

    {
        twins::String s;
        s = "► Service Menu";
        s.trim(10, true);
        EXPECT_EQ(10, s.utf8Len()); // trimmed at apace - no ellipsis added
        EXPECT_STREQ("► Service ", s.cstr());
    }

    EXPECT_EQ(0, ios_stat.chunks);
}

TEST(STRING, trim_ellipsis_2)
{
    ios_stat = {};
    twins::init(&ios);

    {
        twins::String s;
        s = "► Service Menu";
        s.trim(12, true); // trim at non-space character
        EXPECT_EQ(12, s.utf8Len());
        EXPECT_STREQ("► Service M…", s.cstr());
    }

    EXPECT_EQ(0, ios_stat.chunks);
}

TEST(STRING, move_assign)
{
    ios_stat = {};
    twins::init(&ios);

    {
        twins::String s1;
        s1.append("Menu");

        twins::String s2;
        s2 = std::move(s1);

        EXPECT_EQ(0, s1.size());
        EXPECT_EQ(4, s2.size());
    }

    EXPECT_EQ(0, ios_stat.chunks);
}
