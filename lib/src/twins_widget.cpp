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

#include <stdarg.h>
#include <time.h>

// -----------------------------------------------------------------------------

namespace twins
{

/** Global state object */
static struct
{
    Coord   parentCoord;        // current widget left-top position
    String  str;                // common string buff for widget renderers
    const Widget *pWnd = {};    //
    IWindowState *pWndState = {};
} g;

// -----------------------------------------------------------------------------

void log(const char *file, const char *func, unsigned line, const char *fmt, ...)
{
    uint16_t row = pIOs->getLogsRow();
    moveTo(1, row);
    writeStr("\033[1L"); // insert line

    // display only file name, trim the path
    if (const char *delim = strrchr(file, '/'))
        file = delim + 1;

    time_t t = time(NULL);
    struct tm *p_stm = localtime(&t);
    writeStrFmt("[%2d:%2d:%02d] %s() %s:%u: ",
        p_stm->tm_hour, p_stm->tm_min, p_stm->tm_sec,
        func, file, line);

    va_list ap;
    va_start(ap, fmt);
    pIOs->writeStrFmt(fmt, ap);
    pIOs->flush();
    va_end(ap);
}

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


// forward decl
static void drawWidgetInternal(const Widget *pWgt);

// -----------------------------------------------------------------------------

const char * toString(Widget::Type type)
{
    #define CASE_WGT_STR(t)     case Widget::t: return #t;

    switch (type)
    {
    CASE_WGT_STR(None)
    CASE_WGT_STR(Window)
    CASE_WGT_STR(Panel)
    CASE_WGT_STR(Label)
    CASE_WGT_STR(Edit)
    CASE_WGT_STR(CheckBox)
    CASE_WGT_STR(Button)
    CASE_WGT_STR(Led)
    CASE_WGT_STR(PageCtrl)
    CASE_WGT_STR(Page)
    CASE_WGT_STR(ProgressBar)
    default: return "?";
    }
}

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

// -----------------------------------------------------------------------------

static void drawArea(const Coord &coord, const Size &size, ColorBG clBg, ColorFG clFg, const FrameStyle style)
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
    g.str.clear();
    g.str.append(frame[0]);
    g.str.append(frame[1], size.width - 2);
    g.str.append(frame[2]);
    writeStr(g.str.cstr());

    // lines in the middle
    g.str.clear();
    g.str.append(frame[3]);
    g.str.append(frame[4], size.width - 2);
    g.str.append(frame[5]);

    for (int r = coord.row + 1; r < coord.row + size.height - 1; r++)
    {
        moveBy(-size.width, 1);
        writeStr(g.str.cstr());
    }

    // bottom line
    g.str.clear();
    g.str.append(frame[6]);
    g.str.append(frame[7], size.width - 2);
    g.str.append(frame[8]);
    moveBy(-size.width, 1);
    writeStr(g.str.cstr());
}

// -----------------------------------------------------------------------------

