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

/** @brief Template returning length of array of type T */
void inputPosixInit(uint16_t timeoutMs);
void inputPosixFree();
void inputPosixCheckKeys(twins::KeySequence &output, bool &quitRequested);

// -----------------------------------------------------------------------------

} // namespace
