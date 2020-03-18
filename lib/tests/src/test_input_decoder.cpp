/******************************************************************************
 * @brief   TWins - unit tests
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "twins.hpp"

// -----------------------------------------------------------------------------

TEST(INPUTDECODER, empty)
{
    twins::AnsiSequence as = {};
    twins::KeyCode key;

    decodeInputSeq(as, key);

    EXPECT_EQ(KEY_MOD_NONE, key.mod_all);
    EXPECT_EQ(twins::Key::None, key.key);
    EXPECT_STREQ("<?>", key.name);
}

TEST(INPUTDECODER, unknown)
{
    twins::AnsiSequence as = {};
    twins::KeyCode key;

    strcat(as.data, "\033[1234");
    as.len = strlen(as.data)-1;
    decodeInputSeq(as, key);

    EXPECT_EQ(KEY_MOD_NONE, key.mod_all);
    EXPECT_EQ(twins::Key::None, key.key);
    EXPECT_STREQ("<?>", key.name);
}

TEST(INPUTDECODER, u8_character)
{
    twins::AnsiSequence as = {};
    twins::KeyCode key;

    strcat(as.data, "Ź");
    as.len = strlen("Ź");
    decodeInputSeq(as, key);

    EXPECT_EQ(KEY_MOD_NONE, key.mod_all);
    EXPECT_STREQ("Ź", key.utf8);
    EXPECT_STRNE("", key.name);
    EXPECT_STRNE("<?>", key.name);
}

TEST(INPUTDECODER, Esc)
{
    twins::AnsiSequence as = {};
    twins::KeyCode key;

    as.data[0] = (char)twins::Ansi::ESC;
    as.len = 1;
    decodeInputSeq(as, key);

    EXPECT_EQ(KEY_MOD_SPECIAL, key.mod_all);
    EXPECT_EQ(twins::Key::Esc, key.key);
    EXPECT_STRNE("", key.name);
    EXPECT_STRNE("<?>", key.name);
}

TEST(INPUTDECODER, Ctrl_S)
{
    twins::AnsiSequence as = {};
    twins::KeyCode key;

    as.data[0] = (char)0x13;
    as.len = 1;
    decodeInputSeq(as, key);

    EXPECT_EQ(KEY_MOD_CTRL, key.mod_all);
    EXPECT_STREQ("S", key.utf8);
    EXPECT_STRNE("", key.name);
    EXPECT_STRNE("<?>", key.name);
}

TEST(INPUTDECODER, Shift_F1)
{
    twins::AnsiSequence as = {};
    twins::KeyCode key;

    strcat(as.data, "\033[1;5H");
    as.len = strlen(as.data)-1;
    decodeInputSeq(as, key);

    EXPECT_EQ(KEY_MOD_SPECIAL | KEY_MOD_CTRL, key.mod_all);
    EXPECT_EQ(twins::Key::Home, key.key);
    EXPECT_STRNE("", key.name);
    EXPECT_STRNE("<?>", key.name);
}
