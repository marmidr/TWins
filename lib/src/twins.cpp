/******************************************************************************
 * @brief   TWins - core
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#include "twins.hpp"
#include "twins_stack.hpp"

#include <string.h>
#include <stdio.h>

// -----------------------------------------------------------------------------

namespace twins
{

/** @brief TWins I/O structure */
const IOs *pIOs;

/** @brief Line buffer to avoid printing single chars */
static twins::String lineBuff;

/** @brief Current font colors and attributes */
static ColorFG currentClFg = ColorFG::Default;
static ColorBG currentClBg = ColorBG::Default;

/** @brief Font colors and attribute stacks */
static Stack<ColorFG> stackClFg;
static Stack<ColorBG> stackClBg;
static Stack<FontAttrib> stackAttr;

// -----------------------------------------------------------------------------

void init(const IOs *ios)
{
    pIOs = ios;
}

int writeChar(char c, int16_t count)
{
    if (count <= 0)
        return 0;
    lineBuff.clear();
    lineBuff.append(c, count);
    return writeStr(lineBuff.cstr());
}

int writeStr(const char *s)
{
    if (!s) return 0;
    return pIOs->writeStr(s);
}

int writeStrFmt(const char *fmt, ...)
{
    if (!fmt) return 0;
    va_list ap;
    va_start(ap, fmt);
    int n = pIOs->writeStrFmt(fmt, ap);
    va_end(ap);
    return n;
}

// TODO: implement full buffering
void flush()
{
    writeStr(lineBuff.cstr());
    lineBuff.clear();
}

void moveTo(uint16_t col, uint16_t row)
{
    writeStrFmt(ESC_CURSOR_GOTO_FMT, row, col);
}

void moveToCol(uint16_t col)
{
    writeStrFmt(ESC_CURSOR_COLUMN_FMT, col);
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

void pushClrFg(ColorFG cl)
{
    stackClFg.push(currentClFg);
    currentClFg = cl;
    pIOs->writeStr(encodeCl(currentClFg));
}

void popClrFg()
{
    if (stackClFg.size())
    {
        currentClFg = *stackClFg.pop();
        pIOs->writeStr(encodeCl(currentClFg));
    }
}

void resetClrFg()
{
    stackClFg.clear();
    pIOs->writeStr(ESC_FG_DEFAULT);
}

// -----------------------------------------------------------------------------

void pushClrBg(ColorBG cl)
{
    stackClBg.push(currentClBg);
    currentClBg = cl;
    pIOs->writeStr(encodeCl(currentClBg));
}

void popClrBg()
{
    if (stackClBg.size())
    {
        currentClBg = *stackClBg.pop();
        pIOs->writeStr(encodeCl(currentClBg));
    }
}

void resetClrBg()
{
    stackClBg.clear();
    pIOs->writeStr(ESC_BG_DEFAULT);
}

// -----------------------------------------------------------------------------

static int8_t attrFaint = 0;

void pushAttr(FontAttrib attr)
{
    stackAttr.push(attr);

    switch (attr)
    {
    case FontAttrib::Bold:          if (!attrFaint) pIOs->writeStr(ESC_BOLD); break;
    case FontAttrib::Faint:         attrFaint++; pIOs->writeStr(ESC_FAINT); break;
    case FontAttrib::Italics:       pIOs->writeStr(ESC_ITALICS_ON); break;
    case FontAttrib::Underline:     pIOs->writeStr(ESC_UNDERLINE_ON); break;
    case FontAttrib::BlinkSlow:     pIOs->writeStr(ESC_BLINK_SLOW); break;
    case FontAttrib::Inverse:       pIOs->writeStr(ESC_INVERSE_ON); break;
    case FontAttrib::Invisible:     pIOs->writeStr(ESC_INVISIBLE_ON); break;
    case FontAttrib::StrikeThrough: pIOs->writeStr(ESC_STRIKETHROUGH_ON); break;
    default: break;
    }
}

void popAttr()
{
    if (auto *pAttr = stackAttr.pop())
    {
        switch (*pAttr)
        {
        case FontAttrib::Bold:          if (!attrFaint) pIOs->writeStr(ESC_NORMAL); break;
        case FontAttrib::Faint:         attrFaint--; pIOs->writeStr(ESC_NORMAL); break;
        case FontAttrib::Italics:       pIOs->writeStr(ESC_ITALICS_OFF); break;
        case FontAttrib::Underline:     pIOs->writeStr(ESC_UNDERLINE_OFF); break;
        case FontAttrib::BlinkSlow:     pIOs->writeStr(ESC_BLINK_OFF); break;
        case FontAttrib::Inverse:       pIOs->writeStr(ESC_INVERSE_OFF); break;
        case FontAttrib::Invisible:     pIOs->writeStr(ESC_INVISIBLE_OFF); break;
        case FontAttrib::StrikeThrough: pIOs->writeStr(ESC_STRIKETHROUGH_OFF); break;
        default: break;
        }
    }
}

void resetAttr()
{
    attrFaint = 0;
    stackAttr.clear();
    pIOs->writeStr(ESC_ATTRIBUTES_DEFAULT);
}

// -----------------------------------------------------------------------------

}
