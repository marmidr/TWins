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
        twins::cli::reset();
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
            [&vcalled = ver_called](twins::cli::Argv &argv)
            {
                // with full lambda, captures are possible but costs more
                vcalled = true;
            }
        },
        #else
        {
            "ver|V",
            "    Show SW version; alias 'V'",
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
                twins::cli::exec("    V  ", p_commands);
            }
        },
        {
            "move",
            "<up/dn/home>" "\r\n"
            "    Perform a move",
            TWINS_CLI_HANDLER
            {
                if (argv.size() >= 2)
                    move_dir = *argv[1];
            }
        },
        { /* terminator */ }
    };

    twins::cli::write("ver" "\r\n");
    twins::cli::checkAndExec(commands);
    twins::cli::write("move up" "\r\n");
    EXPECT_TRUE(twins::cli::checkAndExec(commands));
    EXPECT_TRUE(ver_called);
    EXPECT_EQ('u', move_dir);

    // test for alias
    ver_called = false;
    twins::cli::write("V" "\r\n");
    EXPECT_TRUE(twins::cli::checkAndExec(commands));
    EXPECT_TRUE(ver_called);

    // test for call
    p_commands = commands;
    ver_called = false;
    twins::cli::write("call_ver" "\r\n");
    twins::cli::checkAndExec(commands);
    EXPECT_TRUE(ver_called);

    // print history
    twins::cli::write("hist" "\r\n");
    EXPECT_TRUE(twins::cli::checkAndExec(commands));

    // print help
    twins::cli::write("help" "\r\n");
    EXPECT_TRUE(twins::cli::checkAndExec(commands));

    // unknown cmd
    twins::cli::write("say-ello\r\n");
    EXPECT_FALSE(twins::cli::checkAndExec(commands));

    // get history
    EXPECT_GE(twins::cli::getHistory().size(), 2);
    twins::cli::reset();
    EXPECT_EQ(twins::cli::getHistory().size(), 0);
}

TEST_F(CLI, control_codes)
{
    const twins::cli::Cmd commands[] =
    {
        {
            "*HELLO!#",
            "",
            TWINS_CLI_HANDLER {}
        },
        { /* terminator */ }
    };

    // empty command
    twins::cli::write("\r\n");
    EXPECT_FALSE(twins::cli::checkAndExec(commands));

    // put something to history
    twins::cli::write("HELLO\r\n");
    EXPECT_FALSE(twins::cli::checkAndExec(commands));

    // put wrong command and modify it as in terminal
    {
        twins::cli::reset();
        twins::cli::write("HERO");
        // left
        twins::cli::write("\e[D");
        twins::cli::write("\e[D");
        // del R
        twins::cli::write("\e[3~");
        // insert LL
        twins::cli::write("LL");
        // right
        twins::cli::write("\e[C");
        // append !
        twins::cli::write("!");
        // home
        twins::cli::write("\e[H");
        twins::cli::write("*");
        // end
        twins::cli::write("\e[F");
        twins::cli::write("#");
        // and run it
        twins::cli::write("\r");
        EXPECT_TRUE(twins::cli::checkAndExec(commands));
    }

    // no command ready
    EXPECT_FALSE(twins::cli::checkAndExec(commands));

    // up - recall from history
    twins::cli::write("\e[A");
    twins::cli::write("\r\n");
    EXPECT_TRUE(twins::cli::checkAndExec(commands));

    // down
    twins::cli::write("\e[B");
}
