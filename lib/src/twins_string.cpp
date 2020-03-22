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

// -----------------------------------------------------------------------------

namespace twins
{

String::~String()
{
    if (mpBuff)
        pIOs->mfree(mpBuff);
}

void String::append(const char *s)
{
    if (!s) return;
    int s_len = strlen(s);
    resize(mSize + s_len);
    strcat(mpBuff, s);
    mSize += s_len;
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

void String::append(const char *s, int16_t count)
{
    if (count <= 0) return;
    if (!s) return;
    int s_len = strlen(s);
    resize(mSize + count*s_len);
    char *p = mpBuff + mSize;
    mSize += count*s_len;
    while (count--)
        p = strcat(p, s);
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

void String::trim(uint16_t trimPos, bool addEllipsis)
{
    if (trimPos >= mSize)
        return;
    if (addEllipsis && trimPos > 0)
        trimPos--;

    char *p = mpBuff;
    char *pEnd = mpBuff + mSize;

    for (int pos = 0; pos < trimPos; pos++)
    {
        int seqLen = utf8seqlen(p);
        if (seqLen <= 0) break;
        p += seqLen;
    }

    if (p >= pEnd) return;
    mSize = p - mpBuff;
    char last = mpBuff[mSize];

    if (addEllipsis && last == ' ') mSize++;
    mpBuff[mSize] = '\0';
    if (addEllipsis && last != ' ') append("…");
}

void String::clear()
{
    if (mCapacity > 1000)
        free();

    mSize = 0;
    if (mpBuff) *mpBuff = '\0';
}

void String::operator=(const char *s)
{
    clear();
    append(s);
}

void String::operator=(String &&other)
{
    free();
    mpBuff    = other.mpBuff;
    mCapacity = other.mCapacity;
    mSize     = other.mSize;

    other.mpBuff = nullptr;
    other.free();
}

unsigned String::utf8Len() const
{
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
        mpBuff = (char*)pIOs->malloc(newCapacity);
        mCapacity = newCapacity;
        *mpBuff = '\0';
        mSize = 0;
        return;
    }

    if (newCapacity > mCapacity)
    {
        // reallocation needed
        char *pnew = (char*)pIOs->malloc(newCapacity);
        mCapacity = newCapacity;
        memcpy(pnew, mpBuff, mSize+1);
        pIOs->mfree(mpBuff);
        mpBuff = pnew;
    }
}

void String::free()
{
    if (mpBuff) pIOs->mfree(mpBuff);
    mpBuff = nullptr;
    mCapacity = 0;
    mSize = 0;
}

// -----------------------------------------------------------------------------

} // namespace
