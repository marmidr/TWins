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

static IOs *pIOs;

// -----------------------------------------------------------------------------

void init(IOs *ios)
{
    pIOs = ios;
}

void writeStr(const char *s)
{
    pIOs->writeStr(s);
}

void writeStrFmt(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    pIOs->writeStrFmt(fmt, ap);
    va_end(ap);
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
