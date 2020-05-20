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
ColorFG intenseClTheme(ColorFG cl) { return cl; }
ColorBG intenseClTheme(ColorBG cl) { return cl; }

}

// -----------------------------------------------------------------------------

// must be global due to static twins objects destroyed after main() quit
twins::DefaultPAL twins_pal;

int main(int argc, char **argv)
{
    std::vector<char*> vargs(argv, argv+argc);
    auto arg1 = std::string("--gtest_color=yes");
    vargs.push_back((char*)arg1.c_str());
    argc = vargs.size();

    testing::InitGoogleTest(&argc, vargs.data());
    twins::init(&twins_pal);

    return RUN_ALL_TESTS();
}
