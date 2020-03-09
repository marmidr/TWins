/******************************************************************************
 * @brief   TWins - concatenating string buffer
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#pragma once
#include <stdint.h>
#include <string.h>

// -----------------------------------------------------------------------------

namespace twins
{

extern IOs *pIOs;

struct StringBuff
{
    // avoid memory allocation in constructor
    StringBuff() = default;

    ~StringBuff()
    {
        pIOs->mfree(pBuff);
    }
    void resize(uint16_t sz)
    {
        if (!pBuff)
        {
            capacity = sz > 0 ? sz : 8;
            pBuff = (char*)pIOs->malloc(capacity);
            *pBuff = '\0';
            return;
        }

        if (sz > capacity)
        {
            capacity = sz;
            char *pnew = (char*)pIOs->malloc(capacity);
            memcpy(pnew, pBuff, size+1);
            pIOs->mfree(pBuff);
            pBuff = pnew;
        }
    }
    void cat(const char *s)
    {
        int s_len = strlen(s);
        if (size + s_len >= capacity-1)
            resize(capacity + s_len + 10);

        strcat(pBuff, s);
        size += s_len;
        pBuff[size] = '\0';
    }
    void cat(char c)
    {
        if (size + 1 >= capacity-1)
            resize(capacity + 10);

        pBuff[size] = c;
        size += 1;
        pBuff[size] = '\0';
    }
    void clear()
    {
        size = 0;
        if (pBuff) *pBuff = '\0';
    }
    const char *cstr() const
    {
        return pBuff ? pBuff : "";
    }

private:
    char *  pBuff = nullptr;
    int16_t capacity = 0;
    int16_t size = 0;
};

// -----------------------------------------------------------------------------

} // namespace
