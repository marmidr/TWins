/******************************************************************************
 * @brief   TWins - utility code
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#pragma once

#include "twins_string.hpp"
#include "twins_vector.hpp"

// -----------------------------------------------------------------------------

namespace twins::util
{

/** @brief Structure to store pointer to first element and their number */
template <typename T>
struct Range
{
    T* data = {};
    unsigned size = {};
};

using StringRange = twins::util::Range<const char>;



/** @brief Split string into separate words using any of \p delim characters as delimiters
 *  @return vector of pointers to the beginning of text lines */
twins::Vector<twins::util::StringRange> splitWords(const char *str, const char *delim = " \t\n");

/** @brief Split string into separate words using any of \p delim characters as delimiters */
twins::Vector<twins::String> splitWordsCpy(const char *str, const char *delim = " \t\n");

/** @brief Insert \p newLine and ellipsis to ensure the line length is always < \p maxLineLen */
twins::String wordWrap(const char *str, uint16_t maxLineLen, const char *newLine = "\n");

/** @brief Split lines using newline character
 *  @return vector of pointers to the beginning of text lines */
twins::Vector<twins::util::StringRange> splitLines(const char *str);

// -----------------------------------------------------------------------------

} // namespace
