/******************************************************************************
 * @brief   TWins - unit tests main
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#include "gtest/gtest.h"
#include "twins.hpp"
#include "twins_pal_defimpl.hpp"

#include <vector>
#include <string>

// -----------------------------------------------------------------------------

namespace twins
{

const char* encodeClTheme(ColorFG cl)  { return ""; }
const char* encodeClTheme(ColorBG cl)  { return ""; }
ColorFG intensifyClTheme(ColorFG cl) { return cl; }
ColorBG intensifyClTheme(ColorBG cl) { return cl; }

}

// -----------------------------------------------------------------------------

// must be global due to static twins objects destroyed after main() quit
struct TestPAL : twins::DefaultPAL
{
    TestPAL()
    {
        twins::init(this);
    }

    ~TestPAL()
    {
        // moved here to cover the code
        twins::mouseMode(twins::MouseMode::Off);
        deinit();
        twins::deinit();
    }

    void flushBuff() override
    {
        // do not write anything to terminal
        if (lineBuff.size())
        {
            if (lineBuff.size() > lineBuffMaxSize)
                lineBuffMaxSize = lineBuff.size();

            lineBuff.clear();
        }
    }
};

TestPAL test_pal;

// -----------------------------------------------------------------------------

int main(int argc, char **argv)
{
    std::vector<char*> vargs(argv, argv+argc);
    auto arg1 = std::string("--gtest_color=yes");
    vargs.push_back((char*)arg1.c_str());
    argc = vargs.size();

    testing::InitGoogleTest(&argc, vargs.data());
    int rc = RUN_ALL_TESTS();
    fprintf(stderr, "\n*** Tests finished ***\n");
    return rc;
}
