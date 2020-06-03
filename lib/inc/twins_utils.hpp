/******************************************************************************
 * @brief   TWins - utility code
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#pragma once

#include "twins_string.hpp"
#include "twins_vector.hpp"

// -----------------------------------------------------------------------------

namespace twins
{

/** @brief Split string into separate words using \p delim characters as delimiters */
twins::Vector<twins::String> splitWords(const char *str, const char *delim = " \t\n");

/** @brief  */
twins::String wordWrap(const char *str, uint16_t maxLineLen, const char *newLine = "\n");

// -----------------------------------------------------------------------------

} // namespace
