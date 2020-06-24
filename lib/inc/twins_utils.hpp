/******************************************************************************
 * @brief   TWins - utility code
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#pragma once

#include "twins_string.hpp"
#include "twins_vector.hpp"
#include "twins_common.hpp"

#include <assert.h>

// -----------------------------------------------------------------------------

namespace twins::util
{

/** @brief Like \b strchr() but with limited length */
const char* strnchr(const char *str, int strSz, char c);

/** @brief Split string into separate words using any of \p delim characters as delimiters
 *  @param str input string
 *  @param delim set of whitespace characters delimiting words
 *  @param storeDelim if true, every second entry returned is delimiter before next word
 *  @return vector of pointers to the beginning of text lines
 */
twins::Vector<twins::StringRange> splitWords(const char *str, const char *delim = " \t\n", bool storeDelim = false);

/** @brief Insert \p newLine and ellipsis to ensure the line length is always < \p maxLineLen
 *  @param str input string
 *  @param maxLineLen max number of printable characters that fits into line of text
 *  @param delim set of characters where word wrap can occur
 *  @param separator sequence to insert to break too long line
*/
twins::String wordWrap(const char *str, uint16_t maxLineLen, const char *delim = " \t\n", const char *separator = "\n");

/** @brief Split lines using newline character
 *  @param str input string
 *  @return vector of pointers to the beginning of text lines
 */
twins::Vector<twins::StringRange> splitLines(const char *str);

// -----------------------------------------------------------------------------

/** @brief String helper holding text wrapped and splitted into lines */
struct WrappedString
{
    WrappedString() = default;

    /** @brief */
    void config(uint16_t maxWidth = 250, const char* delim = " \t\n", const char* sep = "\n")
    {
        m_maxWidth = maxWidth;
        if (delim) m_delim = delim;
        if (sep) m_sep = sep;
        m_dirty = true;
    }

    /** @brief */
    void updateLines(const char *newContent = nullptr)
    {
        if (newContent)
        {
            m_sourceStr.clear();
            m_sourceStr.append(newContent);
        }
        m_wrappedStr = wordWrap(m_sourceStr.cstr(), m_maxWidth, m_delim, m_sep);
        m_lines = splitLines(m_wrappedStr.cstr());
        m_dirty = false;
    }

    /** @brief */
    const Vector<StringRange>& getLines()
    {
        if (m_dirty)
            updateLines();
        return m_lines;
    }

    /** @brief */
    String& getSourceStr()
    {
        return m_sourceStr;
    }

    /** @brief */
    const String& getWrappedStr() const
    {
        return m_wrappedStr;
    }

    /** @brief */
    bool isDirty() const { return m_dirty; }

    /** @brief */
    WrappedString& operator =(const char *str)
    {
        m_lines.clear();
        m_sourceStr = str;
        m_dirty = true;
        return *this;
    }

    /** @brief */
    WrappedString& operator =(const String &other)
    {
        *this = other.cstr();
        return *this;
    }

    /** @brief */
    WrappedString& operator =(String &&other)
    {
        m_lines.clear();
        m_sourceStr = std::move(other);
        m_dirty = true;
        return *this;
    }

private:
    bool        m_dirty = false;
    uint16_t    m_maxWidth = 250;
    const char* m_delim = " \t\n";
    const char* m_sep = "\n";
    String      m_sourceStr;
    String      m_wrappedStr;
    Vector<StringRange> m_lines;
};

// -----------------------------------------------------------------------------

} // namespace
