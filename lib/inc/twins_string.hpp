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
    String(const char*) = delete;
    String() = default;
    String(String &&other);
    ~String();

    /** @brief Append \p repeat of given string \p s */
    void append(const char *s, int16_t repeat = 1);
    /** @brief Append new string \p s, size of \p sLen bytes */
    void appendLen(const char *s, int16_t sLen);
    /** @brief Append \p repeat of given characters \p c */
    void append(char c, int16_t repeat = 1);
    /** @brief Append formatted string */
    void appendFmt(const char *fmt, ...);
    void appendVFmt(const char *fmt, va_list ap);
    /** @brief Trim string that is too long to fit; optionally append ellipsis ... at the \p trimPos */
    void trim(int16_t trimPos, bool addEllipsis = false, bool ignoreESC = false);
    /** @brief Erase \p len characters from string at \p pos */
    void erase(int16_t pos, int16_t len = 1);
    /** @brief Insert string \p s at \p pos */
    void insert(int16_t pos, const char *s);
    /** @brief If shorter than len - add spaces; if longer - calls trim */
    void setLength(int16_t len, bool addEllipsis = false, bool ignoreESC = false);
    /** @brief Set size to zero; release buffer memory only if capacity >= \p threshordToFree */
    void clear(uint16_t threshordToFree = 500);
    /** @brief Return string size, in bytes */
    unsigned size() const { return mSize; }
    /** @brief Return string length, in characters, assuming UTF-8 encoding */
    unsigned u8len(bool ignoreESC = false) const;
    /** @brief Return C-style string buffer */
    const char *cstr() const { return mpBuff ? mpBuff : ""; }
    /** @brief Reserve buffer if u know the string size in advance */
    void reserve(uint16_t newCapacity);
    /** @brief Convenient assign operators */
    String& operator =(const char *s);
    String& operator =(const String &other);
    String& operator =(String &&other);
    /** @brief Convenient append operator */
    String& operator <<(char c) { append(c); return *this; }
    String& operator <<(const char *s) { append(s); return *this; }
    String& operator <<(const String &other) { append(other.cstr()); return *this; }

    /** @brief Return ESC sequence length starting at \p str */
    static unsigned escLen(const char *str, const char *strEnd = nullptr);
    /** @brief Return length of UTF-8 string \p str, ignoring ESC sequences inside it */
    static unsigned u8len(const char *str, const char *strEnd = nullptr, bool ignoreESC = false);
    /** @brief Return pointer to \p str moved by \p toSkip UTF-8 characters, omitting ESC sequences */
    static const char* u8skipIgnoreEsc(const char *str, unsigned toSkip);

private:
    void free();
    bool sourceIsOurs(const char *s) const { return (s >= mpBuff) && (s < mpBuff + mCapacity); }

private:
    char *  mpBuff = nullptr;
    int16_t mCapacity = 0;
    int16_t mSize = 0;
};

// -----------------------------------------------------------------------------

} // namespace
