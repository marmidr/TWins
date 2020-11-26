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
    static bool ver_called;
    static char move_dir;

    ver_called = false;
    move_dir = 0;

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
            }
        },
        {
            "move",
            "<up/dn/home>" "\r\n"
            "    Perform a move",
            TWINS_CLI_HANDLER
            {
                if (argc == 2)
                    move_dir = *argv[1];
            }
        },
        { /* terminator */ }
    };

    twins::cli::write("ver" "\r\n");
    twins::cli::checkAndExec(commands);
    twins::cli::write("move up" "\r\n");
    twins::cli::checkAndExec(commands);
    EXPECT_TRUE(ver_called);
    EXPECT_EQ('u', move_dir);
}
