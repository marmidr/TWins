/******************************************************************************
 * @brief   TWins - unit tests
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "twins.hpp"
#include "twins_utils.hpp"

// -----------------------------------------------------------------------------

TEST(TWINS_UTILS, splitWords)
{
    {
        auto words = twins::splitWords(nullptr);
        EXPECT_EQ(0, words.size());
    }

    {
        auto words = twins::splitWords("");
        EXPECT_EQ(0, words.size());
    }

    {
        auto words = twins::splitWords("  \t \n ");
        EXPECT_EQ(0, words.size());
    }

    {
        auto words = twins::splitWords("abc");
        EXPECT_EQ(1, words.size());
        EXPECT_STREQ("abc", words[0].cstr());
    }

    {
        auto words = twins::splitWords(" abc def ");
        ASSERT_EQ(2, words.size());
        EXPECT_STREQ("abc", words[0].cstr());
        EXPECT_STREQ("def", words[1].cstr());
    }

    {
        auto words = twins::splitWords(" abc\n def   g ");
        ASSERT_EQ(3, words.size());
        EXPECT_STREQ("abc", words[0].cstr());
        EXPECT_STREQ("def", words[1].cstr());
        EXPECT_STREQ("g", words[2].cstr());
    }

    {
        auto words = twins::splitWords(" abc def ", ",");
        ASSERT_EQ(1, words.size());
        EXPECT_STREQ(" abc def ", words[0].cstr());
    }
}

TEST(TWINS_UTILS, wordWrap)
{
    {
        auto s = twins::wordWrap(nullptr, 1);
        EXPECT_EQ(0, s.size());
    }

    {
        auto s = twins::wordWrap("a", 0);
        EXPECT_EQ(0, s.size());
    }

    {
        auto s = twins::wordWrap("ab cdę fghi z", 3, ".");
        EXPECT_STREQ("ab .cdę.fg….z ", s.cstr());
    }

    {
        auto s = twins::wordWrap("ab  cdę   fghi", 20, ".");
        EXPECT_STREQ("ab cdę fghi ", s.cstr());
    }

    {
        // auto s = twins::wordWrap("abc de", 2, "|");
        // EXPECT_STREQ("a…|d…", s.cstr());
    }

    {
        auto s = twins::wordWrap(" ab\n   ćde \t fg ", 3, ".");
        EXPECT_STREQ("ab .ćde.fg ", s.cstr());
    }

    {
        auto s = twins::wordWrap("ab cd ęfgh i jk", 5, ".");
        EXPECT_STREQ("ab cd.ęfgh .i jk ", s.cstr());
    }

    {
        auto s = twins::wordWrap("ab  \e[1mcd\e[0m  ęfgh i jk", 5, ".");
        EXPECT_STREQ("ab \e[1mcd\e[0m.ęfgh .i jk ", s.cstr());
    }

}
