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

    twins::cli::process("ver" "\r\n");
    twins::cli::checkAndExec(commands);
    twins::cli::process("move up" "\r\n");
    EXPECT_TRUE(twins::cli::checkAndExec(commands));
    EXPECT_TRUE(ver_called);
    EXPECT_EQ('u', move_dir);

    // test for alias
    ver_called = false;
    twins::cli::process("V" "\r\n");
    EXPECT_TRUE(twins::cli::checkAndExec(commands));
    EXPECT_TRUE(ver_called);

    // test for call
    p_commands = commands;
    ver_called = false;
    twins::cli::process("call_ver" "\r\n");
    twins::cli::checkAndExec(commands);
    EXPECT_TRUE(ver_called);

    // print history
    twins::cli::process("hist" "\r\n");
    EXPECT_TRUE(twins::cli::checkAndExec(commands));

    // print help
    twins::cli::process("help" "\r\n");
    EXPECT_TRUE(twins::cli::checkAndExec(commands));

    // unknown cmd
    twins::cli::process("say-ello\r\n");
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
    twins::cli::process("\r\n");
    EXPECT_FALSE(twins::cli::checkAndExec(commands));

    // put something to history
    twins::cli::process("HELLO\r\n");
    EXPECT_FALSE(twins::cli::checkAndExec(commands));

    // put wrong command and modify it as in terminal
    {
        twins::cli::reset();
        twins::cli::process("HERO");
        // left
        twins::cli::process("\e[D");
        twins::cli::process("\e[D");
        // del R
        twins::cli::process("\e[3~");
        // insert LL
        twins::cli::process("LL");
        // right
        twins::cli::process("\e[C");
        // append !
        twins::cli::process("!");
        // home
        twins::cli::process("\e[H");
        twins::cli::process("*");
        // end
        twins::cli::process("\e[F");
        twins::cli::process("#");
        // and run it
        twins::cli::process("\r");
        EXPECT_TRUE(twins::cli::checkAndExec(commands));
    }

    // no command ready
    EXPECT_FALSE(twins::cli::checkAndExec(commands));

    // up - recall from history
    twins::cli::process("\e[A");
    twins::cli::process("\r\n");
    EXPECT_TRUE(twins::cli::checkAndExec(commands));

    // down
    twins::cli::process("\e[B");
}

static std::string args_value;

TEST_F(CLI, quoted_args)
{
    const twins::cli::Cmd commands[] =
    {
        {
            "name",
            "  <\"first name\">",
            TWINS_CLI_HANDLER
            {
                if (argv.size() >= 3)
                {
                    args_value =  argv[1];
                    args_value += " - ";
                    args_value += argv[2];
                }
            }
        },
        { /* terminator */ }
    };

    // no arg
    args_value.clear();
    twins::cli::process("name" "\r\n");
    EXPECT_TRUE(twins::cli::checkAndExec(commands));
    EXPECT_TRUE(args_value.empty());

    // arg ok
    args_value.clear();
    twins::cli::process("name   Tiamat \"Heaven Of High\" -s TFG" "\r\n");
    EXPECT_TRUE(twins::cli::checkAndExec(commands));
    EXPECT_STREQ(args_value.c_str(), "Tiamat - Heaven Of High");

    // missing closing quote
    args_value.clear();
    twins::cli::process(" name Therion \"Clavicula 🔱 Nox" "\r\n");
    EXPECT_TRUE(twins::cli::checkAndExec(commands));
    EXPECT_STREQ(args_value.c_str(), "Therion - Clavicula 🔱 Nox");

    // double
    // args_value.clear();
    // twins::cli::process("nasib \"\"The Mawarannahr\"\"" "\r\n");
    // EXPECT_TRUE(twins::cli::checkAndExec(commands));
    // EXPECT_STREQ(args_value.c_str(), "nasib \"The Mawarannahr\"");
}
