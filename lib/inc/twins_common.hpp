/******************************************************************************
 * @brief   TWins - common definitions
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#pragma once
#include <stdint.h>
#include <stdarg.h>

// -----------------------------------------------------------------------------

namespace twins
{

/** @brief Template returning length of array of type T */
template<unsigned N, typename T>
unsigned arrSize(const T (&arr)[N]) { return N; }


/**
 * @brief
 */
struct IOs
{
    int     (*writeStr)(const char *s);
    int     (*writeStrFmt)(const char *fmt, va_list ap);
    void *  (*malloc)(uint32_t sz);
    void    (*mfree)(void *ptr);
};

extern const IOs *pIOs;

// -----------------------------------------------------------------------------

} // namespace
