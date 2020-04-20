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
IOs *pIOs;

/** @brief Line buffer to avoid printing single chars */
static twins::String lineBuff;

static bool writesBuffered = false;

/** @brief Current font colors and attributes */
static ColorFG currentClFg = ColorFG::Default;
static ColorBG currentClBg = ColorBG::Default;

/** @brief Font colors and attribute stacks */
static Stack<ColorFG> stackClFg;
static Stack<ColorBG> stackClBg;
static Stack<FontAttrib> stackAttr;

// -----------------------------------------------------------------------------

/** */
FontMemento::FontMemento()
{
    szFg = stackClFg.size();
    szBg = stackClBg.size();
    szAttr = stackAttr.size();
}

FontMemento::~FontMemento()
{
    popClFg(stackClFg.size() - szFg);
    popClBg(stackClBg.size() - szBg);
    popAttr(stackAttr.size() - szAttr);
}

// -----------------------------------------------------------------------------

void init(IOs *ios)
{
    pIOs = ios;
    writeStr(ESC_MOUSE_REPORTING_M2_ON);
}

void bufferBegin()
{
    writesBuffered = true;
}

void bufferEnd()
{
    writesBuffered = false;
    // TODO: flushBuffer()
}

// TODO: these writeX functions duplicates twins::String;
// make lineBuff work harder and use more buffering and asynchronous flush
int writeChar(char c, int16_t count)
{
    if (count <= 0)
        return 0;
    lineBuff.clear();
    lineBuff.append(c, count);
    return writeStr(lineBuff.cstr());
}

int writeStr(const char *s, int16_t count)
{
    if (!s || count <= 0)
        return 0;

    if (count == 1)
        return pIOs->writeStr(s);

    lineBuff.clear();
    lineBuff.append(s, count);
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
void flushBuffer()
{
    // give the line buffer to the HAL so it can asynchronicaly send it and release
    //writeStr(std::move(lineBuff));
    //lineBuff.clear();
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

void pushClFg(ColorFG cl)
{
    stackClFg.push(currentClFg);
    currentClFg = cl;
    pIOs->writeStr(encodeCl(currentClFg));
}

void popClFg(int n)
{
    while (stackClFg.size() && n-- > 0)
        currentClFg = *stackClFg.pop();

    pIOs->writeStr(encodeCl(currentClFg));
}

void resetClFg()
{
    stackClFg.clear();
    pIOs->writeStr(ESC_FG_DEFAULT);
}

// -----------------------------------------------------------------------------

void pushClBg(ColorBG cl)
{
    stackClBg.push(currentClBg);
    currentClBg = cl;
    pIOs->writeStr(encodeCl(currentClBg));
}

void popClBg(int n)
{
    while (stackClBg.size() && n-- > 0)
        currentClBg = *stackClBg.pop();

    pIOs->writeStr(encodeCl(currentClBg));
}

void resetClBg()
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

void popAttr(int n)
{
    while (stackAttr.size() && n-- > 0)
    {
        auto *pAttr = stackAttr.pop();

        switch (*pAttr)
        {
        case FontAttrib::Bold:          if (!attrFaint) pIOs->writeStr(ESC_NORMAL); break;
        case FontAttrib::Faint:         if (!--attrFaint) pIOs->writeStr(ESC_NORMAL); break;
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
