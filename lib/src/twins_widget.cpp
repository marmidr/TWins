/******************************************************************************
 * @brief   TWins - widget drawing
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#include "twins.hpp"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

// -----------------------------------------------------------------------------

namespace twins
{

// -----------------------------------------------------------------------------

const char * const frame_none[] =
{
    " ", " ", " ",
    " ", " ", " ",
    " ", " ", " ",
};

const char * const frame_single[] =
{
    "┌", "─", "┐",
    "│", " ", "│",
    "└", "─", "┘",
};

const char * const frame_double[] =
{
    "╔", "═", "╗",
    "║", " ", "║",
    "╚", "═", "╝",
};

extern IOs *pIOs;
struct TxtBuff
{
    TxtBuff(uint32_t sz) { buff = (char*)pIOs->malloc(sz+1); }
    ~TxtBuff()           { pIOs->mfree(buff); }
    char *buff;
};

static Coord origin;
static const WindowCallbacks *pWndClbcks;

// -----------------------------------------------------------------------------

static void drawFrame(const Coord &coord, const Size &size, ColorBG clBg, ColorFG clFg, const FrameStyle style)
{
    moveTo(origin.col + coord.col, origin.row + coord.row);

    const char * const * frame = frame_none;
    switch (style)
    {
    case FrameStyle::Single: frame = frame_single; break;
    case FrameStyle::Double: frame = frame_double; break;
    }

    // background and frame
    writeStr(clrBg(clBg));
    writeStr(clrFg(clFg));

    writeStr(frame[0]);
    for (int c = coord.col + 1; c < coord.col + size.width - 1; c++)
        writeStr(frame[1]);
    writeStr(frame[2]);

    moveBy(-size.width, 1);

    for (int r = coord.row + 1; r < coord.row + size.height - 1; r++)
    {
        writeStr(frame[3]);
        for (int c = coord.col + 1; c < coord.col + size.width - 1; c++)
            writeStr(frame[4]);
        writeStr(frame[5]);
        moveBy(-size.width, 1);
    }

    writeStr(frame[6]);
    for (int c = coord.col + 1; c < coord.col + size.width - 1; c++)
        writeStr(frame[7]);
    writeStr(frame[8]);
}

static void drawWindow(const Widget *pWgt)
{
    auto origin_bkp = origin;
    origin = {0, 0};
    pWndClbcks = pWgt->window.pCallbacks;
    assert(pWndClbcks);
    drawFrame(pWgt->coord, pWgt->size, pWgt->window.bgColor, pWgt->window.fgColor, pWgt->window.frameStyle);

    // title
    if (pWgt->window.caption)
    {
        auto capt_len = strlen(pWgt->window.caption);
        moveTo(pWgt->coord.col + (pWgt->size.width - capt_len - 4)/2,
            pWgt->coord.row);
        writeStrFmt("╡ %s%s%s ╞", ESC_FG_YELLOW, pWgt->window.caption, ESC_FG_DEFAULT);
        writeStr(ESC_BG_DEFAULT);
    }

    origin = pWgt->coord;
    for (int i = 0; i < pWgt->window.childrensCount; i++)
    {
        if (pWgt->window.pChildrens[i].type == Widget::Type::None)
            break;
        drawWidget(&pWgt->window.pChildrens[i]);
    }
    origin = origin_bkp;

    pWndClbcks = nullptr;
    moveTo(0, pWgt->coord.row + pWgt->size.height);
    writeStr(ESC_BG_DEFAULT ESC_FG_DEFAULT);
}

static void drawPanel(const Widget *pWgt)
{
    assert(pWndClbcks);
    if (!pWndClbcks->isEnabled(pWgt)) writeStr(ESC_FAINT_ON);
    drawFrame(pWgt->coord, pWgt->size, pWgt->panel.bgColor, pWgt->panel.fgColor, pWgt->panel.frameStyle);

    // title
    if (pWgt->panel.caption)
    {
        auto capt_len = strlen(pWgt->panel.caption);
        moveTo(origin.col + pWgt->coord.col + (pWgt->size.width - capt_len - 2)/2,
            origin.row + pWgt->coord.row);

        writeStrFmt(" %s%s%s ", ESC_FG_WHITE_INTENSE ESC_BOLD_ON, pWgt->panel.caption, ESC_BOLD_OFF);
        writeStr(ESC_BG_DEFAULT);
    }

    auto origin_bkp = origin;
    origin.col += pWgt->coord.col;
    origin.row += pWgt->coord.row;
    for (int i = 0; i < pWgt->panel.childrensCount; i++)
    {
        if (pWgt->panel.pChildrens[i].type == Widget::Type::None)
            break;
        drawWidget(&pWgt->panel.pChildrens[i]);
    }
    origin = origin_bkp;

    if (!pWndClbcks->isEnabled(pWgt)) writeStr(ESC_FAINT_OFF);
}

static void drawLabel(const Widget *pWgt)
{
    assert(pWndClbcks);

    // label text
    if (pWgt->label.text)
    {
        auto text_len = strlen(pWgt->label.text);
        moveTo(origin.col + pWgt->coord.col, origin.row + pWgt->coord.row);

        // limit the text len to Widget width
        auto max_len = pWgt->size.width;
        char fmt[6];
        snprintf(fmt, sizeof(fmt), "%%" ".%u" "s", max_len);
        // setup colors
        writeStrFmt("%s%s", clrFg(pWgt->label.fgColor), clrBg(pWgt->label.bgColor));
        // print label
        int n = writeStrFmt(fmt, pWgt->label.text);
        // fill remaining space with spaces
        writeChar(' ', max_len - n);
    }
}

void drawWidget(const Widget *pWgt)
{
    switch (pWgt->type)
    {
    case Widget::Window:    drawWindow(pWgt); break;
    case Widget::Panel:     drawPanel(pWgt); break;
    case Widget::Label:     drawLabel(pWgt); break;
    case Widget::CheckBox:  break;
    case Widget::Button:    break;
    case Widget::PageCtrl:  break;
    default:                break;
    }
}

// -----------------------------------------------------------------------------

}
