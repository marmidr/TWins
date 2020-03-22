/******************************************************************************
 * @brief   TWins - string class for our needs
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#pragma once
#include <stdint.h>
#include <stdarg.h>

// -----------------------------------------------------------------------------

namespace twins
{

/**
 * @brief
 */
class String
{
public:
    // avoid memory allocation in constructor
    String() = default;
    ~String();

    /** @brief Append new string or character or number of given characters */
    void append(const char *s);
    void append(char c, int16_t count = 1);
    void append(const char *s, int16_t count);
    /** @brief Append formatted string */
    void appendFmt(const char *fmt, ...);
    /** @brief Trim string that is too long to fit; optionally append ellipsis ... at the \p trimPos */
    void trim(uint16_t trimPos, bool addEllipsis = false);
    /** @brief Set size to zero; does not release buffer memory */
    void clear();
    /** @brief Return string size, in bytes */
    unsigned size() const { return mSize; }
    /** @brief Return string length, in characters, assuming UTF-8 encoding */
    unsigned utf8Len() const;
    /** @brief Return C-style string buffer */
    const char *cstr() const { return mpBuff ? mpBuff : ""; }
    /** @brief Convenient assign operators */
    void operator=(const char *s);
    void operator=(const String &other) { *this = other.cstr(); }
    void operator=(String &&other);
    /** @brief Releases string content, used with move assign operator=(String&&) */
    String&& release() { return (String&&)*this; }

private:
    void resize(uint16_t newCapacity);
    void free();

private:
    char *  mpBuff = nullptr;
    int16_t mCapacity = 0;
    int16_t mSize = 0;
};

// -----------------------------------------------------------------------------

} // namespace
