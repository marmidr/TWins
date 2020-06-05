/******************************************************************************
 * @brief   TWins - unit tests
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "twins_utils.hpp"

// -----------------------------------------------------------------------------

TEST(UTILS, splitWords)
{
    {
        auto words = twins::util::splitWords(nullptr);
        EXPECT_EQ(0, words.size());
    }

    {
        auto words = twins::util::splitWords("");
        EXPECT_EQ(0, words.size());
    }

    {
        auto words = twins::util::splitWords("  \t \n ");
        EXPECT_EQ(0, words.size());
    }

    {
        auto words = twins::util::splitWords("abc");
        EXPECT_EQ(1, words.size());
        // EXPECT_STREQ("abc", words[0].cstr());
    }

    {
        auto words = twins::util::splitWords(" abc def ");
        ASSERT_EQ(2, words.size());
        // EXPECT_STREQ("abc", words[0].cstr());
        // EXPECT_STREQ("def", words[1].cstr());
    }

    {
        auto words = twins::util::splitWords(" abc\n def   g ");
        ASSERT_EQ(3, words.size());
        // EXPECT_STREQ("abc", words[0].cstr());
        // EXPECT_STREQ("def", words[1].cstr());
        // EXPECT_STREQ("g", words[2].cstr());
    }

    {
        auto words = twins::util::splitWords(" abc def ", ",");
        ASSERT_EQ(1, words.size());
        // EXPECT_STREQ(" abc def ", words[0].cstr());
    }
}

TEST(UTILS, splitWordsCpy)
{
    {
        auto words = twins::util::splitWordsCpy(nullptr);
        EXPECT_EQ(0, words.size());
    }

    {
        auto words = twins::util::splitWordsCpy("");
        EXPECT_EQ(0, words.size());
    }

    {
        auto words = twins::util::splitWordsCpy("  \t \n ");
        EXPECT_EQ(0, words.size());
    }

    {
        auto words = twins::util::splitWordsCpy("abc");
        EXPECT_EQ(1, words.size());
        EXPECT_STREQ("abc", words[0].cstr());
    }

    {
        auto words = twins::util::splitWordsCpy(" abc def ");
        ASSERT_EQ(2, words.size());
        EXPECT_STREQ("abc", words[0].cstr());
        EXPECT_STREQ("def", words[1].cstr());
    }

    {
        auto words = twins::util::splitWordsCpy(" abc\n def   g ");
        ASSERT_EQ(3, words.size());
        EXPECT_STREQ("abc", words[0].cstr());
        EXPECT_STREQ("def", words[1].cstr());
        EXPECT_STREQ("g", words[2].cstr());
    }

    {
        auto words = twins::util::splitWordsCpy(" abc def ", ",");
        ASSERT_EQ(1, words.size());
        EXPECT_STREQ(" abc def ", words[0].cstr());
    }
}

TEST(UTILS, wordWrap)
{
    {
        auto s = twins::util::wordWrap(nullptr, 1);
        EXPECT_EQ(0, s.size());
    }

    {
        auto s = twins::util::wordWrap("a", 0);
        EXPECT_EQ(0, s.size());
    }

    {
        auto s = twins::util::wordWrap("ab cdę fghi z", 3, ".");
        EXPECT_STREQ("ab .cdę.fg….z ", s.cstr());
    }

    {
        auto s = twins::util::wordWrap("ab  cdę   fghi", 20, ".");
        EXPECT_STREQ("ab cdę fghi ", s.cstr());
    }

    {
        auto s = twins::util::wordWrap("abc de", 2, "|");
        EXPECT_STREQ("a…|de", s.cstr());
    }

    {
        auto s = twins::util::wordWrap(" ab\n   ćde \t fg ", 3, ".");
        EXPECT_STREQ("ab .ćde.fg ", s.cstr());
    }

    {
        auto s = twins::util::wordWrap("ab cd ęfgh i jk", 5, ".");
        EXPECT_STREQ("ab cd.ęfgh .i jk ", s.cstr());
    }

    {
        auto s = twins::util::wordWrap("ab  \e[1mcd\e[0m  ęfgh i jk", 5, ".");
        EXPECT_STREQ("ab \e[1mcd\e[0m.ęfgh .i jk ", s.cstr());
    }

}

TEST(UTILS, splitLines)
{
    {
        auto vec = twins::util::splitLines(nullptr);
        EXPECT_EQ(0, vec.size());
    }

    {
        auto vec = twins::util::splitLines("");
        EXPECT_EQ(0, vec.size());
    }

    {
        auto vec = twins::util::splitLines(" abc ");
        ASSERT_EQ(1, vec.size());
        twins::String s; s.appendLen(vec[0].data, vec[0].size);
        EXPECT_STREQ(" abc ", s.cstr());
    }

    {
        auto vec = twins::util::splitLines(" abc \n");
        ASSERT_EQ(1, vec.size());
        twins::String s; s.appendLen(vec[0].data, vec[0].size);
        EXPECT_STREQ(" abc ", s.cstr());
    }

    {
        auto vec = twins::util::splitLines("\n\n\n");
        EXPECT_EQ(3, vec.size());
        EXPECT_EQ(0, vec[0].size);
        EXPECT_EQ(0, vec[1].size);
        EXPECT_EQ(0, vec[2].size);
    }

    {
        auto vec = twins::util::splitLines("\n..\n\n...");
        EXPECT_EQ(4, vec.size());
        EXPECT_EQ(0, vec[0].size);
        EXPECT_EQ(2, vec[1].size);
        EXPECT_EQ(0, vec[2].size);
        EXPECT_EQ(3, vec[3].size);
    }
}
