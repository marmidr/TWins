/******************************************************************************
 * @brief   TWins - string class for our needs
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#pragma once
#include "twins_common.hpp"
#include "twins_string.hpp"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

#include <utility> // std::move

// -----------------------------------------------------------------------------

namespace twins
{

struct DefaultIOs : twins::IOs
{
    int writeStr(const char *s) override
    {
        return printf("%s", s);
    }

    int writeStrAsync(twins::String &&str) override
    {
        asyncStr = std::move(str);
        return printf("%s", asyncStr.cstr());
    }

    int writeStrFmt(const char *fmt, va_list ap) override
    {
        return vprintf(fmt, ap);
    }

    void flushBuff() override
    {
        fflush(stdout);
    }

    void* memAlloc(uint32_t size) override
    {
        void *ptr = malloc(size);
        auto allocated = malloc_usable_size(ptr);

        stats.memAllocated += allocated;
        if (stats.memAllocated > stats.memAllocatedMax)
            stats.memAllocatedMax = stats.memAllocated;

        stats.memChunks++;
        if (stats.memChunks > stats.memChunksMax)
            stats.memChunksMax = stats.memChunks;

        return ptr;
    }

    void memFree(void *ptr) override
    {
        auto allocated = malloc_usable_size(ptr);
        stats.memAllocated -= allocated;
        stats.memChunks--;
        free(ptr);
    }

    uint16_t getLogsRow() override
    {
        return 0;
    }

    // statistics
    Stats stats = {};

    // string for async write
    twins::String asyncStr;
};

// -----------------------------------------------------------------------------

}
