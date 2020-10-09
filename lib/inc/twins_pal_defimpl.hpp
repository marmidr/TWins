/******************************************************************************
 * @brief   TWins - string class for our needs
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#pragma once

#if defined __linux__ || defined __CYGWIN__ || defined __MSYS__
# define TWINS_ENV_LINUX_LIKE   1
#else
# define TWINS_ENV_LINUX_LIKE   0
#endif

#ifndef TWINS_PAL_FULLIMPL
# define TWINS_PAL_FULLIMPL     1
#endif

// -----------------------------------------------------------------------------

#include "twins_common.hpp"
#include "twins_string.hpp"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

#if TWINS_ENV_LINUX_LIKE
# include <time.h>
# include <unistd.h>
#endif

// -----------------------------------------------------------------------------

namespace twins
{

struct DefaultPAL : twins::IPal
{
    int writeChar(char c, int16_t repeat) override
    {
        auto sz = lineBuff.size();
        lineBuff.append(c, repeat);
        return lineBuff.size() - sz;
    }

    int writeStr(const char *s, int16_t repeat) override
    {
        auto sz = lineBuff.size();
        lineBuff.append(s, repeat);
        return lineBuff.size() - sz;
    }

    int writeStrLen(const char *s, uint16_t sLen) override
    {
        lineBuff.appendLen(s, sLen);
        return sLen;
    }

    int writeStrVFmt(const char *fmt, va_list ap) override
    {
        auto sz = lineBuff.size();
        lineBuff.appendVFmt(fmt, ap);
        return lineBuff.size() - sz;
    }

    void flushBuff() override
    {
    #if TWINS_PAL_FULLIMPL
        if (lineBuff.size())
        {
            fwrite(lineBuff.cstr(), 1, lineBuff.size(), stdout);

            if (lineBuff.size() > lineBuffMaxSize)
            {
                lineBuffMaxSize = lineBuff.size();
            }

            lineBuff.clear(1000);
            lineBuff.reserve(500);
        }

        fflush(stdout);
    #endif
    }

    void* memAlloc(uint32_t size) override
    {
    #if TWINS_PAL_FULLIMPL
        void *ptr = malloc(size);
        auto allocated = malloc_usable_size(ptr);

        stats.memAllocated += allocated;
        if (stats.memAllocated > stats.memAllocatedMax)
            stats.memAllocatedMax = stats.memAllocated;

        stats.memChunks++;
        if (stats.memChunks > stats.memChunksMax)
            stats.memChunksMax = stats.memChunks;

        return ptr;
    #else
        return nullptr;
    #endif
    }

    void memFree(void *ptr) override
    {
    #if TWINS_PAL_FULLIMPL
        if (ptr)
        {
            auto allocated = malloc_usable_size(ptr);
            stats.memAllocated -= allocated;
            stats.memChunks--;
            free(ptr);
        }
    #endif
    }

    void sleep(uint16_t ms) override
    {
    #if TWINS_ENV_LINUX_LIKE && TWINS_PAL_FULLIMPL
        //*
        timespec ts = {};
        ts.tv_sec = ms / 1000;
        ms %= 1000;
        ts.tv_nsec = 1'000'000 * ms;
        nanosleep(&ts, nullptr);
        //*/

        usleep(ms * 1000);
    #endif
    }

    uint16_t getLogsRow() override
    {
        return 0;
    }

    uint32_t getTimeStamp() override
    {
    #if TWINS_ENV_LINUX_LIKE && TWINS_PAL_FULLIMPL
        static timespec ts_at_start;
        if (ts_at_start.tv_sec == 0)
            clock_gettime(CLOCK_MONOTONIC, &ts_at_start);

        timespec ts_now;
        clock_gettime(CLOCK_MONOTONIC, &ts_now);
        uint32_t sec_diff = ts_now.tv_sec - ts_at_start.tv_sec;
        auto msec_diff = (ts_now.tv_nsec - ts_at_start.tv_nsec) / 1'000'000;
        return sec_diff * 1000 + msec_diff;
    #else
        return 0;
    #endif
    }

    uint32_t getTimeDiff(uint32_t timestamp) override
    {
        auto now = getTimeStamp();
        return now - timestamp;
    }

protected:
    void deinit()
    {
        lineBuff.clear(0); // before PAL is unregistered
    }

public:
    String lineBuff;
    uint32_t lineBuffMaxSize = 0;

    // statistics
    Stats stats = {};
};

// -----------------------------------------------------------------------------

}
