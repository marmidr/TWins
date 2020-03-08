/******************************************************************************
 * @brief   TWins - concatenating string buffer
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// -----------------------------------------------------------------------------

namespace twins
{

struct StringBuff
{
    StringBuff()
    {
        capacity = 5;
        size = 0;
        buff = (char*)malloc(capacity);
        *buff = '\0';
    }
    ~StringBuff()
    {
        free(buff);
    }
    void resize(int sz)
    {
        if (sz > capacity)
        {
            capacity = sz;
            buff = (char*)realloc(buff, capacity);
        }
    }
    void cat(const char *s)
    {
        int s_len = strlen(s);
        if (size + s_len >= capacity-1)
            resize(capacity + s_len + 10);

        strcat(buff, s);
        size += s_len;
        buff[size] = '\0';
    }
    void cat(char c)
    {
        if (size + 1 >= capacity-1)
            resize(capacity + 10);

        buff[size] = c;
        size += 1;
        buff[size] = '\0';
    }
    void clear()
    {
        size = 0;
        *buff = '\0';
    }

    char *buff;
    int capacity;
    int size;
};

// -----------------------------------------------------------------------------

} // namespace
