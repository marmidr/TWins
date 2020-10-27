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

// static const char *getLineBuff()
// {
//     if (auto *pal = dynamic_cast<twins::DefaultPAL*>(twins::pPAL))
//     {
//         return pal->lineBuff.cstr();
//     }

//     return "";
// }

// -----------------------------------------------------------------------------

class TWINS : public testing::Test
{
protected:
    void SetUp() override
    {

    }

    void TearDown() override
    {
        twins::flushBuffer();
    }
};

// -----------------------------------------------------------------------------

TEST_F(TWINS, encodeCl)
{
    {
        const char *cl = twins::encodeCl(twins::ColorFG::White);
        EXPECT_STRNE(cl, "");

        cl = twins::encodeCl(twins::ColorFG::Inherit);
        EXPECT_STREQ(cl, "");
    }

    {
        const char *cl = twins::encodeCl(twins::ColorBG::White);
        EXPECT_STRNE(cl, "");

        cl = twins::encodeCl(twins::ColorBG::Inherit);
        EXPECT_STREQ(cl, "");
    }
}

TEST_F(TWINS, intensifyCl)
{
    {
        auto cl = twins::intensifyCl(twins::ColorFG::White);
        EXPECT_EQ(twins::ColorFG::WhiteIntense, cl);

        cl = twins::intensifyCl(twins::ColorFG::Inherit);
        EXPECT_EQ(twins::ColorFG::Inherit, cl);
    }

    {
        auto cl = twins::intensifyCl(twins::ColorBG::White);
        EXPECT_EQ(twins::ColorBG::WhiteIntense, cl);

        cl = twins::intensifyCl(twins::ColorBG::Inherit);
        EXPECT_EQ(twins::ColorBG::Inherit, cl);
    }

    {
 	    auto cl = twins::ColorBG::White;
        twins::intensifyClIf(false, cl);
        EXPECT_EQ(twins::ColorBG::White, cl);
        twins::intensifyClIf(true, cl);
        EXPECT_EQ(twins::ColorBG::WhiteIntense, cl);
    }
}

TEST_F(TWINS, log)
{
    twins::Locker lck;
    EXPECT_TRUE(lck.isLocked());

    // PAL present
    TWINS_LOG_I("123");
    twins::flushBuffer();
    twins::log(nullptr, __FILE__, __LINE__, nullptr, nullptr);

    // no PAL
    auto *pal_bkp = twins::pPAL;
    twins::pPAL = nullptr;
    TWINS_LOG_W("123");
    twins::log(nullptr, __FILE__, __LINE__, nullptr, nullptr);

    // restore
    twins::pPAL = pal_bkp;
}

TEST_F(TWINS, attr)
{
    twins::pushAttr(twins::FontAttrib::None);
    twins::pushAttr(twins::FontAttrib::Bold);
    twins::pushAttr(twins::FontAttrib::Faint);
    twins::pushAttr(twins::FontAttrib::Italics);
    twins::pushAttr(twins::FontAttrib::Underline);
    twins::pushAttr(twins::FontAttrib::Blink);
    twins::pushAttr(twins::FontAttrib::Inverse);
    twins::pushAttr(twins::FontAttrib::Invisible);
    twins::pushAttr(twins::FontAttrib::StrikeThrough);
    twins::pushAttr(twins::FontAttrib::StrikeThrough);
    twins::popAttr(6);

    twins::resetAttr();
    twins::resetClFg();
    twins::resetClBg();
}

TEST_F(TWINS, logRaw)
{
    twins::logRawBegin("START", true);
    twins::logRawWrite("raw string");
    twins::logRawEnd("END");
}

TEST_F(TWINS, sleep)
{
    twins::sleepMs(10);
}
