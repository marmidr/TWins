/******************************************************************************
 * @brief   TWins - widget drawing
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#include "twins.hpp"
#include "twins_string.hpp"
#include "twins_utf8str.hpp"

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

extern IOs *pIOs;               //
static Coord parentCoord;       // current widget left-top position
static IWindowState *pWndState; //
static String wgtStr;           // common string buff for widget renderers

// forward decl
static void drawWidgetInternal(const Widget *pWgt);

// -----------------------------------------------------------------------------

static void coordOffsetBy(Coord &cord, const Coord &offs)
{
    cord.col += offs.col;
    cord.row += offs.row;
}

static void drawFrame(const Coord &coord, const Size &size, ColorBG clBg, ColorFG clFg, const FrameStyle style)
{
    moveTo(parentCoord.col + coord.col, parentCoord.row + coord.row);

    const char * const * frame = frame_none;
    switch (style)
    {
    case FrameStyle::Single: frame = frame_single; break;
    case FrameStyle::Double: frame = frame_double; break;
    default: break;
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
    pWndState = pWgt->window.getState();
    assert(pWndState);

    auto origin_bkp = parentCoord;
    parentCoord = {0, 0};
    drawFrame(pWgt->coord, pWgt->size, pWgt->window.bgColor, pWgt->window.fgColor, twins::FrameStyle::Double);

    // title
    if (pWgt->window.caption)
    {
        auto capt_len = strlen(pWgt->window.caption);
        moveTo(pWgt->coord.col + (pWgt->size.width - capt_len - 4)/2,
            pWgt->coord.row);
        writeStrFmt("╡ %s%s%s ╞", ESC_FG_YELLOW ESC_BOLD_ON, pWgt->window.caption, ESC_FG_DEFAULT ESC_BOLD_OFF);
        writeStr(ESC_BG_DEFAULT);
    }

    parentCoord = pWgt->coord;
    for (int i = 0; i < pWgt->window.childCount; i++)
    {
        if (pWgt->window.pChildrens[i].type == Widget::Type::None)
            break;
        drawWidgetInternal(&pWgt->window.pChildrens[i]);
    }
    parentCoord = origin_bkp;

    pWndState = nullptr;
    moveTo(0, pWgt->coord.row + pWgt->size.height);
    // TODO: resetClBg(), resetClFg()
    writeStr(ESC_BG_DEFAULT ESC_FG_DEFAULT);
}

static void drawPanel(const Widget *pWgt)
{
    if (!pWndState->isEnabled(pWgt)) writeStr(ESC_FAINT_ON);
    drawFrame(pWgt->coord, pWgt->size, pWgt->panel.bgColor, pWgt->panel.fgColor, twins::FrameStyle::Single);

    // title
    if (pWgt->panel.caption)
    {
        auto capt_len = strlen(pWgt->panel.caption);
        moveTo(parentCoord.col + pWgt->coord.col + (pWgt->size.width - capt_len - 2)/2,
            parentCoord.row + pWgt->coord.row);

        writeStrFmt(" %s%s%s ", ESC_FG_WHITE_INTENSE ESC_BOLD_ON, pWgt->panel.caption, ESC_BOLD_OFF);
        writeStr(ESC_BG_DEFAULT);
    }

    auto origin_bkp = parentCoord;
    parentCoord.col += pWgt->coord.col;
    parentCoord.row += pWgt->coord.row;
    for (int i = 0; i < pWgt->panel.childCount; i++)
    {
        if (pWgt->panel.pChildrens[i].type == Widget::Type::None)
            break;
        drawWidgetInternal(&pWgt->panel.pChildrens[i]);
    }
    parentCoord = origin_bkp;

    //TODO: attributesReset()
    if (!pWndState->isEnabled(pWgt)) writeStr(ESC_FAINT_OFF);
}

static void drawLabel(const Widget *pWgt)
{
    // label text
    if (pWgt->label.text)
        wgtStr = pWgt->label.text;
    else
        pWndState->getLabelText(pWgt, wgtStr);

    auto max_len = pWgt->size.width;
    wgtStr.trim(max_len, true);

    moveTo(parentCoord.col + pWgt->coord.col, parentCoord.row + pWgt->coord.row);
    // setup colors
    writeStr(clrFg(pWgt->label.fgColor));
    writeStr(clrBg(pWgt->label.bgColor));
    // print label
    writeStr(wgtStr.cstr());
    // fill remaining space with spaces;
    // count UTF-8 sequences, not bytes
    int n = utf8len(wgtStr.cstr());
    writeChar(' ', max_len - n);
}

static void drawLed(const Widget *pWgt)
{
    // led text
    moveTo(parentCoord.col + pWgt->coord.col, parentCoord.row + pWgt->coord.row);
    auto clbg = pWndState->getLedLit(pWgt) ? pWgt->led.bgColorOn : pWgt->led.bgColorOff;
    // setup colors
    writeStr(clrFg(pWgt->led.fgColor));
    writeStr(clrBg(clbg));
    writeStr(pWgt->led.text);
}

static void drawCheckbox(const Widget *pWgt)
{
    const char *s_chk_state = pWndState->getCheckboxChecked(pWgt) ? "[x] " : "[ ] ";
    moveTo(parentCoord.col + pWgt->coord.col, parentCoord.row + pWgt->coord.row);

    // setup colors
    if (pWndState->isFocused(pWgt))
        writeStr(ESC_INVERSE_ON);
    writeStr(s_chk_state);
    writeStr(pWgt->checkbox.text);
    writeStr(ESC_INVERSE_OFF);
}

static void drawButton(const Widget *pWgt)
{
    moveTo(parentCoord.col + pWgt->coord.col, parentCoord.row + pWgt->coord.row);
    wgtStr.clear();
    wgtStr.append("[ ");
    wgtStr.append(pWgt->button.text);
    wgtStr.trim(pWgt->size.width-2);
    wgtStr.append(" ]");
    writeStr(wgtStr.cstr());
}

static void drawPageControl(const Widget *pWgt)
{
    moveTo(parentCoord.col + pWgt->coord.col, parentCoord.row + pWgt->coord.row);
}

static void drawPage(const Widget *pWgt)
{
    moveTo(parentCoord.col + pWgt->coord.col, parentCoord.row + pWgt->coord.row);
}

static void drawProgressBarControl(const Widget *pWgt)
{
    int pos = 0, max = 1;
    pWndState->getProgressBarNfo(pWgt, pos, max);

    if (max <= 0) max = 1;
    if (pos > max) pos = max;

    moveTo(parentCoord.col + pWgt->coord.col, parentCoord.row + pWgt->coord.row);
    wgtStr.clear();
    int fill = pos * pWgt->size.width / max;
    wgtStr.append("█", fill);
    wgtStr.append("░", pWgt->size.width - fill);
    writeStr(wgtStr.cstr());

    // writeStr("░░▒▒▓▓██");
    // writeStr("████░░░░░░░░░░░░");
}

static void drawWidgetInternal(const Widget *pWgt)
{
    switch (pWgt->type)
    {
    case Widget::Window:        drawWindow(pWgt); break;
    case Widget::Panel:         drawPanel(pWgt); break;
    case Widget::Label:         drawLabel(pWgt); break;
    case Widget::CheckBox:      drawCheckbox(pWgt); break;
    case Widget::Button:        drawButton(pWgt); break;
    case Widget::Led:           drawLed(pWgt); break;
    case Widget::PageCtrl:      drawPageControl(pWgt); break;
    case Widget::Page:          drawPage(pWgt); break;
    case Widget::ProgressBar:   drawProgressBarControl(pWgt); break;
    default:                    break;
    }

    writeStr(ESC_BG_DEFAULT ESC_FG_DEFAULT);
}

// recursive function searching for widgetId
static const Widget *findWidget(const Widget widgets[], const uint16_t widgetsCount, uint16_t widgetId, Coord &widgetCord)
{
    const auto *p_wgt = widgets;

    for (uint16_t w = 0; w < widgetsCount; w++)
    {
        if (p_wgt->id == widgetId)
        {
            // coordOffsetBy(widgetScreenCord, p_wgt);
            return p_wgt;
        }

        if (p_wgt->type == Widget::Window)
        {
            const auto *p = findWidget(p_wgt->window.pChildrens, p_wgt->window.childCount, widgetId, widgetCord);
            if (p)
            {
                coordOffsetBy(widgetCord, p_wgt->coord);
                return p;
            }
        }

        if (p_wgt->type == Widget::Panel)
        {
            const auto *p = findWidget(p_wgt->panel.pChildrens, p_wgt->panel.childCount, widgetId, widgetCord);
            if (p)
            {
                coordOffsetBy(widgetCord, p_wgt->coord);
                return p;
            }
        }

        p_wgt++;
    }

    return nullptr;
}

// -----------------------------------------------------------------------------

void drawWidget(const Widget *pWindow, uint16_t widgetId)
{
    assert(pWindow);
    assert(pWindow->type == Widget::Window);

    if (widgetId == WIDGET_ID_ALL)
    {
        drawWidgetInternal(pWindow);
    }
    else
    {
        pWndState = pWindow->window.getState();
        assert(pWndState);
        parentCoord = {0, 0};

        // search window for widget by it's Id
        if (const auto *p_wgt = findWidget(pWindow, 1, widgetId, parentCoord))
            drawWidgetInternal(p_wgt);

        pWndState = nullptr;
    }
}

// -----------------------------------------------------------------------------

}
