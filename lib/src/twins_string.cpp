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
#include <utility> //std::swap

// -----------------------------------------------------------------------------

namespace twins
{

String::String(String &&other)
{
    *this = std::move(other);
}

String::~String()
{
    if (mpBuff)
        pIOs->memFree(mpBuff);
}

void String::append(const char *s, int16_t count)
{
    if (count <= 0) return;
    if (!s) return;
    int s_len = strlen(s);
    resize(mSize + count * s_len);
    char *p = mpBuff + mSize;
    mSize += count  *s_len;
    while (count--)
        p = strcat(p, s);
    mpBuff[mSize] = '\0';
}

void String::appendLen(const char *s, int16_t sLen)
{
    if (sLen <= 0) return;
    if (!s) return;
    resize(mSize + sLen);
    strncat(mpBuff + mSize, s, sLen);
    mSize += sLen;
    mpBuff[mSize] = '\0';
}

void String::append(char c, int16_t count)
{
    if (count <= 0) return;
    resize(mSize + count);
    char *p = mpBuff + mSize;
    mSize += count;
    while (count--)
        *p++ = c;
    mpBuff[mSize] = '\0';
}

void String::appendFmt(const char *fmt, ...)
{
    if (!fmt) return;

    // https://en.cppreference.com/w/cpp/io/c/fprintf
    uint8_t retry = 1;

    do
    {
        va_list ap;
        va_start(ap, fmt);
        int freespace = mCapacity - mSize;
        int n = vsnprintf(mpBuff + mSize, freespace, fmt, ap);
        va_end(ap);

        if (n > 0)
        {
            // everything ok
            if (n < freespace)
            {
                mSize += n;
                break;
            }

            // too small buffer
            resize(mCapacity + n + 10);
        }
        else
        {
            // error
            break;
        }

    } while (retry--);
}

void String::trim(int16_t trimPos, bool addEllipsis, bool ignoreESC)
{
    if (trimPos < 0 || trimPos >= mSize)
        return;
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

    if (p >= mpBuff + mSize) return;
    mSize = p - mpBuff;
    char last = mpBuff[mSize];

    if (addEllipsis && last == ' ') mSize++;
    mpBuff[mSize] = '\0';
    if (addEllipsis && last != ' ') append("…");
}

void String::erase(int16_t pos, int16_t len)
{
    if (pos < 0 || pos >= mSize)
        return;
    if (len <= 0)
        return;

    char *p = mpBuff;

    for (int i = 0; i < pos; i++)
    {
        int seqLen = utf8seqlen(p);
        if (seqLen <= 0) break;
        p += seqLen;
    }

    if (p >= mpBuff + mSize) return;

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

    memcpy(erase_at, erase_at + bytes_to_erase, mSize - (erase_at - mpBuff));
    mSize -= bytes_to_erase;
    mpBuff[mSize] = '\0';
}

void String::insert(int16_t pos, const char *s)
{
    if (pos < 0)
        return;
    if (!s || !*s)
        return;

    if ((unsigned)pos >= u8len())
    {
        append(s);
        return;
    }

    char *p = mpBuff;

    for (int i = 0; i < pos; i++)
    {
        int seqLen = utf8seqlen(p);
        if (seqLen <= 0) break;
        p += seqLen;
    }

    if (p >= mpBuff + mSize) return;

    char *insert_at = p;
    unsigned bytes_to_insert = strlen(s);

    resize(mSize + bytes_to_insert);
    memcpy(insert_at + bytes_to_insert, insert_at, mSize - (insert_at - mpBuff));
    memcpy(insert_at, s, bytes_to_insert);
    mSize += bytes_to_insert;
    mpBuff[mSize] = '\0';
}

void String::setLength(int16_t len, bool addEllipsis, bool ignoreESC)
{
    if (len < 0)
        return;

    int u8len = ignoreESC ? u8lenIgnoreEsc(mpBuff) : utf8len(mpBuff);

    if (u8len <= len)
        append(' ', len - u8len);
    else
        trim(len, addEllipsis, ignoreESC);
}

void String::clear()
{
    if (mCapacity > 1000)
        free();

    mSize = 0;
    if (mpBuff) *mpBuff = '\0';
}

String& String::operator=(const char *s)
{
    clear();
    append(s);
    return *this;
}

String& String::operator=(String &&other)
{
    std::swap(mpBuff,    other.mpBuff);
    std::swap(mCapacity, other.mCapacity);
    std::swap(mSize,     other.mSize);
    return *this;
}

unsigned String::u8len(bool ignoreESC) const
{
    if (ignoreESC)
        return u8lenIgnoreEsc(mpBuff);
    else
        return (unsigned)utf8len(mpBuff);
}

void String::resize(uint16_t newCapacity)
{
    if (newCapacity < mCapacity)
        return;

    newCapacity++;  // extra byte for NUL

    if (newCapacity - mCapacity < 32)
    {
        // avoid allocating every time 1-byte more
        newCapacity += 32 - (newCapacity - mCapacity);
    }

    if (!mpBuff)
    {
        // first-time buffer allocation
        mpBuff = (char*)pIOs->memAlloc(newCapacity);
        mCapacity = newCapacity;
        *mpBuff = '\0';
        mSize = 0;
        return;
    }

    if (newCapacity > mCapacity)
    {
        // reallocation needed
        char *pnew = (char*)pIOs->memAlloc(newCapacity);
        mCapacity = newCapacity;
        memcpy(pnew, mpBuff, mSize+1);
        pIOs->memFree(mpBuff);
        mpBuff = pnew;
    }
}

void String::free()
{
    if (mpBuff) pIOs->memFree(mpBuff);
    mpBuff = nullptr;
    mCapacity = 0;
    mSize = 0;
}

unsigned String::escLen(const char *str)
{
    // ESC sequence always ends with:
    // - A..Z
    // - a..z
    // - @, ^, ~
    constexpr int esc_max_seq_len = 7;

    if (str && *str == '\e')
    {
        unsigned n = 1;
        while (n < esc_max_seq_len)
        {
            const char c = str[n];

            switch (c)
            {
            case '\0':
                return 0;
            case '@':
            case '^':
            case '~':
                return n+1;
            default:
                if (c >= 'A' && c <= 'Z') return n+1;
                if (c >= 'a' && c <= 'z') return n+1;
                break;
            }

            n++;
        }
    }

    return 0;
}

unsigned String::u8lenIgnoreEsc(const char *str)
{
    if (!str || !*str)
        return 0;

    const char *str_end = str + strlen(str);
    unsigned len = 0;

    while (str < str_end)
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
