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

/** @brief Pointer to PAL used internally by TWins */
IPal *pPAL;

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

void init(IPal *pal)
{
    pPAL = pal;
}

int writeChar(char c, int16_t repeat)
{
    return pPAL ? pPAL->writeChar(c, repeat) : 0;
}

int writeStr(const char *s, int16_t repeat)
{
    return pPAL ? pPAL->writeStr(s, repeat) : 0;
}

int writeStrFmt(const char *fmt, ...)
{
    if (!(fmt && pPAL))
        return 0;

    va_list ap;
    va_start(ap, fmt);
    int n = pPAL->writeStrVFmt(fmt, ap);
    va_end(ap);
    return n;
}

int writeStrVFmt(const char *fmt, va_list ap)
{
    return pPAL ? pPAL->writeStrVFmt(fmt, ap) : 0;
}

void flushBuffer()
{
    if (pPAL)
        pPAL->flushBuff();
}

// -----------------------------------------------------------------------------

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
    writeStr(encodeCl(currentClFg));
}

void popClFg(int n)
{
    while (stackClFg.size() && n-- > 0)
        currentClFg = *stackClFg.pop();

    writeStr(encodeCl(currentClFg));
}

void resetClFg()
{
    stackClFg.clear();
    writeStr(ESC_FG_DEFAULT);
}

// -----------------------------------------------------------------------------

void pushClBg(ColorBG cl)
{
    stackClBg.push(currentClBg);
    currentClBg = cl;
    writeStr(encodeCl(currentClBg));
}

void popClBg(int n)
{
    while (stackClBg.size() && n-- > 0)
        currentClBg = *stackClBg.pop();

    writeStr(encodeCl(currentClBg));
}

void resetClBg()
{
    stackClBg.clear();
    writeStr(ESC_BG_DEFAULT);
}

// -----------------------------------------------------------------------------

static int8_t attrFaint = 0;

void pushAttr(FontAttrib attr)
{
    stackAttr.push(attr);

    switch (attr)
    {
    case FontAttrib::Bold:          if (!attrFaint) writeStr(ESC_BOLD); break;
    case FontAttrib::Faint:         attrFaint++;    writeStr(ESC_FAINT); break;
    case FontAttrib::Italics:       writeStr(ESC_ITALICS_ON); break;
    case FontAttrib::Underline:     writeStr(ESC_UNDERLINE_ON); break;
    case FontAttrib::Blink:         writeStr(ESC_BLINK); break;
    case FontAttrib::Inverse:       writeStr(ESC_INVERSE_ON); break;
    case FontAttrib::Invisible:     writeStr(ESC_INVISIBLE_ON); break;
    case FontAttrib::StrikeThrough: writeStr(ESC_STRIKETHROUGH_ON); break;
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
        case FontAttrib::Bold:          if (!attrFaint)   writeStr(ESC_NORMAL); break;
        case FontAttrib::Faint:         if (!--attrFaint) writeStr(ESC_NORMAL); break;
        case FontAttrib::Italics:       writeStr(ESC_ITALICS_OFF); break;
        case FontAttrib::Underline:     writeStr(ESC_UNDERLINE_OFF); break;
        case FontAttrib::Blink:         writeStr(ESC_BLINK_OFF); break;
        case FontAttrib::Inverse:       writeStr(ESC_INVERSE_OFF); break;
        case FontAttrib::Invisible:     writeStr(ESC_INVISIBLE_OFF); break;
        case FontAttrib::StrikeThrough: writeStr(ESC_STRIKETHROUGH_OFF); break;
        default: break;
        }
    }
}

void resetAttr()
{
    attrFaint = 0;
    stackAttr.clear();
    writeStr(ESC_ATTRIBUTES_DEFAULT);
}

// -----------------------------------------------------------------------------

}
