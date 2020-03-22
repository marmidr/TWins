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

/** @brief Read codes from keyboard and returns pointer to nul-terminated buffer
  *        If \p quitRequested is set, application is requested to end
  * @return nul-terminated C-string
  */
const char * inputPosixRead(bool &quitRequested);

// -----------------------------------------------------------------------------

} // namespace
