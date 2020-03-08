/******************************************************************************
 * @brief   TWins - keyboard input - POSIX
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#pragma once
#include <stdint.h>

// -----------------------------------------------------------------------------

namespace twins
{

/** @brief Template returning length of array of type T */
void inputPosixInit(uint16_t timeoutMs);
void inputPosixFree();
const char* inputPosixCheckInput(bool &quitRequested);

// -----------------------------------------------------------------------------

} // namespace
