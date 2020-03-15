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

const char * const frame_pgcontrol[] =
{
    "├", "─", "┐",
    "│", " ", "│",
    "├", "─", "┘",
};

const char * const frame_double[] =
{
    "╔", "═", "╗",
    "║", " ", "║",
    "╚", "═", "╝",
};

static Coord parentCoord;       // current widget left-top position
static IWindowState *pWndState; //
static String wgtStr;           // common string buff for widget renderers

// forward decl
static void drawWidgetInternal(const Widget *pWgt);

// -----------------------------------------------------------------------------

static void operator += (Coord &cord, const Coord &offs)
{
    cord.col += offs.col;
    cord.row += offs.row;
}

static Coord operator + (const Coord &cord1, const Coord &cord2)
{
    Coord ret = {
        uint8_t(cord1.col + cord2.col),
        uint8_t(cord1.row + cord2.row)
    };
    return ret;
}

// static Size operator + (const Size &sz1, const Size &sz2)
// {
//     Size ret = {
//         uint8_t(sz1.width + sz2.width),
//         uint8_t(sz1.height + sz2.height)
//     };
//     return ret;
// }

static Size operator - (const Size &sz1, const Size &sz2)
{
    Size ret = {
        uint8_t(sz1.width - sz2.width),
        uint8_t(sz1.height - sz2.height)
    };
    return ret;
}

static void drawFrame(const Coord &coord, const Size &size, ColorBG clBg, ColorFG clFg, const FrameStyle style)
{
    moveTo(coord.col, coord.row);

    const char * const * frame = frame_none;
    switch (style)
    {
    case FrameStyle::Single:    frame = frame_single; break;
    case FrameStyle::Double:    frame = frame_double; break;
    case FrameStyle::PgControl: frame = frame_pgcontrol; break;
    default: break;
    }

    // background and frame color
    if (clBg != ColorBG::None) pushClrBg(clBg);
    if (clFg != ColorFG::None) pushClrFg(clFg);

    // top line
    wgtStr.clear();
    wgtStr.append(frame[0]);
    wgtStr.append(frame[1], size.width - 2);
    wgtStr.append(frame[2]);
    writeStr(wgtStr.cstr());

    // lines in the middle
    wgtStr.clear();
    wgtStr.append(frame[3]);
    wgtStr.append(frame[4], size.width - 2);
    wgtStr.append(frame[5]);

    for (int r = coord.row + 1; r < coord.row + size.height - 1; r++)
    {
        moveBy(-size.width, 1);
        writeStr(wgtStr.cstr());
    }

    // bottom line
    wgtStr.clear();
    wgtStr.append(frame[6]);
    wgtStr.append(frame[7], size.width - 2);
    wgtStr.append(frame[8]);
    moveBy(-size.width, 1);
    writeStr(wgtStr.cstr());
}

// -----------------------------------------------------------------------------

static void drawWindow(const Widget *pWgt)
{
    // TODO: if this is Popup, send SAVE_SCREEN command and RESTORE_SCREEN on hide
    parentCoord = {0, 0};
    drawFrame(pWgt->coord, pWgt->size,
        pWgt->window.bgColor, pWgt->window.fgColor, FrameStyle::Double);

    // title
    if (pWgt->window.title)
    {
        auto capt_len = strlen(pWgt->window.title);
        moveTo(pWgt->coord.col + (pWgt->size.width - capt_len - 4)/2,
            pWgt->coord.row);
        pushClrFg(ColorFG::Yellow);
        pushAttr(FontAttrib::Bold);
        writeStrFmt("╡ %s ╞", pWgt->window.title);
        popAttr();
        popClrFg();
    }

    parentCoord = pWgt->coord;
    for (int i = 0; i < pWgt->window.childCount; i++)
    {
        if (pWgt->window.pChildrens[i].type == Widget::Type::None)
            break;
        drawWidgetInternal(&pWgt->window.pChildrens[i]);
    }

    // reset colors set by frame drawer
    popClrBg();
    popClrFg();
    moveTo(0, pWgt->coord.row + pWgt->size.height);
}

static void drawPanel(const Widget *pWgt)
{
    const auto my_coord = parentCoord + pWgt->coord;

    drawFrame(my_coord, pWgt->size,
        pWgt->panel.bgColor, pWgt->panel.fgColor, FrameStyle::Single);

    // title
    if (pWgt->panel.title)
    {
        auto capt_len = strlen(pWgt->panel.title);
        moveTo(my_coord.col + (pWgt->size.width - capt_len - 2)/2, my_coord.row);
        pushAttr(FontAttrib::Bold);
        writeStrFmt(" %s ", pWgt->panel.title);
        popAttr();
    }

    auto coord_bkp = parentCoord;
    parentCoord = my_coord;
    for (int i = 0; i < pWgt->panel.childCount; i++)
    {
        if (pWgt->panel.pChildrens[i].type == Widget::Type::None)
            break;
        drawWidgetInternal(&pWgt->panel.pChildrens[i]);
    }
    parentCoord = coord_bkp;

    // reset colors set by frame drawer
    popClrBg();
    popClrFg();
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
    pushClrBg(pWgt->label.bgColor);
    pushClrFg(pWgt->label.fgColor);
    // print label
    writeStr(wgtStr.cstr());
    // fill remaining space with spaces;
    // count UTF-8 sequences, not bytes
    int n = utf8len(wgtStr.cstr());
    writeChar(' ', max_len - n);
    // restore colors
    popClrFg();
    popClrBg();
}

