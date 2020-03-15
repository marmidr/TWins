/******************************************************************************
 * @brief   TWins - keyboard input - POSIX
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#pragma once
#include "twins_common.hpp"
#include <stdint.h>

// -----------------------------------------------------------------------------

namespace twins
{

/** @brief Initialze keyboard device reader */
void inputPosixInit(uint16_t timeoutMs);

/** @brief Free keyboard device reader resources */
void inputPosixFree();

/** @brief Read codes from keyboard and store in \p output; check \p output.len if anything was read.
  *        If \p quitRequested is set, application is requested to end
  */
void inputPosixRead(AnsiSequence &output, bool &quitRequested);

// -----------------------------------------------------------------------------

} // namespace
