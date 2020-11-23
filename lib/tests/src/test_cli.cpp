/******************************************************************************
 * @brief   TWins - unit tests
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "twins_cli.hpp"
#include <string>

// -----------------------------------------------------------------------------

class CLI : public testing::Test
{
protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

// -----------------------------------------------------------------------------

TEST_F(CLI, commands)
{
    static bool ver_called, stop_called;
    ver_called = stop_called = false;

    twins::cli::Cmd commands[] =
    {
        {
            "", "", {}
        },
        {
            "ver",
            "    Show SW version",
            TWINS_CLI_HANDLER
            {
                ver_called = true;
            }
        },
        {
            "stop S",
            "    Stop everything",
            TWINS_CLI_HANDLER
            {
                stop_called = true;
            }
        },
        {
            "move",
            "<up/dn/home>" "\r\n"
            "    Perform a move",
            TWINS_CLI_HANDLER
            {
            }
        },
        { /* terminator */ }
    };

    // twins::cli::write("ver" "\r\n"); // TODO: process entire string, not just first code
    twins::cli::write("v");
    twins::cli::write("e");
    twins::cli::write("r");
    twins::cli::write("\r\n");
    twins::cli::checkAndExec(commands);
    EXPECT_TRUE(ver_called);
}
