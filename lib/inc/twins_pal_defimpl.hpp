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
#include <time.h>

#include <utility> // std::move

// -----------------------------------------------------------------------------

namespace twins
{

struct DefaultPAL : twins::IPal
{
    int writeStr(const char *s) override
    {
        return printf("%s", s);
    }

    int writeStrAsync(twins::String &&str) override
    {
        return printf("%s", str.cstr());
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
        if (ptr)
        {
            auto allocated = malloc_usable_size(ptr);
            stats.memAllocated -= allocated;
            stats.memChunks--;
            free(ptr);
        }
    }

    void sleep(uint16_t ms) override
    {
        timespec ts = {};
        ts.tv_sec = ms / 1000;
        ms %= 1000;
        ts.tv_nsec = 1'000'000 * ms;
        nanosleep(&ts, nullptr);
    }

    uint16_t getLogsRow() override
    {
        return 0;
    }

    uint32_t getTimeStamp() override
    {
        static timespec ts_at_start;
        if (ts_at_start.tv_sec == 0)
            clock_gettime(CLOCK_MONOTONIC, &ts_at_start);

        timespec ts_now;
        clock_gettime(CLOCK_MONOTONIC, &ts_now);
        uint32_t sec_diff = ts_now.tv_sec - ts_at_start.tv_sec;
        auto msec_diff = (ts_now.tv_nsec - ts_at_start.tv_nsec) / 1'000'000;
        return sec_diff * 1000 + msec_diff;
    }

    uint32_t getTimeDiff(uint32_t timestamp) override
    {
        auto now = getTimeStamp();
        return now - timestamp;
    }

    // statistics
    Stats stats = {};
};

// -----------------------------------------------------------------------------

}
