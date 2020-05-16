/******************************************************************************
 * @brief   TWins - unit tests
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "twins.hpp"
#include "twins_pal_defimpl.hpp"

// -----------------------------------------------------------------------------

struct STRING_Test : public testing::Test
{
    void SetUp() override
    {
        pal.stats = {};
    }

    void TearDown() override
    {
        EXPECT_EQ(0, pal.stats.memChunks);
    }

    twins::DefaultPAL& pal = (twins::DefaultPAL&)*twins::pPAL;
};

// -----------------------------------------------------------------------------

TEST_F(STRING_Test, clear)
{
    {
        twins::String s;

        EXPECT_STREQ("", s.cstr());
        EXPECT_EQ(0, s.size());
        EXPECT_EQ(0, s.u8len());
        EXPECT_EQ(0, pal.stats.memChunks);

        s.clear();
        EXPECT_EQ("", s.cstr());
        EXPECT_EQ(0, s.size());
        EXPECT_EQ(0, s.u8len());
        EXPECT_EQ(0, pal.stats.memChunks);
    }

    EXPECT_EQ(0, pal.stats.memChunksMax);
}

TEST_F(STRING_Test, append_no_resize)
{
    {
        twins::String s;
        EXPECT_STREQ("", s.cstr());

        s.append(nullptr);
        s.append("Cześć", 0); // append 0 copies
        s.append("Cześć");
        s.append(s.cstr()); // append ourselve shall fail

        EXPECT_STREQ("Cześć", s.cstr());
        EXPECT_EQ(7, s.size());
        EXPECT_EQ(5, s.u8len());
        EXPECT_EQ(1, pal.stats.memChunks);

        s.clear();
        EXPECT_EQ(1, pal.stats.memChunks);
        EXPECT_EQ(0, s.size());
        EXPECT_EQ(0, s.u8len());
    }

    EXPECT_EQ(1, pal.stats.memChunksMax);
}

TEST_F(STRING_Test, append_resize_buffer)
{
    {
        twins::String s;
        s.append("12345");
        s.append("ABCDE", 6); // force buffer growth

        EXPECT_EQ(35, s.size());
        EXPECT_EQ(35, s.u8len());
        EXPECT_EQ(1, pal.stats.memChunks);

        s.clear();
        s.append('X', -5);
        s.append('X');
        EXPECT_EQ(1, pal.stats.memChunks);
        EXPECT_EQ(1, s.size());
        EXPECT_EQ(1, s.u8len());
    }

    EXPECT_EQ(2, pal.stats.memChunksMax);
}

TEST_F(STRING_Test, append_very_long)
{
    twins::String s;
    s.append("12345ABCDE", 101);

    EXPECT_EQ(1010, s.size());
    EXPECT_EQ(1010, s.u8len());

    s.clear();
    s.append('X');
    EXPECT_EQ(1, s.size());
}

TEST_F(STRING_Test, append_esc)
{
    twins::String s;
    s.append(ESC_BLINK "x" ESC_BLINK_OFF);

    EXPECT_EQ(10, s.size());
    EXPECT_EQ(10, s.u8len(false));
    EXPECT_EQ(1, s.u8len(true));
}

TEST_F(STRING_Test, append_len)
{
    twins::String s;
    s.appendLen(nullptr, 3);
    EXPECT_EQ(0, s.size());
    s.appendLen("ABCDE", -15);
    EXPECT_EQ(0, s.size());

    s.appendLen("ABCDE", 3);
    s.appendLen("123456789", 5);

    EXPECT_EQ(8, s.size());
    EXPECT_EQ(8, s.u8len());
    EXPECT_STREQ("ABC12345", s.cstr());
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
        EXPECT_EQ(1, pal.stats.memChunks);
    }

    EXPECT_EQ(1, pal.stats.memChunksMax);
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
        EXPECT_EQ(1, pal.stats.memChunks);
    }

    EXPECT_EQ(2, pal.stats.memChunksMax);
}

TEST_F(STRING_Test, stream_append)
{
    twins::String s;
    s.append("x");
    EXPECT_STREQ("x", s.cstr());

    s << "► " << "Service Menu";
    EXPECT_STREQ("x► Service Menu", s.cstr());
}

TEST_F(STRING_Test, trim_no_ellipsis)
{
    twins::String s;
    s.append("► Service Menu");

    // beyound text
    auto sz = s.size();
    s.trim(s.size());
    EXPECT_EQ(sz, s.size());

    // inside text
    s.trim(10);
    EXPECT_EQ(10, s.u8len());
    EXPECT_STREQ("► Service ", s.cstr());
}

TEST_F(STRING_Test, trim_ellipsis_1)
{
    twins::String s;
    s = "► Service Menu";
    s.trim(10, true);
    EXPECT_EQ(10, s.u8len()); // trimmed at apace - no ellipsis added
    EXPECT_STREQ("► Service ", s.cstr());
}

TEST_F(STRING_Test, trim_ellipsis_2)
{
    twins::String s;
    s = "► Service Menu";
    s.trim(12, true); // trim at non-space character
    EXPECT_EQ(12, s.u8len());
    EXPECT_STREQ("► Service M…", s.cstr());
}

TEST_F(STRING_Test, trim_ignore_esc)
{
    twins::String s;
    s.append("►" ESC_BOLD " Service" ESC_NORMAL " Menu");

    s.trim(10, false, true);
    EXPECT_STREQ("►" ESC_BOLD " Service" ESC_NORMAL " ", s.cstr());
}

TEST_F(STRING_Test, set_len)
{
    {
        twins::String s;
        s = "1.";
        s.setLength(10);
        EXPECT_STREQ("1.        ", s.cstr());
        s.setLength(3);
        EXPECT_STREQ("1. ", s.cstr());
    }

    {
        twins::String s;
        s = "12345";

        s.setLength(6);
        EXPECT_STREQ("12345 ", s.cstr());

        s.setLength(5, false);
        EXPECT_STREQ("12345", s.cstr());

        s.setLength(5, true);
        EXPECT_STREQ("12345", s.cstr());

        s.setLength(2, true);
        EXPECT_STREQ("1…", s.cstr());

        s.setLength(3, true);
        EXPECT_STREQ("1… ", s.cstr());
    }
}

TEST_F(STRING_Test, set_len_ignore_esc)
{
    twins::String s;
    s.append("►" ESC_BOLD " Service" ESC_NORMAL " Menu");

    s.setLength(20, false, true);
    EXPECT_STREQ("►" ESC_BOLD " Service" ESC_NORMAL " Menu" "      ", s.cstr());

    s.setLength(10, false, true);
    EXPECT_STREQ("►" ESC_BOLD " Service" ESC_NORMAL " ", s.cstr());
}

TEST_F(STRING_Test, copy_assign)
{
    twins::String s;
    s = "Menu";

    // such try shall fail
    s = s;
    EXPECT_STREQ("Menu", s.cstr());

    s = s.cstr();
    EXPECT_STREQ("Menu", s.cstr());

    s = s.cstr() + 3;
    EXPECT_STREQ("Menu", s.cstr());
}

TEST_F(STRING_Test, move_assign)
{
    twins::String s1;
    s1 = "Menu";

    // such try shall fail
    s1 = std::move(s1);
    EXPECT_STREQ("Menu", s1.cstr());

    twins::String s2;
    s2 = std::move(s1);

    EXPECT_EQ(0, s1.size());
    EXPECT_EQ(4, s2.size());

    twins::String s3 = std::move(s2);
    EXPECT_EQ(0, s2.size());
    EXPECT_EQ(4, s3.size());
}

TEST_F(STRING_Test, erase)
{
    {
        twins::String s;

        s = "A";
        s.erase(-2, 1);
        EXPECT_STREQ("A", s.cstr());
        s.erase(0, -1);
        EXPECT_STREQ("A", s.cstr());
        s.erase(1, 1);
        EXPECT_STREQ("A", s.cstr());
    }

    {
        twins::String s;

        s = "*ĄBĆDĘ#";
        s.erase(1, 1);
        EXPECT_STREQ("*BĆDĘ#", s.cstr());
        s.erase(3, 2);
        EXPECT_STREQ("*BĆ#", s.cstr());
        s.erase(1, 15);
        EXPECT_STREQ("*", s.cstr());
    }
}

TEST_F(STRING_Test, insert)
{
    {
        twins::String s;

        s = "A";
        s.insert(-2, "*");
        EXPECT_STREQ("A", s.cstr());
        s.insert(0, nullptr);
        EXPECT_STREQ("A", s.cstr());
        s.insert(0, "");
        EXPECT_STREQ("A", s.cstr());
        s.insert(5, ".");
        EXPECT_STREQ("A.", s.cstr());
    }

    {
        twins::String s;

        s = "*ĄBĆDĘ#";
        s.insert(1, ".");
        EXPECT_STREQ("*.ĄBĆDĘ#", s.cstr());
        s.insert(5, "••");
        EXPECT_STREQ("*.ĄBĆ••DĘ#", s.cstr());
        s.insert(11, "X");
        EXPECT_STREQ("*.ĄBĆ••DĘ#X", s.cstr());
    }

    {
        twins::String s;

        s.insert(0, "••");
        EXPECT_STREQ("••", s.cstr());
    }
}

TEST(STRING, escLen)
{
    EXPECT_EQ(0, twins::String::escLen(nullptr));
    EXPECT_EQ(0, twins::String::escLen(""));
    // Up
    EXPECT_EQ(0, twins::String::escLen("x\e[A"));
    EXPECT_EQ(3, twins::String::escLen("\e[A"));
    // Home
    EXPECT_EQ(4, twins::String::escLen("\e[1~"));
    // F1
    EXPECT_EQ(5, twins::String::escLen("\e[23^"));
    // F1
    EXPECT_EQ(3, twins::String::escLen("\eOP"));
    // C-S-F1
    EXPECT_EQ(5, twins::String::escLen("\e[23@"));
}

TEST(STRING, u8lenIgnoreEsc)
{
    EXPECT_EQ(0, twins::String::u8lenIgnoreEsc(nullptr));
    EXPECT_EQ(0, twins::String::u8lenIgnoreEsc(""));

    EXPECT_EQ(3, twins::String::u8lenIgnoreEsc("ABC"));
    EXPECT_EQ(3, twins::String::u8lenIgnoreEsc("ĄBĆ"));

    EXPECT_EQ(3, twins::String::u8lenIgnoreEsc("ĄBĆ\e[A"));
    EXPECT_EQ(3, twins::String::u8lenIgnoreEsc("\e[AĄBĆ"));
    EXPECT_EQ(4, twins::String::u8lenIgnoreEsc("Ą\e[ABĆ\e[1;2AĘ"));
}

TEST(STRING, u8skipIgnoreEsc)
{
    EXPECT_STREQ("", twins::String::u8skipIgnoreEsc(nullptr, 0));
    EXPECT_STREQ("", twins::String::u8skipIgnoreEsc("", 5));

    EXPECT_STREQ("ABC", twins::String::u8skipIgnoreEsc("ABC", 0));
    EXPECT_STREQ("C", twins::String::u8skipIgnoreEsc("ABC", 2));
    EXPECT_STREQ("", twins::String::u8skipIgnoreEsc("ABC", 5));
    EXPECT_STREQ("Ć", twins::String::u8skipIgnoreEsc("ĄBĆ", 2));

    EXPECT_STREQ("Ć\e[1;2AĘ", twins::String::u8skipIgnoreEsc("Ą\e[ABĆ\e[1;2AĘ", 2));
    EXPECT_STREQ("", twins::String::u8skipIgnoreEsc("Ą\e[ABĆ\e[1;2AĘ", 4));
}
