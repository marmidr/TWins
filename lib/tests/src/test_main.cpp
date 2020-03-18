/******************************************************************************
 * @brief   TWins - unit tests main
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#include "gtest/gtest.h"

#include <vector>
#include <string>

// -----------------------------------------------------------------------------

int main(int argc, char **argv)
{
    std::vector<char*> vargs(argv, argv+argc);
    auto arg1 = std::string("--gtest_color=yes");
    vargs.push_back((char*)arg1.c_str());
    argc = vargs.size();

    testing::InitGoogleTest(&argc, vargs.data());
    return RUN_ALL_TESTS();
}
