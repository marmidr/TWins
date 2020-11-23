/******************************************************************************
 * @brief   TWins - command line interface
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#pragma once
#include <stdint.h>
#include "twins_vector.hpp"
#include "twins_string.hpp"

// -----------------------------------------------------------------------------

namespace twins::cli
{

#define TWINS_CLI_HANDLER       [](uint8_t argc, const char **argv)

/**
 * @brief Struct holding command name and pointer to handler function
 * if \p argc is 1+, the \p argv holds comamnd arguments
 */
struct Cmd
{
    const char* name;
    const char* help;
    void (*handler)(uint8_t argc, const char **argv);
};


// -----------------------------------------------------------------------------

using History = Vector<String>;

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

// -----------------------------------------------------------------------------

} // namespace
