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
 *  @param str input string
 *  @param delim set of whitespace characters delimiting words
 *  @param storeDelim if true, every second entry returned is delimiter before next word
 *  @return vector of pointers to the beginning of text lines
 */
twins::Vector<twins::util::StringRange> splitWords(const char *str, const char *delim = " \t\n", bool storeDelim = false);

/** @brief Insert \p newLine and ellipsis to ensure the line length is always < \p maxLineLen
 *  @param str input string
 *  @param maxLineLen max number of printable characters that fits into line of text
 *  @param delim set of characters where word wrap can occur
 *  @param separator sequence to insert to break too long line
*/
twins::String wordWrap(const char *str, uint16_t maxLineLen, const char *delim = " \t", const char *separator = "\n");

/** @brief Split lines using newline character
 *  @param str input string
 *  @return vector of pointers to the beginning of text lines
 */
twins::Vector<twins::util::StringRange> splitLines(const char *str);

// -----------------------------------------------------------------------------

} // namespace
