/******************************************************************************
 * @brief   TWins - unit tests
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "twins.hpp"
#include "twins_ios_defimpl.hpp"

// -----------------------------------------------------------------------------

struct STRING_Test : public testing::Test
{
    void SetUp() override
    {
        tios.stats = {};
    }

    void TearDown() override
    {
        EXPECT_EQ(0, tios.stats.memChunks);
    }

    twins::DefaultIOs& tios = (twins::DefaultIOs&)*twins::pIOs;
};

// -----------------------------------------------------------------------------

TEST_F(STRING_Test, clear)
{
    {
        twins::String s;

        EXPECT_STREQ("", s.cstr());
        EXPECT_EQ(0, s.size());
        EXPECT_EQ(0, s.utf8Len());
        EXPECT_EQ(0, tios.stats.memChunks);

        s.clear();
        EXPECT_EQ("", s.cstr());
        EXPECT_EQ(0, s.size());
        EXPECT_EQ(0, s.utf8Len());
        EXPECT_EQ(0, tios.stats.memChunks);
    }

    EXPECT_EQ(0, tios.stats.memChunksMax);
}

TEST_F(STRING_Test, append_1)
{
    {
        twins::String s;
        s.append(nullptr);
        s.append("Cześć", 0);
        s.append("Cześć");

        EXPECT_STREQ("Cześć", s.cstr());
        EXPECT_EQ(7, s.size());
        EXPECT_EQ(5, s.utf8Len());
        EXPECT_EQ(1, tios.stats.memChunks);

        s.clear();
        EXPECT_EQ(1, tios.stats.memChunks);
        EXPECT_EQ(0, s.size());
        EXPECT_EQ(0, s.utf8Len());
    }

    EXPECT_EQ(1, tios.stats.memChunksMax);
}

TEST_F(STRING_Test, append_2)
{
    {
        twins::String s;
        s.append("12345");
        s.append("ABCDE", 6); // force buffer growth

        EXPECT_EQ(35, s.size());
        EXPECT_EQ(35, s.utf8Len());
        EXPECT_EQ(1, tios.stats.memChunks);

        s.clear();
        s.append('X', -5);
        s.append('X');
        EXPECT_EQ(1, tios.stats.memChunks);
        EXPECT_EQ(1, s.size());
        EXPECT_EQ(1, s.utf8Len());
    }

    EXPECT_EQ(2, tios.stats.memChunksMax);
}

TEST_F(STRING_Test, append_fmt__fits_in_buffer)
{
    {
        twins::String s;
        s.append("12345");
        s.appendFmt(nullptr, "Fun()", __LINE__);
        EXPECT_EQ(5, s.size());

        s.appendFmt("%s:%4u", "Fun()", 2048);
        EXPECT_EQ(15, s.size());
        EXPECT_TRUE(strstr(s.cstr(), ":2048"));
        EXPECT_EQ(1, tios.stats.memChunks);
    }

    EXPECT_EQ(1, tios.stats.memChunksMax);
}

TEST_F(STRING_Test, append_fmt__buffer_to_small)
{
    {
        twins::String s;
        s.append("12345", 6);
        EXPECT_EQ(30, s.size());

        s.appendFmt("%s:%4u", "Fun()", 2048); // buffer must be expanded
        EXPECT_TRUE(strstr(s.cstr(), ":2048"));
        EXPECT_EQ(40, s.size());
        EXPECT_EQ(1, tios.stats.memChunks);
    }

    EXPECT_EQ(2, tios.stats.memChunksMax);
}

TEST_F(STRING_Test, trim_no_ellipsis)
{
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
}

TEST_F(STRING_Test, trim_ellipsis_1)
{
    {
        twins::String s;
        s = "► Service Menu";
        s.trim(10, true);
        EXPECT_EQ(10, s.utf8Len()); // trimmed at apace - no ellipsis added
        EXPECT_STREQ("► Service ", s.cstr());
    }
}

TEST_F(STRING_Test, trim_ellipsis_2)
{
    {
        twins::String s;
        s = "► Service Menu";
        s.trim(12, true); // trim at non-space character
        EXPECT_EQ(12, s.utf8Len());
        EXPECT_STREQ("► Service M…", s.cstr());
    }
}

TEST_F(STRING_Test, move_assign)
{
    {
        twins::String s1;
        s1 = "Menu";

        twins::String s2;
        s2 = std::move(s1);

        EXPECT_EQ(0, s1.size());
        EXPECT_EQ(4, s2.size());

        twins::String s3 = std::move(s2);
        EXPECT_EQ(0, s2.size());
        EXPECT_EQ(4, s3.size());
    }
}
