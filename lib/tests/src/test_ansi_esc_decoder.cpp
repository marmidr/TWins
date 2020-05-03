/******************************************************************************
 * @brief   TWins - unit tests
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "twins.hpp"

// -----------------------------------------------------------------------------

namespace twins
{
void ansiDecodeInputSeqReset();
}

static char rbBuffer[15];


// -----------------------------------------------------------------------------

TEST(ANSI_INPUTDECODER, empty)
{
    twins::RingBuff<char> input(rbBuffer);
    twins::KeyCode kc;

    ASSERT_EQ(0, input.size());
    ASSERT_EQ(sizeof(rbBuffer), input.capacity());
    twins::ansiDecodeInputSeq(input, kc);

    EXPECT_EQ(KEY_MOD_NONE, kc.mod_all);
    EXPECT_EQ(twins::Key::None, kc.key);
    EXPECT_STREQ("<?>", kc.name);
}

TEST(ANSI_INPUTDECODER, unknown)
{
    twins::RingBuff<char> input(rbBuffer);
    twins::KeyCode kc;

    input.write("\033[1234");
    ansiDecodeInputSeq(input, kc);

    EXPECT_EQ(KEY_MOD_NONE, kc.mod_all);
    EXPECT_EQ(twins::Key::None, kc.key);
    EXPECT_STREQ("<?>", kc.name);
}

TEST(ANSI_INPUTDECODER, u8_character)
{
    twins::RingBuff<char> input(rbBuffer);
    twins::KeyCode kc;

    input.write("Ź");
    ansiDecodeInputSeq(input, kc);

    EXPECT_EQ(KEY_MOD_NONE, kc.mod_all);
    EXPECT_STREQ("Ź", kc.utf8);
    EXPECT_STRNE("", kc.name);
    EXPECT_STRNE("<?>", kc.name);
}

TEST(ANSI_INPUTDECODER, Esc)
{
    twins::RingBuff<char> input(rbBuffer);
    twins::KeyCode kc;

    input.write((char)twins::Ansi::ESC);
    ansiDecodeInputSeq(input, kc);

    EXPECT_EQ(KEY_MOD_SPECIAL, kc.mod_all);
    EXPECT_EQ(twins::Key::Esc, kc.key);
    EXPECT_STRNE("", kc.name);
    EXPECT_STRNE("<?>", kc.name);
}

TEST(ANSI_INPUTDECODER, Ctrl_S)
{
    twins::RingBuff<char> input(rbBuffer);
    twins::KeyCode kc;

    input.write((char)0x13);
    ansiDecodeInputSeq(input, kc);

    EXPECT_EQ(KEY_MOD_CTRL, kc.mod_all);
    EXPECT_STREQ("S", kc.utf8);
    EXPECT_STRNE("", kc.name);
    EXPECT_STRNE("<?>", kc.name);
}

TEST(ANSI_INPUTDECODER, Ctrl_F1)
{
    twins::RingBuff<char> input(rbBuffer);
    twins::KeyCode kc;

    input.write("\033[1;5H");
    ansiDecodeInputSeq(input, kc);

    EXPECT_EQ(KEY_MOD_SPECIAL | KEY_MOD_CTRL, kc.mod_all);
    EXPECT_EQ(twins::Key::Home, kc.key);
    EXPECT_STRNE("", kc.name);
    EXPECT_STRNE("<?>", kc.name);
}

TEST(ANSI_INPUTDECODER, UnknownSeq__Ctrl_Home)
{
    twins::ansiDecodeInputSeqReset();
    twins::RingBuff<char> input(rbBuffer);
    twins::KeyCode kc;

    input.write("\033*42~");
    input.write("\033[1;5H@");

    ansiDecodeInputSeq(input, kc);
    EXPECT_EQ(KEY_MOD_SPECIAL | KEY_MOD_CTRL, kc.mod_all);
    EXPECT_EQ(twins::Key::Home, kc.key);

    ansiDecodeInputSeq(input, kc);
    EXPECT_EQ(KEY_MOD_NONE, kc.mod_all);
    EXPECT_STREQ("@", kc.utf8);
}

TEST(ANSI_INPUTDECODER, LoongUnknownSeq__Ctrl_Home)
{
    twins::ansiDecodeInputSeqReset();
    twins::RingBuff<char> input(rbBuffer);
    twins::KeyCode kc;

    // next ESC is more that 7 bytes further,
    // so entire buffer will be cleared
    input.write("\033*123456789~");
    ansiDecodeInputSeq(input, kc);
    EXPECT_EQ(twins::Key::None, kc.key);

    input.write("\033[1;5H");
    ansiDecodeInputSeq(input, kc);
    EXPECT_EQ(twins::Key::None, kc.key);

    input.write("+");
    ansiDecodeInputSeq(input, kc); // 3rd try - abandon
    EXPECT_EQ(twins::Key::None, kc.key);
    EXPECT_EQ(0, input.size());
}

TEST(ANSI_INPUTDECODER, NUL_InInput)
{
    twins::ansiDecodeInputSeqReset();
    twins::RingBuff<char> input(rbBuffer);
    twins::KeyCode kc;

    input.write('\0');
    input.write("\t");
    ansiDecodeInputSeq(input, kc);
    EXPECT_EQ(twins::Key::None, kc.key);
}

TEST(ANSI_INPUTDECODER, Ctrl_F1__incomplete)
{
    twins::ansiDecodeInputSeqReset();
    twins::RingBuff<char> input(rbBuffer);
    twins::KeyCode kc;

    EXPECT_TRUE(input.write("\033["));
    EXPECT_EQ(2, input.size());
    ansiDecodeInputSeq(input, kc);

    EXPECT_EQ(2, input.size());
    EXPECT_EQ(KEY_MOD_NONE, kc.mod_all);
    EXPECT_EQ(twins::Key::None, kc.key);

    // write rest of previous sequence and additional one key
    EXPECT_TRUE(input.write("1;5H\033"));
    EXPECT_EQ(7, input.size());
    ansiDecodeInputSeq(input, kc);
    EXPECT_EQ(1, input.size());
    EXPECT_EQ(KEY_MOD_SPECIAL | KEY_MOD_CTRL, kc.mod_all);
    EXPECT_EQ(twins::Key::Home, kc.key);

    // decode rest of the input
    ansiDecodeInputSeq(input, kc);
    EXPECT_EQ(0, input.size());
    EXPECT_EQ(KEY_MOD_SPECIAL, kc.mod_all);
    EXPECT_EQ(twins::Key::Esc, kc.key);
}

TEST(ANSI_INPUTDECODER, L__S_C_UP__O)
{
    twins::ansiDecodeInputSeqReset();
    twins::RingBuff<char> input(rbBuffer);
    twins::KeyCode kc;

    // write rest of previous sequence and additional one key
    EXPECT_TRUE(input.write("Ł\033[1;6AÓ*"));

    ansiDecodeInputSeq(input, kc);
    EXPECT_EQ(KEY_MOD_NONE, kc.mod_all);
    EXPECT_STREQ("Ł", kc.utf8);

    ansiDecodeInputSeq(input, kc);
    EXPECT_EQ(KEY_MOD_SPECIAL | KEY_MOD_SHIFT | KEY_MOD_CTRL, kc.mod_all);
    EXPECT_EQ(twins::Key::Up, kc.key);

    ansiDecodeInputSeq(input, kc);
    EXPECT_EQ(KEY_MOD_NONE, kc.mod_all);
    EXPECT_STREQ("Ó", kc.utf8);

    // remains '*'
    EXPECT_EQ(1, input.size());
}

TEST(ANSI_INPUTDECODER, CR)
{
    twins::ansiDecodeInputSeqReset();
    twins::RingBuff<char> input(rbBuffer);
    twins::KeyCode kc;

    input.write("\r\r\t");

    ansiDecodeInputSeq(input, kc);
    EXPECT_EQ(twins::Key::Enter, kc.key);

    ansiDecodeInputSeq(input, kc);
    EXPECT_EQ(twins::Key::Enter, kc.key);

    ansiDecodeInputSeq(input, kc);
    EXPECT_EQ(twins::Key::Tab, kc.key);
}

TEST(ANSI_INPUTDECODER, LF)
{
    twins::ansiDecodeInputSeqReset();
    twins::RingBuff<char> input(rbBuffer);
    twins::KeyCode kc;

    input.write("\n\n\t");

    ansiDecodeInputSeq(input, kc);
    EXPECT_EQ(twins::Key::Enter, kc.key);

    ansiDecodeInputSeq(input, kc);
    EXPECT_EQ(twins::Key::Enter, kc.key);

    ansiDecodeInputSeq(input, kc);
    EXPECT_EQ(twins::Key::Tab, kc.key);
}

TEST(ANSI_INPUTDECODER, CR_LF_CR)
{
    twins::ansiDecodeInputSeqReset();
    twins::RingBuff<char> input(rbBuffer);
    twins::KeyCode kc;

    input.write("\n\r\n\t\n\r\t");

    ansiDecodeInputSeq(input, kc);
    EXPECT_EQ(twins::Key::Enter, kc.key);

    ansiDecodeInputSeq(input, kc);
    EXPECT_EQ(twins::Key::Enter, kc.key);

    ansiDecodeInputSeq(input, kc);
    EXPECT_EQ(twins::Key::Tab, kc.key);

    ansiDecodeInputSeq(input, kc);
    EXPECT_EQ(twins::Key::Enter, kc.key);

    ansiDecodeInputSeq(input, kc);
    EXPECT_EQ(twins::Key::Enter, kc.key);

    ansiDecodeInputSeq(input, kc);
    EXPECT_EQ(twins::Key::Tab, kc.key);
}

// -----------------------------------------------------------------------------

TEST(ANSI_ESC_LEN, SeqLen)
{
    EXPECT_EQ(0, twins::ansiEscSeqLen(nullptr));
    EXPECT_EQ(0, twins::ansiEscSeqLen(""));
    // Up
    EXPECT_EQ(0, twins::ansiEscSeqLen("x\e[A"));
    EXPECT_EQ(3, twins::ansiEscSeqLen("\e[A"));
}

TEST(ANSI_ESC_LEN, TextLen_IgnoreEsc)
{
    EXPECT_EQ(0, twins::ansiUtf8LenIgnoreEsc(nullptr));
    EXPECT_EQ(0, twins::ansiUtf8LenIgnoreEsc(""));

    EXPECT_EQ(3, twins::ansiUtf8LenIgnoreEsc("ABC"));
    EXPECT_EQ(3, twins::ansiUtf8LenIgnoreEsc("ĄBĆ"));

    EXPECT_EQ(3, twins::ansiUtf8LenIgnoreEsc("ĄBĆ\e[A"));
    EXPECT_EQ(3, twins::ansiUtf8LenIgnoreEsc("\e[AĄBĆ"));
    EXPECT_EQ(4, twins::ansiUtf8LenIgnoreEsc("Ą\e[ABĆ\e[1;2AĘ"));
}

TEST(ANSI_ESC_LEN, Skip_IgnoreEsc)
{
    EXPECT_STREQ("", twins::ansiUtf8SkipIgnoreEsc(nullptr, 0));
    EXPECT_STREQ("", twins::ansiUtf8SkipIgnoreEsc("", 5));

    EXPECT_STREQ("ABC", twins::ansiUtf8SkipIgnoreEsc("ABC", 0));
    EXPECT_STREQ("C", twins::ansiUtf8SkipIgnoreEsc("ABC", 2));
    EXPECT_STREQ("", twins::ansiUtf8SkipIgnoreEsc("ABC", 5));
    EXPECT_STREQ("Ć", twins::ansiUtf8SkipIgnoreEsc("ĄBĆ", 2));

    EXPECT_STREQ("Ć\e[1;2AĘ", twins::ansiUtf8SkipIgnoreEsc("Ą\e[ABĆ\e[1;2AĘ", 2));
    EXPECT_STREQ("", twins::ansiUtf8SkipIgnoreEsc("Ą\e[ABĆ\e[1;2AĘ", 4));
}
