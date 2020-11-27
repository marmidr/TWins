/******************************************************************************
 * @brief   TWins - command line interface
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#pragma once
#include "twins_vector.hpp"
#include "twins_string.hpp"

#include <stdint.h>

#if !TWINS_LIGHTWEIGHT_CMD
# include <functional>
#endif

// -----------------------------------------------------------------------------

namespace twins::cli
{

using Argv = Vector<const char*>;

#define TWINS_CLI_HANDLER  [](twins::cli::Argv &argv)

/**
 * @brief Struct holding command name and pointer to handler function
 * if \p argc is 1+, the \p argv holds comamnd arguments
 */
struct Cmd
{
    const char* name;
    const char* help;
    #if TWINS_LIGHTWEIGHT_CMD
    void (*handler)(Argv &argv);
    #else
    std::function<void(Argv &argv)> handler;
    #endif
};


// -----------------------------------------------------------------------------

using History = Vector<String>;

/**
 * @brief Controls parser debug output
 */
extern bool verbose;

/**
 * @brief Clear command line buffer
 */
void reset(void);

/**
 * @brief Append \p str to line buffer, emit echo
 */
void write(const char* str, uint8_t str_len = 0);

/**
 * @brief New line >
 */
void prompt(bool newln = true);

/**
 * @brief Returns the command history
 */
History& getHistory(void);

/**
 * @brief If line ends with '\r', call the matching \p commands handler
 * @param pCommands array of \b Cmd, terminated with empty cmd {}
 * @return true if command line is complete and handler was found and executed
 */
bool checkAndExec(const Cmd* pCommands);

/**
 * @brief Execute command line \p cmdline.
 * @param cmdline command and arguments; terminator \b \r not required
 * @param pCommands array of \b Cmd, terminated with empty cmd {}
 * @return true if handler was found and executed
 */
bool exec(const char *cmdline, const Cmd* pCommands);

// -----------------------------------------------------------------------------

} // namespace