static void drawWindow(const Widget *pWgt)
{
    // TODO: if this is Popup, send SAVE_SCREEN command and RESTORE_SCREEN on hide
    g.parentCoord = {0, 0};
    drawArea(pWgt->coord, pWgt->size,
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

    g.parentCoord = pWgt->coord;
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
    const auto my_coord = g.parentCoord + pWgt->coord;

    drawArea(my_coord, pWgt->size,
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

    auto coord_bkp = g.parentCoord;
    g.parentCoord = my_coord;
    for (int i = 0; i < pWgt->panel.childCount; i++)
    {
        if (pWgt->panel.pChildrens[i].type == Widget::Type::None)
            break;
        drawWidgetInternal(&pWgt->panel.pChildrens[i]);
    }
    g.parentCoord = coord_bkp;

    // reset colors set by frame drawer
    popClrBg();
    popClrFg();
}

static void drawLabel(const Widget *pWgt)
{
    // label text
    if (pWgt->label.text)
        g.str = pWgt->label.text;
    else
        g.pWndState->getLabelText(pWgt, g.str);

    auto max_len = pWgt->size.width;
    g.str.trim(max_len, true);

    moveTo(g.parentCoord.col + pWgt->coord.col, g.parentCoord.row + pWgt->coord.row);
    // setup colors
    pushClrBg(pWgt->label.bgColor);
    pushClrFg(pWgt->label.fgColor);
    // print label
    writeStr(g.str.cstr());
    // fill remaining space with spaces;
    // count UTF-8 sequences, not bytes
    int n = utf8len(g.str.cstr());
    writeChar(' ', max_len - n);
    // restore colors
    popClrFg();
    popClrBg();
}

static void drawLed(const Widget *pWgt)
{
    auto clbg = g.pWndState->getLedLit(pWgt) ? pWgt->led.bgColorOn : pWgt->led.bgColorOff;
    // led text
    moveTo(g.parentCoord.col + pWgt->coord.col, g.parentCoord.row + pWgt->coord.row);
    pushClrBg(clbg);
    pushClrFg(pWgt->led.fgColor);
    writeStr(pWgt->led.text);
    popClrFg();
    popClrBg();
}

static void drawCheckbox(const Widget *pWgt)
{
    const char *s_chk_state = g.pWndState->getCheckboxChecked(pWgt) ? "[x] " : "[ ] ";
    bool focused = g.pWndState->isFocused(pWgt);

    moveTo(g.parentCoord.col + pWgt->coord.col, g.parentCoord.row + pWgt->coord.row);
    if (focused) pushAttr(FontAttrib::Inverse);
    writeStr(s_chk_state);
    writeStr(pWgt->checkbox.text);
    if (focused) popAttr();
}

static void drawButton(const Widget *pWgt)
{
    g.str.clear();
    g.str.append("[ ");
    g.str.append(pWgt->button.text);
    g.str.trim(pWgt->size.width-2);
    g.str.append(" ]");

    bool focused = g.pWndState->isFocused(pWgt);

    moveTo(g.parentCoord.col + pWgt->coord.col, g.parentCoord.row + pWgt->coord.row);
    if (focused) pushAttr(FontAttrib::Inverse);
    writeStr(g.str.cstr());
    if (focused) popAttr();
}

static void drawPageControl(const Widget *pWgt)
{
    const auto my_coord = g.parentCoord + pWgt->coord;

    drawArea(my_coord + Coord{pWgt->pagectrl.tabWidth, 0}, pWgt->size - Size{pWgt->pagectrl.tabWidth, 0},
        ColorBG::None, ColorFG::None, FrameStyle::PgControl);

    auto coord_bkp = g.parentCoord;
    g.parentCoord = my_coord;
    // trad title
    g.str.clear();
    g.str.append(" ≡ MENU ≡ ");
    g.str.append(' ', pWgt->pagectrl.tabWidth - g.str.utf8Len());
    moveTo(my_coord.col, my_coord.row);
    pushAttr(FontAttrib::Inverse);
    writeStr(g.str.cstr());
    popAttr();

    // draw childrens and left/right borders
    int pg_idx = g.pWndState->getPageCtrlPageIndex(pWgt);
    moveTo(g.parentCoord.col + pWgt->coord.col, g.parentCoord.row + pWgt->coord.row);

    for (int i = 0; i < pWgt->pagectrl.childCount; i++)
    {
        if (i == pWgt->size.height-1)
            break;

        const auto *p_page = &pWgt->pagectrl.pChildrens[i];
        if (p_page->type == Widget::Type::None)
            break;

        // draw page title
        g.str.clear();
        g.str.appendFmt("%s%s", i == pg_idx ? "►" : " ", p_page->page.title);
        g.str.append(' ', pWgt->pagectrl.tabWidth - utf8len(p_page->page.title));
        g.str.trim(pWgt->pagectrl.tabWidth, true);

        moveTo(my_coord.col, my_coord.row + i + 1);
        pushClrFg(p_page->page.fgColor);
        if (i == pg_idx) pushAttr(FontAttrib::Inverse);
        writeStr(g.str.cstr());
        if (i == pg_idx) popAttr();
        popClrFg();

        if (g.pWndState->isVisible(p_page))
        {
            g.parentCoord.col += pWgt->pagectrl.tabWidth;
            drawWidgetInternal(p_page);
            g.parentCoord.col -= pWgt->pagectrl.tabWidth;
        }
    }

    g.parentCoord = coord_bkp;
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
    g.pWndState->getProgressBarNfo(pWgt, pos, max);

    if (max <= 0) max = 1;
    if (pos > max) pos = max;

    moveTo(g.parentCoord.col + pWgt->coord.col, g.parentCoord.row + pWgt->coord.row);
    g.str.clear();
    int fill = pos * pWgt->size.width / max;
    g.str.append("█", fill);
    g.str.append("▒", pWgt->size.width - fill);
    writeStr(g.str.cstr());

    // writeStr("░░▒▒▓▓██");
    // writeStr("████░░░░░░░░░░░░");
    // proposition: [####.........]
}

// -----------------------------------------------------------------------------

static void drawWidgetInternal(const Widget *pWgt)
{
    bool en = g.pWndState->isEnabled(pWgt);
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

struct WidgetSearchStruct
{
    WID   searchedID = {};      // given
    Coord parentCoord = {};     // expected
    bool  isVisible = true;     // expected
    const Widget *pWidget = {}; // expected
    const Widget *pParent = {}; // expected
};

template<typename T>
bool iterateParent(const T *pParentDetails, const Widget *pParent, WidgetSearchStruct &wss)
{
    if (findWidget(pParentDetails->pChildrens, pParentDetails->childCount, wss))
    {
        if (!wss.pParent)
            wss.pParent = pParent;

        wss.parentCoord += pParent->coord;
        wss.isVisible &= g.pWndState->isVisible(pParent);

        // pages coords == pagecontrol coords + tabs width
        if (pParent->type == Widget::Type::PageCtrl)
            wss.parentCoord.col += pParent->pagectrl.tabWidth;

        return true;
    }

    return false;
}


// recursive function searching for widgetId
static bool findWidget(const Widget widgets[], const uint16_t widgetsCount, WidgetSearchStruct &wss)
{
    const auto *p_wgt = widgets;

    for (uint16_t w = 0; w < widgetsCount; w++)
    {
        if (p_wgt->id == wss.searchedID)
        {
            wss.pWidget = p_wgt;
            wss.isVisible = g.pWndState->isVisible(wss.pWidget);
            return true;
        }

        if (p_wgt->type == Widget::Window)
        {
            if (iterateParent(&p_wgt->window, p_wgt, wss))
                return true;
            // window is top-most, sole widget
            return false;
        }
        else
        if (p_wgt->type == Widget::Panel)
        {
            if (iterateParent(&p_wgt->panel, p_wgt, wss))
                return true;
        }
        else
        if (p_wgt->type == Widget::PageCtrl)
        {
            if (iterateParent(&p_wgt->pagectrl, p_wgt, wss))
                return true;
        }
        else
        if (p_wgt->type == Widget::Page)
        {
            if (iterateParent(&p_wgt->page, p_wgt, wss))
                return true;
        }

        p_wgt++;
    }

    return false;
}

static const Widget* findWidget(const WID widgetId)
{
    if (!g.pWnd || widgetId == WIDGET_ID_NONE)
        return nullptr;

    WidgetSearchStruct wss { searchedID : widgetId };

    // search window for widget by it's Id
    if (findWidget(g.pWnd, 1, wss))
        return wss.pWidget;

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

static bool isParent(const Widget *pWgt)
{
    if (!pWgt)
        return false;

    switch (pWgt->type)
    {
    case Widget::Window:
    case Widget::Panel:
    case Widget::PageCtrl:
    case Widget::Page:
        return true;
    default:
        return false;
    }
}

static bool isFocusable(const Widget *pWgt)
{
    if (!pWgt)
        return false;

    switch (pWgt->type)
    {
    case Widget::Edit:
    case Widget::CheckBox:
    case Widget::Button:
    //case Widget::PageCtrl:
        return true;
    default:
        return false;
    }
}

static const Widget* getNextFocusable(const Widget *pParent, const WID focusedID)
{
    if (!pParent)
        return nullptr;

    const Widget *p_childs = {};
    uint16_t child_cnt = 0;

    switch (pParent->type)
    {
    case Widget::Window:
        p_childs = pParent->window.pChildrens;
        child_cnt = pParent->window.childCount;
        break;
    case Widget::Panel:
        p_childs = pParent->panel.pChildrens;
        child_cnt = pParent->panel.childCount;
        break;
    case Widget::Page:
        p_childs = pParent->page.pChildrens;
        child_cnt = pParent->page.childCount;
        break;
    case Widget::PageCtrl:
        {
            // get selected page childrens
            int idx = g.pWndState->getPageCtrlPageIndex(pParent);
            if (idx >= 0 && idx < pParent->pagectrl.childCount)
            {
                pParent = &pParent->pagectrl.pChildrens[idx];
                //TWINS_LOG("search pgctrl page %d", idx);
                p_childs =  pParent->page.pChildrens;
                child_cnt = pParent->page.childCount;
            }
        }
        break;
    default:
        TWINS_LOG("-E- no-parent widget");
        return nullptr;
    }


    if (focusedID == WIDGET_ID_NONE)
    {
        //TWINS_LOG("Search * in %s items[%d]", toString(pParent->type), child_cnt);

        // give me first focusable
        for (uint16_t i = 0; i < child_cnt; i++)
        {
            const auto *p_wgt = &p_childs[i];
            //TWINS_LOG("  %s(%d)", toString(p_wgt->type), p_wgt->id);

            if (isFocusable(p_wgt))
                return p_wgt;

            if (isParent(p_wgt))
            {
                //TWINS_LOG("Search parent %s(%d)", toString(p_wgt->type), p_wgt->id);
                if ((p_wgt = getNextFocusable(p_wgt, focusedID)))
                    return p_wgt;
            }
        }
    }
    else
    {
        const Widget *p_wgt = {};

        // find widget next to curent
        for (uint16_t i = 0; i < child_cnt && !p_wgt; i++)
            if (p_childs[i].id == focusedID)
                p_wgt = &p_childs[i+1];

        if (p_wgt)
        {
            //TWINS_LOG("Search in %s childs[%d]", toString(pParent->type), child_cnt);

            // iterate until focusable found
            for (uint16_t i = 0; i < child_cnt; i++)
            {
                if (p_wgt == p_childs + child_cnt)
                    p_wgt = p_childs;

                //TWINS_LOG("  %s(%d)", toString(p_wgt->type), p_wgt->id);

                if (isFocusable(p_wgt))
                    return p_wgt;

                if (isParent(p_wgt))
                {
                    //TWINS_LOG("Search parent %s(%d)", toString(p_wgt->type), p_wgt->id);
                    if (const auto *p = getNextFocusable(p_wgt, focusedID))
                        return p;
                }
            }
        }
    }

    return nullptr;
}

static WID focusNext(const Widget *pWindow, const WID focusedID)
{
    assert(pWindow);
    assert(pWindow->type == Widget::Window);

    WidgetSearchStruct wss { searchedID : focusedID };

    // search window for widget by it's Id
    if (!findWidget(pWindow, 1, wss))
    {
        //TWINS_LOG("findWidget() failed");
        wss.pParent = pWindow;
    }

    //TWINS_LOG("wgt %d parent %s(%d)", wss.searchedID, toString(wss.pParent->type), wss.pParent->id);

    // use the parent to get next widget
    if (auto *p_next = getNextFocusable(wss.pParent, focusedID))
    {
        //TWINS_LOG("found %d", p_next->id);
        return p_next->id;
    }

    //TWINS_LOG("ret WIDGET_ID_NONE");
    return WIDGET_ID_NONE;
}

static WID focusParent(const Widget *pWindow, WID focusedID)
{
    assert(pWindow);
    assert(pWindow->type == Widget::Window);

    if (focusedID == WIDGET_ID_NONE)
        return pWindow->id;

    WidgetSearchStruct wss { searchedID : focusedID };

    // search window for widget by it's Id
    if (findWidget(pWindow, 1, wss))
    {
        if (wss.pParent)
            return wss.pParent->id;

        return pWindow->id;
    }

    //TWINS_LOG("ret WIDGET_ID_NONE");
    return WIDGET_ID_NONE;
}

static void processEditKey(const Widget *pEdit, const KeyCode &kc)
{
    // TODO: implement
    g.pWndState->onEditChange(pEdit, g.str);
    g.pWndState->invalidate(pEdit->id);
}

// -----------------------------------------------------------------------------

void drawWidget(const Widget *pWindow, WID widgetId)
{
    assert(pWindow);
    assert(pWindow->type == Widget::Window);
    g.pWnd = pWindow;
    g.pWndState = pWindow->window.getState();
    assert(g.pWndState);
    cursorHide();

    if (widgetId == WIDGET_ID_ALL)
    {
        drawWidgetInternal(pWindow);
    }
    else
    {
        WidgetSearchStruct wss { searchedID : widgetId };

        // search window for widget by it's Id
        if (findWidget(pWindow, 1, wss))
        {
            if (wss.isVisible)
            {
                g.parentCoord = wss.parentCoord;
                // set parent's background color
                pushClrBg(getWidgetBgColor(wss.pParent));
                drawWidgetInternal(wss.pWidget);
                popClrBg();
            }
        }
    }

    resetAttr();
    resetClrBg();
    resetClrFg();
    cursorShow();
}

void drawWidgets(const Widget *pWindow, const WID *pWidgetIds, uint16_t count)
{
    assert(pWindow);
    assert(pWindow->type == Widget::Window);
    g.pWnd = pWindow;
    g.pWndState = pWindow->window.getState();
    assert(g.pWndState);
    cursorHide();

    for (unsigned i = 0; i < count; i++)
    {
        WidgetSearchStruct wss;
        wss.searchedID = pWidgetIds[i];
        wss.pParent = pWindow;

        // search window for widget by it's Id
        if (findWidget(pWindow, 1, wss))
        {
            if (wss.isVisible)
            {
                g.parentCoord = wss.parentCoord;
                // set parent's background color
                pushClrBg(getWidgetBgColor(wss.pParent));
                drawWidgetInternal(wss.pWidget);
                popClrBg();
            }
        }
    }

    resetAttr();
    resetClrBg();
    resetClrFg();
    cursorShow();
}

// -----------------------------------------------------------------------------

void processKey(const Widget *pWindow, const KeyCode &kc)
{
    assert(pWindow);
    assert(pWindow->type == Widget::Window);
    g.pWnd = pWindow;
    g.pWndState = pWindow->window.getState();
    assert(g.pWndState);

    //TWINS_LOG("---");

    if (kc.m_spec)
    {
        switch (kc.key)
        {
        case Key::Esc:
        {
            auto &curr_id = g.pWndState->getFocusedID();
            const Widget* p_wgt = findWidget(curr_id);
            if (p_wgt && p_wgt->type == Widget::Edit)
                processEditKey(p_wgt, kc);

            auto new_id = focusParent(pWindow, curr_id);
            //TWINS_LOG("ESC: %d -> %d", curr_id, new_id);

            if (new_id != curr_id)
            {
                auto prev_id = curr_id;
                curr_id = new_id;
                g.pWndState->invalidate(prev_id);
                g.pWndState->invalidate(new_id);
            }
            break;
        }
        case Key::Tab:
        {
            // TODO: if in edit mode - cancel
            auto &curr_id = g.pWndState->getFocusedID();
            auto new_id = focusNext(pWindow, curr_id);
            //TWINS_LOG("TAB: %d -> %d", curr_id, new_id);

            if (new_id != curr_id)
            {
                auto prev_id = curr_id;
                curr_id = new_id;
                g.pWndState->invalidate(prev_id);
                g.pWndState->invalidate(new_id);
            }
            break;
        }
        case Key::PgUp:
        case Key::PgDown:
        {
            // assume only one page control on window, so PgUp, PgDown
            // will always be assigned to this control
            for (uint16_t i = 0; i < pWindow->window.childCount; i++)
            {
                const auto *p_wgt = &pWindow->window.pChildrens[i];

                if (p_wgt->type == Widget::PageCtrl)
                {
                    int idx = g.pWndState->getPageCtrlPageIndex(p_wgt);
                    idx += kc.key == Key::PgDown ? 1 : -1;
                    if (idx < 0)                           idx = p_wgt->pagectrl.childCount -1;
                    if (idx >= p_wgt->pagectrl.childCount) idx = 0;

                    //TWINS_LOG("PG.UP/DWN: newPage%d", idx);
                    g.pWndState->onPageControlPageChange(p_wgt, idx);
                    g.pWndState->invalidate(p_wgt->id);
                }
            }
        }
        case Key::Enter:
        {
            auto curr_id = g.pWndState->getFocusedID();
            if (const Widget* p_wgt = findWidget(curr_id))
            {
                switch (p_wgt->type)
                {
                case Widget::Edit:
                    processEditKey(p_wgt, kc);
                    break;
                case Widget::CheckBox:
                    g.pWndState->onCheckboxToggle(p_wgt);
                    g.pWndState->invalidate(p_wgt->id);
                    break;
                case Widget::Button:
                    g.pWndState->onButtonClick(p_wgt);
                    g.pWndState->invalidate(p_wgt->id);
                    break;
                default:
                    break;
                }
            }
            break;
        }
        case Key::Backspace:
        case Key::Delete:
        case Key::Left:
        case Key::Right:
        case Key::Home:
        case Key::End:
        {
            auto curr_id = g.pWndState->getFocusedID();
            const Widget* p_wgt = findWidget(curr_id);
            if (p_wgt && p_wgt->type == Widget::Edit)
                processEditKey(p_wgt, kc);
            break;
        }
        default:
            break;
        }
    }
    else if (kc.mod_all == KEY_MOD_NONE)
    {
        auto curr_id = g.pWndState->getFocusedID();
        if (const Widget* p_wgt = findWidget(curr_id))
        {
            if (kc.utf8[0] == ' ')
            {
                switch (p_wgt->type)
                {
                case Widget::CheckBox:
                    g.pWndState->onCheckboxToggle(p_wgt);
                    g.pWndState->invalidate(p_wgt->id);
                    break;
                case Widget::DropDownList:
                    // show popup-list
                    break;
                default:
                    break;
                }
            }

            if (p_wgt->type == Widget::Edit)
            {
                processEditKey(p_wgt, kc);
            }
        }
    }
}

// -----------------------------------------------------------------------------

}
