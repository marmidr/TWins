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
    String(const char*s);
    String() = default;
    String(String &&other);
    ~String();

    /** @brief Append \p repeat of given string \p s */
    String& append(const char *s, int16_t repeat = 1);
    /** @brief Append new string \p s, size of \p sLen bytes */
    String& appendLen(const char *s, int16_t sLen);
    /** @brief Append \p repeat of given characters \p c */
    String& append(char c, int16_t repeat = 1);
    /** @brief Append new string \p s */
    inline String& append(const String& s) { return append(s.cstr()); }
    /** @brief Append formatted string */
    String& appendFmt(const char *fmt, ...);
    void appendVFmt(const char *fmt, va_list ap);
    /** @brief Trim string that is too long to fit; optionally append ellipsis ... at the \p trimPos */
    String& trim(int16_t trimPos, bool addEllipsis = false, bool ignoreESC = false);
    /** @brief Erase \p len characters from string at \p pos */
    String& erase(int16_t pos, int16_t len = 1);
    /** @brief Insert string \p s at \p pos */
    String& insert(int16_t pos, const char *s, int16_t repeat = 1);
    /** @brief If shorter than \p newWidth - add spaces; if longer - calls trim */
    void setWidth(int16_t newWidth, bool addEllipsis = false);
    /** @brief Set size to zero; release buffer memory only if capacity >= \p threshordToFree */
    String& clear(uint16_t threshordToFree = 500);
    /** @brief Return string size, in bytes */
    unsigned size() const { return mSize; }
    /** @brief Return length of UTF-8 string, ignoring ESC sequences inside it
     *         and recognizing double-width glyphs */
    unsigned u8len(bool ignoreESC = false, bool realWidth = false) const;
    /** @brief Text width on terminal */
    inline unsigned width() { return u8len(true, true); }
    /** @brief Return C-style string buffer */
    const char *cstr() const { return mpBuff ? mpBuff : ""; }
    /** @brief Reserve buffer if u know the string size in advance */
    void reserve(uint16_t newCapacity);
    /** @brief Convenient assign operators */
    String& operator =(const char *s);
    String& operator =(const String &other);
    String& operator =(String &&other);
    /** @brief Convenient append operator */
    String& operator <<(char c) { return append(c); }
    String& operator <<(const char *s) { return append(s); }
    String& operator <<(const String &s) { return appendLen(s.cstr(), s.size()); }

    /** @brief Return ESC sequence length starting at \p str */
    static unsigned escLen(const char *str, const char *strEnd = nullptr);
    /** @brief Return length of UTF-8 string \p str, ignoring ESC sequences inside it
     *         and recognizing double-width glyphs */
    static unsigned u8len(const char *str, const char *strEnd = nullptr, bool ignoreESC = false, bool realWidth = false);
    /** @brief Text width on terminal */
    static inline unsigned width(const char *str, const char *strEnd = nullptr) { return u8len(str, strEnd, true, true); }
    /** @brief Return pointer to \p str moved by \p toSkip UTF-8 characters, omitting ESC sequences */
    static const char* u8skip(const char *str, unsigned toSkip, bool ignoreESC = true);

private:
    void freeBuff();
    bool sourceIsOurs(const char *s) const { return (s >= mpBuff) && (s < mpBuff + mCapacity); }

private:
    char *  mpBuff = nullptr;
    int16_t mCapacity = 0;
    int16_t mSize = 0;
};

// -----------------------------------------------------------------------------

} // namespace
