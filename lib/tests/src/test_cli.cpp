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
    static const twins::cli::Cmd *p_commands = nullptr; // to solve lack of lambda captures

    ver_called = false;
    move_dir = 0;

    const twins::cli::Cmd commands[] =
    {
        {
            "",
            "",
            {}
        },
        #if TWINS_LAMBDA_CMD
        {
            "ver|V",
            "    Show SW version; alias 'V'",
            [&vcalled = ver_called](uint8_t argc, const char **argv)
            {
                // with full lambda, captures are possible but costs more
                vcalled = true;
            }
        },
        #else
        {
            "ver|V",
            "    Show SW version; alis 'V'",
            TWINS_CLI_HANDLER
            {
                ver_called = true;
            }
        },
        #endif
        {
            "call_ver",
            "    call 'V'",
            TWINS_CLI_HANDLER
            {
                assert(p_commands);
                twins::cli::exec("V", p_commands);
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

    // test for alias
    ver_called = false;
    twins::cli::write("V" "\r\n");
    twins::cli::checkAndExec(commands);
    EXPECT_TRUE(ver_called);

    // test for call
    p_commands = commands;
    ver_called = false;
    twins::cli::write("call_ver" "\r\n");
    twins::cli::checkAndExec(commands);
    EXPECT_TRUE(ver_called);
}