static void drawLed(const Widget *pWgt)
{
    auto clbg = pWndState->getLedLit(pWgt) ? pWgt->led.bgColorOn : pWgt->led.bgColorOff;
    // led text
    moveTo(parentCoord.col + pWgt->coord.col, parentCoord.row + pWgt->coord.row);
    pushClrBg(clbg);
    pushClrFg(pWgt->led.fgColor);
    writeStr(pWgt->led.text);
    popClrFg();
    popClrBg();
}

static void drawCheckbox(const Widget *pWgt)
{
    const char *s_chk_state = pWndState->getCheckboxChecked(pWgt) ? "[x] " : "[ ] ";
    bool focused = pWndState->isFocused(pWgt);

    moveTo(parentCoord.col + pWgt->coord.col, parentCoord.row + pWgt->coord.row);
    if (focused) pushAttr(FontAttrib::Inverse);
    writeStr(s_chk_state);
    writeStr(pWgt->checkbox.text);
    if (focused) popAttr();
}

static void drawButton(const Widget *pWgt)
{
    wgtStr.clear();
    wgtStr.append("[ ");
    wgtStr.append(pWgt->button.text);
    wgtStr.trim(pWgt->size.width-2);
    wgtStr.append(" ]");

    bool focused = pWndState->isFocused(pWgt);

    moveTo(parentCoord.col + pWgt->coord.col, parentCoord.row + pWgt->coord.row);
    if (focused) pushAttr(FontAttrib::Inverse);
    writeStr(wgtStr.cstr());
    if (focused) popAttr();
}

static void drawPageControl(const Widget *pWgt)
{
    const auto my_coord = parentCoord + pWgt->coord;

    drawFrame(my_coord + Coord{pWgt->pagectrl.tabWidth, 0}, pWgt->size - Size{pWgt->pagectrl.tabWidth, 0},
        ColorBG::None, ColorFG::None, FrameStyle::PgControl);

    auto coord_bkp = parentCoord;
    parentCoord = my_coord;
    // trad title
    wgtStr.clear();
    wgtStr.append(" ≡ MENU ≡ ");
    wgtStr.append(' ', pWgt->pagectrl.tabWidth - wgtStr.utf8Len());
    moveTo(my_coord.col, my_coord.row);
    pushAttr(FontAttrib::Inverse);
    writeStr(wgtStr.cstr());
    popAttr();

    // draw childrens and left/right borders
    int pg_idx = pWndState->getPageCtrlPageIndex(pWgt);
    bool focused = pWndState->isFocused(pWgt);

    moveTo(parentCoord.col + pWgt->coord.col, parentCoord.row + pWgt->coord.row);

    for (int i = 0; i < pWgt->pagectrl.childCount; i++)
    {
        if (i == pWgt->size.height-1)
            break;

        const auto *p_page = &pWgt->pagectrl.pChildrens[i];
        if (p_page->type == Widget::Type::None)
            break;

        // draw page title
        wgtStr.clear();
        wgtStr.appendFmt("%s%s", i == pg_idx ? "►" : " ", p_page->page.title);
        wgtStr.append(' ', pWgt->pagectrl.tabWidth - utf8len(p_page->page.title));
        wgtStr.trim(pWgt->pagectrl.tabWidth, true);

        moveTo(my_coord.col, my_coord.row + i + 1);
        pushClrFg(p_page->page.fgColor);
        if (focused && i == pg_idx) pushAttr(FontAttrib::Inverse);
        writeStr(wgtStr.cstr());
        if (focused && i == pg_idx) popAttr();
        popClrFg();

        parentCoord.col += pWgt->pagectrl.tabWidth;
        drawWidgetInternal(p_page);
        parentCoord.col -= pWgt->pagectrl.tabWidth;
    }

    parentCoord = coord_bkp;
}

static void drawPage(const Widget *pWgt)
{
    // draw childrens
    for (int i = 0; i < pWgt->page.childCount; i++)
    {
        if (pWgt->page.pChildrens[i].type == Widget::Type::None)
            break;
        drawWidgetInternal(&pWgt->page.pChildrens[i]);
    }
}

