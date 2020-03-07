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

static Coord origin;
static const WindowCallbacks *pWndClbcks;

// -----------------------------------------------------------------------------

static void drawFrame(const Coord &coord, const Size &size, ColorBG clBg, const FrameStyle style)
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

static void drawWindow(const Widget *pWdgt)
{
    auto origin_bkp = origin;
    origin = {0, 0};
    pWndClbcks = pWdgt->window.pCallbacks;
    drawFrame(pWdgt->coord, pWdgt->size, pWdgt->window.bgColor, pWdgt->window.frameStyle);

    // title
    if (pWdgt->window.caption)
    {
        auto capt_len = strlen(pWdgt->window.caption);
        moveTo(pWdgt->coord.col + (pWdgt->size.width - capt_len - 4)/2,
            pWdgt->coord.row);
        writeStrFmt("╡ %s%s%s ╞", ESC_FG_YELLOW, pWdgt->window.caption, ESC_FG_DEFAULT);
        writeStr(ESC_BG_DEFAULT);
    }

    origin = pWdgt->coord;
    for (int i = 0; i < pWdgt->window.childrensCount; i++)
        drawWidget(&pWdgt->window.pChildrens[i]);

    origin = origin_bkp;
    pWndClbcks = nullptr;
    moveTo(0, pWdgt->coord.row + pWdgt->size.height);
}

static void drawPanel(const Widget *pWdgt)
{
    assert(pWndClbcks);
    if (!pWndClbcks->isEnabled(pWdgt)) writeStr(ESC_FAINT_ON);
    drawFrame(pWdgt->coord, pWdgt->size, pWdgt->panel.bgColor, pWdgt->panel.frameStyle);

    // title
    if (pWdgt->panel.caption)
    {
        auto capt_len = strlen(pWdgt->panel.caption);
        moveTo(origin.col + pWdgt->coord.col + (pWdgt->size.width - capt_len - 2)/2,
            origin.row + pWdgt->coord.row);

        assert(pWndClbcks);
        writeStrFmt(" %s%s%s ", ESC_FG_YELLOW, pWdgt->panel.caption, ESC_FG_DEFAULT);
        writeStr(ESC_BG_DEFAULT);
    }

    if (!pWndClbcks->isEnabled(pWdgt)) writeStr(ESC_FAINT_OFF);
}

void drawWidget(const Widget *pWdgt)
{
    switch (pWdgt->type)
    {
    case Widget::Window:    drawWindow(pWdgt); break;
    case Widget::Panel:     drawPanel(pWdgt); break;
    case Widget::Label:     break;
    case Widget::CheckBox:  break;
    case Widget::Button:    break;
    case Widget::PageCtrl:  break;
    default:                break;
    }
}

// -----------------------------------------------------------------------------

}
