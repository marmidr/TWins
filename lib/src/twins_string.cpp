/******************************************************************************
 * @brief   TWins - string class for our needs
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#include "twins_common.hpp"
#include "twins_string.hpp"
#include "twins_utf8str.hpp"

#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <utility> //std::swap

// -----------------------------------------------------------------------------

namespace twins
{

String::String(const char*s)
{
    assert(pPAL);
    append(s);
}

String::String(String &&other)
{
    *this = std::move(other);
}

String::~String()
{
    if (mpBuff)
        pPAL->memFree(mpBuff);
}

String& String::append(const char *s, int16_t repeat)
{
    if (repeat <= 0 || !s || sourceIsOurs(s))
        return *this;

    int s_len = strlen(s);
    reserve(mSize + repeat * s_len);
    char *p = mpBuff + mSize;
    mSize += repeat  *s_len;
    while (repeat--)
        p = strcat(p, s);
    mpBuff[mSize] = '\0';
    return *this;
}

String& String::appendLen(const char *s, int16_t sLen)
{
    if (sLen <= 0 || !s || sourceIsOurs(s))
        return *this;

    reserve(mSize + sLen);
    strncat(mpBuff + mSize, s, sLen);
    mSize += sLen;
    mpBuff[mSize] = '\0';
    return *this;
}

String& String::append(char c, int16_t repeat)
{
    if (repeat <= 0) return *this;
    reserve(mSize + repeat);
    char *p = mpBuff + mSize;
    mSize += repeat;
    while (repeat--)
        *p++ = c;
    mpBuff[mSize] = '\0';
    return *this;
}

String& String::appendFmt(const char *fmt, ...)
{
    if (!fmt) return *this;

    va_list ap;
    va_start(ap, fmt);
    appendVFmt(fmt, ap);
    va_end(ap);
    return *this;
}

void String::appendVFmt(const char *fmt, va_list ap)
{
    // https://en.cppreference.com/w/cpp/io/c/fprintf
    uint8_t retry = 1;

    do
    {
        va_list ap_copy;
        va_copy(ap_copy, ap);
        int freespace = mCapacity - mSize;
        int n = vsnprintf(mpBuff + mSize, freespace, fmt, ap_copy);

        if (n > 0)
        {
            // everything ok
            if (n < freespace)
            {
                mSize += n;
                break;
            }

            // printf("too small buffer\n");
            reserve(mCapacity + n + 10);
        }
        else
        {
            // printf("err\n");
            break;
        }

    } while (retry--);
}

String& String::trim(int16_t trimPos, bool addEllipsis, bool ignoreESC)
{
    if (trimPos < 0 || trimPos >= mSize)
        return *this;
    if (addEllipsis && trimPos > 0)
        trimPos--;

    char *p = mpBuff;

    if (ignoreESC)
    {
        p = const_cast<char*>(u8skipIgnoreEsc(p, trimPos));
    }
    else
    {
        for (int i = 0; i < trimPos; i++)
        {
            int seqLen = utf8seqlen(p);
            if (seqLen <= 0) break;
            p += seqLen;
        }
    }

    if (p >= mpBuff + mSize) return *this;
    mSize = p - mpBuff;
    char last = mpBuff[mSize];

    if (addEllipsis && last == ' ') 
        mSize++;
    mpBuff[mSize] = '\0';
    if (addEllipsis && last != ' ') 
        append("…");
    return *this;
}

String& String::erase(int16_t pos, int16_t len)
{
    if (pos < 0 || pos >= mSize || len <= 0)
        return *this;

    char *p = mpBuff;

    for (int i = 0; i < pos; i++)
    {
        int seqLen = utf8seqlen(p);
        if (seqLen <= 0) break;
        p += seqLen;
    }

    if (p >= mpBuff + mSize) 
        return *this;

    char *erase_at = p;
    unsigned bytes_to_erase = 0;

    for (int i = 0; i < len; i++)
    {
        int seqLen = utf8seqlen(p);
        if (seqLen <= 0) break;
        bytes_to_erase += seqLen;
        p += seqLen;
        if (p >= mpBuff + mSize)
            break;
    }

    memmove(erase_at, erase_at + bytes_to_erase, mSize - (erase_at - mpBuff));
    mSize -= bytes_to_erase;
    mpBuff[mSize] = '\0';
    return *this;
}

String& String::insert(int16_t pos, const char *s)
{
    if (pos < 0 || (!s || !*s) || sourceIsOurs(s))
        return *this;

    if ((unsigned)pos >= u8len())
    {
        append(s);
        return *this;
    }

    char *p = mpBuff;

    for (int i = 0; i < pos; i++)
    {
        int seqLen = utf8seqlen(p);
        if (seqLen <= 0) break;
        p += seqLen;
    }

    if (p >= mpBuff + mSize) 
        return *this;

    char *insert_at = p;
    unsigned bytes_to_insert = strlen(s);

    reserve(mSize + bytes_to_insert);
    memmove(insert_at + bytes_to_insert, insert_at, mSize - (insert_at - mpBuff));
    memmove(insert_at, s, bytes_to_insert);
    mSize += bytes_to_insert;
    mpBuff[mSize] = '\0';
    return *this;
}

void String::setLength(int16_t len, bool addEllipsis, bool ignoreESC)
{
    if (len < 0)
        return;

    int u8length = u8len(ignoreESC);

    if (u8length <= len)
        append(' ', len - u8length);
    else
        trim(len, addEllipsis, ignoreESC);
}

String& String::clear(uint16_t threshordToFree)
{
    if (mCapacity >= threshordToFree)
        free();

    mSize = 0;
    if (mpBuff)
        *mpBuff = '\0';
        
    return *this;
}

String& String::operator=(const char *s)
{
    if (!sourceIsOurs(s))
    {
        clear();
        append(s);
    }
    return *this;
}

String& String::operator =(const String &other)
{
    if (this != &other)
        *this = other.cstr();

    return *this;
}

String& String::operator=(String &&other)
{
    if (this != &other)
    {
        std::swap(mpBuff,    other.mpBuff);
        std::swap(mCapacity, other.mCapacity);
        std::swap(mSize,     other.mSize);
    }
    return *this;
}

unsigned String::u8len(bool ignoreESC) const
{
    return u8len(mpBuff, mpBuff + mSize, ignoreESC);
}

void String::reserve(uint16_t newCapacity)
{
    if (newCapacity < mCapacity)
        return;

    newCapacity++;  // extra byte for NUL

    if (newCapacity & (32-1))
    {
        // avoid allocating every time 1-byte more
        newCapacity &= ~(32-1);
        newCapacity += 32;
    }

    if (!mpBuff)
    {
        // first-time buffer allocation
        mpBuff = (char*)pPAL->memAlloc(newCapacity);
        mCapacity = newCapacity;
        *mpBuff = '\0';
        mSize = 0;
        return;
    }

    if (newCapacity > mCapacity)
    {
        // reallocation needed
        char *pnew = (char*)pPAL->memAlloc(newCapacity);
        mCapacity = newCapacity;
        memcpy(pnew, mpBuff, mSize+1);
        pPAL->memFree(mpBuff);
        mpBuff = pnew;
    }
}

void String::free()
{
    if (mpBuff) pPAL->memFree(mpBuff);
    mpBuff = nullptr;
    mCapacity = 0;
    mSize = 0;
}

unsigned String::escLen(const char *str, const char *strEnd)
{
    // ESC sequence always ends with:
    // - A..Z
    // - a..z
    // - @, ^, ~

    // ESC_BG_RGB(r,g,b)    \e[48;2;255;255;255m
    constexpr uint8_t esc_max_seq_len = 20;
    unsigned max_sl;

    if (strEnd && (strEnd - str < esc_max_seq_len))
        max_sl = strEnd - str;
    else
        max_sl = esc_max_seq_len;

    if (str && *str == '\e')
    {
        unsigned sl = 1;
        while (sl < max_sl)
        {
            const char c = str[sl];

            switch (c)
            {
            case '\0':
                return 0;
            case '@':
            case '^':
            case '~':
                return sl + 1;
            case 'M':
                return ((max_sl >= 6) && (strnlen(str, 6) >= 6)) ? 6 : 0;
            default:
                if (c >= 'A' && c <= 'Z' && c != 'O')
                    return sl + 1;
                if (c >= 'a' && c <= 'z')
                    return sl + 1;
                break;
            }

            sl++;
        }
    }

    return 0;
}

unsigned String::u8len(const char *str, const char *strEnd, bool ignoreESC)
{
    if (!str || !*str)
        return 0;

    if (!ignoreESC)
        return (unsigned)utf8len(str);

    if (!strEnd)
        strEnd = str + strlen(str);

    unsigned len = 0;

    while (str < strEnd)
    {
        unsigned esc_len = escLen(str, strEnd);
        bool seq_found = esc_len > 0;

        for (; esc_len && (str < strEnd); )
        {
            str += esc_len;
            esc_len = escLen(str, strEnd);
        }

        if (str < strEnd)
        {
            if (int u8_len = utf8seqlen(str))
            {
                seq_found = true;
                len++;
                str += u8_len;
            }
        }

        // nothing recognized? - string illformed
        if (!seq_found)
            break;
    }

    return len;
}

const char* String::u8skipIgnoreEsc(const char *str, unsigned toSkip)
{
    if (!str || !*str)
        return "";

    const char *str_end = str + strlen(str);
    unsigned skipped = 0;

    while (str < str_end && skipped < toSkip)
    {
        unsigned esc_len = escLen(str);
        bool seq_found = esc_len > 0;

        for (; esc_len && (str < str_end); )
        {
            str += esc_len;
            esc_len = escLen(str);
        }

        if (str < str_end)
        {
            if (int u8_len = utf8seqlen(str))
            {
                seq_found = true;
                skipped++;
                str += u8_len;
            }
        }

        // nothing recognized? - string illformed
        if (!seq_found)
            break;
    }

    return str;
}


// -----------------------------------------------------------------------------

} // namespace