static void drawProgressBar(const Widget *pWgt)
{
    int pos = 0, max = 1;
    pWndState->getProgressBarNfo(pWgt, pos, max);

    if (max <= 0) max = 1;
    if (pos > max) pos = max;

    moveTo(parentCoord.col + pWgt->coord.col, parentCoord.row + pWgt->coord.row);
    wgtStr.clear();
    int fill = pos * pWgt->size.width / max;
    wgtStr.append("█", fill);
    wgtStr.append("▒", pWgt->size.width - fill);
    writeStr(wgtStr.cstr());

    // writeStr("░░▒▒▓▓██");
    // writeStr("████░░░░░░░░░░░░");
}

// -----------------------------------------------------------------------------

static void drawWidgetInternal(const Widget *pWgt)
{
    bool en = pWndState->isEnabled(pWgt);
    if (!en) pushAttr(FontAttrib::Faint);

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
    case Widget::ProgressBar:   drawProgressBar(pWgt); break;
    default:                    break;
    }

    if (!en) popAttr();
}

template<typename T>
const Widget* iterateParent(const T *pParentDetails, const Widget *pParent, const WID widgetId, Coord &widgetCord, const Widget **ppParent)
{
    const auto *p = findWidget(pParentDetails->pChildrens, pParentDetails->childCount, widgetId, widgetCord, ppParent);
    if (p)
    {
        *ppParent = pParent;
        widgetCord += pParent->coord;

        // pages coords == pagecontrol coords + tabs width
        if (pParent->type == Widget::Type::PageCtrl)
            widgetCord.col += pParent->pagectrl.tabWidth;
    }
    return p;
}

// recursive function searching for widgetId
static const Widget *findWidget(const Widget widgets[], const uint16_t widgetsCount,
                                WID widgetId, Coord &widgetCord, const Widget **ppParent)
{
    const auto *p_wgt = widgets;

    for (uint16_t w = 0; w < widgetsCount; w++)
    {
        if (p_wgt->id == widgetId)
        {
            return p_wgt;
        }

        if (p_wgt->type == Widget::Window)
        {
            if (const auto *p = iterateParent(&p_wgt->window, p_wgt, widgetId, widgetCord, ppParent))
                return p;
        }

        if (p_wgt->type == Widget::Panel)
        {
            if (const auto *p = iterateParent(&p_wgt->panel, p_wgt, widgetId, widgetCord, ppParent))
                return p;
        }

        if (p_wgt->type == Widget::PageCtrl)
        {
            if (const auto *p = iterateParent(&p_wgt->pagectrl, p_wgt, widgetId, widgetCord, ppParent))
                return p;
        }

        if (p_wgt->type == Widget::Page)
        {
            if (const auto *p = iterateParent(&p_wgt->page, p_wgt, widgetId, widgetCord, ppParent))
                return p;
        }

        p_wgt++;
    }

    return nullptr;
}

static ColorBG getWidgetBgColor(const Widget *pWgt)
{
    if (pWgt)
    {
        switch (pWgt->type)
        {
        case Widget::Window:    return pWgt->window.bgColor; break;
        case Widget::Panel:     return pWgt->panel.bgColor; break;
        default: break;
        }
    }

    return ColorBG::None;
}

// -----------------------------------------------------------------------------

void drawWidget(const Widget *pWindow, WID widgetId)
{
    assert(pWindow);
    assert(pWindow->type == Widget::Window);
    pWndState = pWindow->window.getState();
    assert(pWndState);

    if (widgetId == WIDGET_ID_ALL)
    {
        drawWidgetInternal(pWindow);
    }
    else
    {
        parentCoord = {0, 0};
        const Widget *p_parent = pWindow;

        // search window for widget by it's Id
        if (const auto *p_wgt = findWidget(pWindow, 1, widgetId, parentCoord, &p_parent))
        {
            // set parent's background color
            pushClrBg(getWidgetBgColor(p_parent));
            drawWidgetInternal(p_wgt);
            popClrBg();
        }
    }

    pWndState = nullptr;
    resetAttr();
    resetClrBg();
    resetClrFg();
}

void drawWidgets(const Widget *pWindow, const WID *pWidgetIds, uint16_t count)
{
    assert(pWindow);
    assert(pWindow->type == Widget::Window);
    pWndState = pWindow->window.getState();
    assert(pWndState);

    for (unsigned i = 0; i < count; i++)
    {
        parentCoord = {0, 0};
        const Widget *p_parent = pWindow;

        // search window for widget by it's Id
        if (const auto *p_wgt = findWidget(pWindow, 1, pWidgetIds[i], parentCoord, &p_parent))
        {
            // set parent's background color
            pushClrBg(getWidgetBgColor(p_parent));
            drawWidgetInternal(p_wgt);
            popClrBg();
        }
    }

    pWndState = nullptr;
    resetAttr();
    resetClrBg();
    resetClrFg();
}

// -----------------------------------------------------------------------------

}
