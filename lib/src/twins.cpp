/******************************************************************************
 * @brief   TWins - core
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#include "twins.hpp"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

// -----------------------------------------------------------------------------

namespace twins
{

// static struct
// {
//     Coord origin;
// } status;

IOs *pIOs;

// -----------------------------------------------------------------------------

void init(IOs *ios)
{
    pIOs = ios;
}

int writeChar(char c, uint16_t count)
{
    char str[2] = {c, '\0'};
    for (uint16_t i = 0; i < count; i++)
        writeStr(str);
    return count;
}

int writeStr(const char *s)
{
    return pIOs->writeStr(s);
}

int writeStrFmt(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int n = pIOs->writeStrFmt(fmt, ap);
    va_end(ap);
    return n;
}

void moveTo(uint16_t col, uint16_t row)
{
    writeStrFmt(ESC_CURSOR_GOTO_FMT, row, col);
}

void moveToCol(uint16_t col)
{

}

void moveBy(int16_t cols, int16_t rows)
{
    if (cols < 0)
        writeStrFmt(ESC_CURSOR_BACKWARD_FMT, -cols);
    else if (cols > 0)
        writeStrFmt(ESC_CURSOR_FORWARD_FMT, cols);


    if (rows < 0)
        writeStrFmt(ESC_CURSOR_UP_FMT, -rows);
    else if (rows > 0)
        writeStrFmt(ESC_CURSOR_DOWN_FMT, rows);
}


// -----------------------------------------------------------------------------

}
