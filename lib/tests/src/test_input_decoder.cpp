/******************************************************************************
 * @brief   TWins - unit tests
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "twins.hpp"

// -----------------------------------------------------------------------------

static char rbBuffer[15];

TEST(INPUTDECODER, empty)
{
    twins::RingBuff<char> input(rbBuffer);
    twins::KeyCode kc;

    ASSERT_EQ(0, input.size());
    ASSERT_EQ(sizeof(rbBuffer), input.capacity());
    decodeInputSeq(input, kc);

    EXPECT_EQ(KEY_MOD_NONE, kc.mod_all);
    EXPECT_EQ(twins::Key::None, kc.key);
    EXPECT_STREQ("<?>", kc.name);
}

TEST(INPUTDECODER, unknown)
{
    twins::RingBuff<char> input(rbBuffer);
    twins::KeyCode kc;

    input.write("\033[1234");
    decodeInputSeq(input, kc);

    EXPECT_EQ(KEY_MOD_NONE, kc.mod_all);
    EXPECT_EQ(twins::Key::None, kc.key);
    EXPECT_STREQ("<?>", kc.name);
}

TEST(INPUTDECODER, u8_character)
{
    twins::RingBuff<char> input(rbBuffer);
    twins::KeyCode kc;

    input.write("Ź");
    decodeInputSeq(input, kc);

    EXPECT_EQ(KEY_MOD_NONE, kc.mod_all);
    EXPECT_STREQ("Ź", kc.utf8);
    EXPECT_STRNE("", kc.name);
    EXPECT_STRNE("<?>", kc.name);
}

TEST(INPUTDECODER, Esc)
{
    twins::RingBuff<char> input(rbBuffer);
    twins::KeyCode kc;

    input.write((char)twins::Ansi::ESC);
    decodeInputSeq(input, kc);

    EXPECT_EQ(KEY_MOD_SPECIAL, kc.mod_all);
    EXPECT_EQ(twins::Key::Esc, kc.key);
    EXPECT_STRNE("", kc.name);
    EXPECT_STRNE("<?>", kc.name);
}

TEST(INPUTDECODER, Ctrl_S)
{
    twins::RingBuff<char> input(rbBuffer);
    twins::KeyCode kc;

    input.write((char)0x13);
    decodeInputSeq(input, kc);

    EXPECT_EQ(KEY_MOD_CTRL, kc.mod_all);
    EXPECT_STREQ("S", kc.utf8);
    EXPECT_STRNE("", kc.name);
    EXPECT_STRNE("<?>", kc.name);
}

TEST(INPUTDECODER, Ctrl_F1)
{
    twins::RingBuff<char> input(rbBuffer);
    twins::KeyCode kc;

    input.write("\033[1;5H");
    decodeInputSeq(input, kc);

    EXPECT_EQ(KEY_MOD_SPECIAL | KEY_MOD_CTRL, kc.mod_all);
    EXPECT_EQ(twins::Key::Home, kc.key);
    EXPECT_STRNE("", kc.name);
    EXPECT_STRNE("<?>", kc.name);
}

TEST(INPUTDECODER, Ctrl_F1_incomplete)
{
    twins::RingBuff<char> input(rbBuffer);
    twins::KeyCode kc;

    EXPECT_TRUE(input.write("\033["));
    EXPECT_EQ(2, input.size());
    decodeInputSeq(input, kc);

    EXPECT_EQ(2, input.size());
    EXPECT_EQ(KEY_MOD_NONE, kc.mod_all);
    EXPECT_EQ(twins::Key::None, kc.key);

    // write rest of previous sequence and additional one key
    EXPECT_TRUE(input.write("1;5H\033"));
    EXPECT_EQ(7, input.size());
    decodeInputSeq(input, kc);
    EXPECT_EQ(1, input.size());
    EXPECT_EQ(KEY_MOD_SPECIAL | KEY_MOD_CTRL, kc.mod_all);
    EXPECT_EQ(twins::Key::Home, kc.key);

    // decode rest of the input
    decodeInputSeq(input, kc);
    EXPECT_EQ(0, input.size());
    EXPECT_EQ(KEY_MOD_SPECIAL, kc.mod_all);
    EXPECT_EQ(twins::Key::Esc, kc.key);
}

TEST(INPUTDECODER, L__S_C_UP__O)
{
    twins::RingBuff<char> input(rbBuffer);
    twins::KeyCode kc;

    // write rest of previous sequence and additional one key
    EXPECT_TRUE(input.write("Ł\033[1;6AÓ*"));

    decodeInputSeq(input, kc);
    EXPECT_EQ(KEY_MOD_NONE, kc.mod_all);
    EXPECT_STREQ("Ł", kc.utf8);

    decodeInputSeq(input, kc);
    EXPECT_EQ(KEY_MOD_SPECIAL | KEY_MOD_SHIFT | KEY_MOD_CTRL, kc.mod_all);
    EXPECT_EQ(twins::Key::Up, kc.key);

    decodeInputSeq(input, kc);
    EXPECT_EQ(KEY_MOD_NONE, kc.mod_all);
    EXPECT_STREQ("Ó", kc.utf8);

    // remains '*'
    EXPECT_EQ(1, input.size());
}
