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
    Coord   parentCoord;            // current widget left-top position
    String  str;                    // common string buff for widget renderers
    const Widget *pWndArray = {};   // array of Window widgets
    IWindowState *pWndState = {};   //
} g;

// -----------------------------------------------------------------------------

void log(const char *file, const char *func, unsigned line, const char *fmt, ...)
{
    FontMemento _m;
    twins::pushClBg(ColorBG::Default);
    twins::pushClFg(ColorFG::White);

    uint16_t row = pIOs->getLogsRow();
    moveTo(1, row);
    insertLines(1);

    // display only file name, trim the path
    if (const char *delim = strrchr(file, '/'))
        file = delim + 1;

    time_t t = time(NULL);
    struct tm *p_stm = localtime(&t);
    writeStrFmt("[%2d:%02d:%02d] %s() %s:%u: ",
        p_stm->tm_hour, p_stm->tm_min, p_stm->tm_sec,
        func, file, line);

    twins::pushClFg(ColorFG::WhiteIntense);

    va_list ap;
    va_start(ap, fmt);
    pIOs->writeStrFmt(fmt, ap);
    pIOs->flushBuff();
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
    if (clBg != ColorBG::None) pushClBg(clBg);
    if (clFg != ColorFG::None) pushClFg(clFg);

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
        auto capt_len = utf8len(pWgt->window.title);
        moveTo(pWgt->coord.col + (pWgt->size.width - capt_len - 4)/2,
            pWgt->coord.row);
        pushClFg(ColorFG::Yellow);
        pushAttr(FontAttrib::Bold);
        writeStrFmt("╡ %s ╞", pWgt->window.title);
        popAttr();
        popClFg();
    }

    g.parentCoord = pWgt->coord;

    for (int i = pWgt->link.childsIdx; i < pWgt->link.childsIdx + pWgt->link.childsCnt; i++)
        drawWidgetInternal(&g.pWndArray[i]);

    // reset colors set by frame drawer
    popClBg();
    popClFg();
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

    for (int i = pWgt->link.childsIdx; i < pWgt->link.childsIdx + pWgt->link.childsCnt; i++)
        drawWidgetInternal(&g.pWndArray[i]);

    g.parentCoord = coord_bkp;

    // reset colors set by frame drawer
    popClBg();
    popClFg();
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
    pushClBg(pWgt->label.bgColor);
    pushClFg(pWgt->label.fgColor);
    // print label
    writeStr(g.str.cstr());
    // fill remaining space with spaces;
    // count UTF-8 sequences, not bytes
    int n = utf8len(g.str.cstr());
    writeChar(' ', max_len - n);
    // restore colors
    popClFg();
    popClBg();
}

static void drawLed(const Widget *pWgt)
{
    auto bgCl = g.pWndState->getLedLit(pWgt) ? pWgt->led.bgColorOn : pWgt->led.bgColorOff;
    // led text
    moveTo(g.parentCoord.col + pWgt->coord.col, g.parentCoord.row + pWgt->coord.row);
    pushClBg(bgCl);
    pushClFg(pWgt->led.fgColor);
    writeStr(pWgt->led.text);
    popClFg();
    popClBg();
}

static void drawCheckbox(const Widget *pWgt)
{
    const char *s_chk_state = g.pWndState->getCheckboxChecked(pWgt) ? "[x] " : "[ ] ";
    bool focused = g.pWndState->isFocused(pWgt);

    moveTo(g.parentCoord.col + pWgt->coord.col, g.parentCoord.row + pWgt->coord.row);
    if (focused) pushAttr(FontAttrib::Inverse);
    pushClFg(pWgt->checkbox.fgColor == ColorFG::None ? ColorFG::Default : pWgt->checkbox.fgColor);
    writeStr(s_chk_state);
    writeStr(pWgt->checkbox.text);
    popClFg();
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
    pushClFg(pWgt->button.fgColor == ColorFG::None ? ColorFG::Default : pWgt->button.fgColor);
    writeStr(g.str.cstr());
    popClFg();
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

    // draw tabs and pages
    const int pg_idx = g.pWndState->getPageCtrlPageIndex(pWgt);
    const bool focused = g.pWndState->isFocused(pWgt);
    moveTo(g.parentCoord.col + pWgt->coord.col, g.parentCoord.row + pWgt->coord.row);

    for (int i = 0; i < pWgt->link.childsCnt; i++)
    {
        if (i == pWgt->size.height-1)
            break;

        const auto *p_page = &g.pWndArray[pWgt->link.childsIdx + i];

        // draw page title
        g.str.clear();
        g.str.appendFmt("%s%s", i == pg_idx ? "►" : " ", p_page->page.title);
        g.str.append(' ', pWgt->pagectrl.tabWidth - utf8len(p_page->page.title));
        g.str.trim(pWgt->pagectrl.tabWidth, true);

        moveTo(my_coord.col, my_coord.row + i + 1);
        pushClFg(p_page->page.fgColor);
        if (/* focused && */ i == pg_idx) pushAttr(FontAttrib::Inverse);
        writeStr(g.str.cstr());
        if (/* focused && */ i == pg_idx) popAttr();
        popClFg();

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
    for (int i = pWgt->link.childsIdx; i < pWgt->link.childsIdx + pWgt->link.childsCnt; i++)
        drawWidgetInternal(&g.pWndArray[i]);
}

static void drawProgressBar(const Widget *pWgt)
{
    int pos = 0, max = 1;
    g.pWndState->getProgressBarState(pWgt, pos, max);

    if (max <= 0) max = 1;
    if (pos > max) pos = max;

    moveTo(g.parentCoord.col + pWgt->coord.col, g.parentCoord.row + pWgt->coord.row);
    g.str.clear();
    int fill = pos * pWgt->size.width / max;
    g.str.append("█", fill);
    g.str.append("▒", pWgt->size.width - fill);

    pushClFg(pWgt->progressbar.fgColor);
    writeStr(g.str.cstr());
    popClFg();

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
    case Widget::List:          break;
    case Widget::DropDownList:  break;
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
};

static bool findWidget(WidgetSearchStruct &wss)
{
    if (wss.searchedID == WIDGET_ID_NONE)
        return false;

    const Widget *p_wgt = g.pWndArray;

    for (;; p_wgt++)
    {
        if (p_wgt->id == wss.searchedID)
            break;

        // pWndArray is terminated by empty entry
        if (p_wgt->id == WIDGET_ID_NONE)
            return false;
    }

    wss.pWidget = p_wgt;
    wss.isVisible = g.pWndState->isVisible(p_wgt);

    // go up the widgets hierarchy
    int parent_idx = p_wgt->link.parentIdx;

    for (;;)
    {
        const auto *p_parent = g.pWndArray + parent_idx;
        wss.isVisible &= g.pWndState->isVisible(p_parent);
        wss.parentCoord += p_parent->coord;

        if (p_parent->type == Widget::Type::PageCtrl)
            wss.parentCoord.col += p_parent->pagectrl.tabWidth;

        if (parent_idx == 0)
            break;

        parent_idx = p_parent->link.parentIdx;
    }

    return true;
}

static const Widget* findWidget(const WID widgetId)
{
    WidgetSearchStruct wss { searchedID : widgetId };

    if (findWidget(wss))
        return wss.pWidget;

    return nullptr;
}

static ColorBG getWidgetBgColor(const Widget *pWgt)
{
    if (pWgt)
    {
        switch (pWgt->type)
        {
        case Widget::Window:
            return pWgt->window.bgColor; break;
        case Widget::Panel:
            return pWgt->panel.bgColor; break;
        case Widget::Page:
        case Widget::PageCtrl:
            return getWidgetBgColor(g.pWndArray + pWgt->link.parentIdx);
        default:
            break;
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
    case Widget::List:
    case Widget::DropDownList:
        return true;
    default:
        return false;
    }
}

static const Widget* getNextFocusable(const Widget *pParent, const WID focusedID, bool forward = true)
{
    if (!pParent)
        return nullptr;

    const Widget *p_childs = {};
    uint16_t child_cnt = 0;

    switch (pParent->type)
    {
    case Widget::Window:
    case Widget::Panel:
    case Widget::Page:
        p_childs  = &g.pWndArray[pParent->link.childsIdx];
        child_cnt = pParent->link.childsCnt;
        break;
    case Widget::PageCtrl:
        {
            // get selected page childrens
            int idx = g.pWndState->getPageCtrlPageIndex(pParent);
            if (idx >= 0 && idx < pParent->link.childsCnt)
            {
                pParent = &g.pWndArray[pParent->link.childsIdx + idx];
                //TWINS_LOG("search pgctrl page %d", idx);
                p_childs  = &g.pWndArray[pParent->link.childsIdx];
                child_cnt = pParent->link.childsCnt;
            }
            else
            {
                return nullptr;
            }
        }
        break;
    default:
        TWINS_LOG("-E- no-parent widget");
        return nullptr;
    }

    assert(p_childs);

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

static WID focusNext(const WID focusedID, bool forward)
{
    WidgetSearchStruct wss { searchedID : focusedID };

    if (!findWidget(wss))
    {
        // here, find may fail only if invalid focusedID was given
        wss.pWidget = g.pWndArray;
    }

    // use the parent to get next widget
    if (auto *p_next = getNextFocusable(g.pWndArray + wss.pWidget->link.parentIdx, focusedID))
        return p_next->id;

    return WIDGET_ID_NONE;
}

static WID focusParent(WID focusedID)
{
    if (focusedID == WIDGET_ID_NONE)
        return g.pWndArray[0].id;

    WidgetSearchStruct wss { searchedID : focusedID };

    if (findWidget(wss))
        return g.pWndArray[wss.pWidget->link.parentIdx].id;

    return WIDGET_ID_NONE;
}

static void processEditKey(const Widget *pEdit, const KeyCode &kc)
{
    // TODO: implement
    g.pWndState->onEditChange(pEdit, g.str);
    g.pWndState->invalidate(pEdit->id);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void drawWidget(const Widget *pWindowArray, WID widgetId)
{
    assert(pWindowArray);
    assert(pWindowArray->type == Widget::Window);
    g.pWndArray = pWindowArray;
    g.pWndState = pWindowArray->window.getState();
    assert(g.pWndState);
    cursorHide();

    if (widgetId == WIDGET_ID_ALL)
    {
        drawWidgetInternal(pWindowArray);
    }
    else
    {
        WidgetSearchStruct wss { searchedID : widgetId };

        // search window for widget by it's Id
        if (findWidget(wss) && wss.isVisible)
        {
            g.parentCoord = wss.parentCoord;
            // set parent's background color
            pushClBg(getWidgetBgColor(g.pWndArray + wss.pWidget->link.parentIdx));
            drawWidgetInternal(wss.pWidget);
            popClBg();
        }
    }

    resetAttr();
    resetClBg();
    resetClFg();
    cursorShow();
}

void drawWidgets(const Widget *pWindowArray, const WID *pWidgetIds, uint16_t count)
{
    assert(pWindowArray);
    assert(pWindowArray->type == Widget::Window);
    g.pWndArray = pWindowArray;
    g.pWndState = pWindowArray->window.getState();
    assert(g.pWndState);
    cursorHide();

    for (unsigned i = 0; i < count; i++)
    {
        WidgetSearchStruct wss { searchedID : pWidgetIds[i] };

        if (findWidget(wss) && wss.isVisible)
        {
            g.parentCoord = wss.parentCoord;
            // set parent's background color
            pushClBg(getWidgetBgColor(g.pWndArray + wss.pWidget->link.parentIdx));
            drawWidgetInternal(wss.pWidget);
            popClBg();
        }
    }

    resetAttr();
    resetClBg();
    resetClFg();
    cursorShow();
}

// -----------------------------------------------------------------------------

void processKey(const Widget *pWindowArray, const KeyCode &kc)
{
    assert(pWindowArray);
    assert(pWindowArray->type == Widget::Window);
    g.pWndArray = pWindowArray;
    g.pWndState = pWindowArray->window.getState();
    assert(g.pWndState);

    if (kc.key == Key::None)
        return;

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

            auto new_id = focusParent(curr_id);
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
            auto &curr_id = g.pWndState->getFocusedID();
            auto new_id = focusNext(curr_id, !kc.m_shift);
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
            for (unsigned i = 0; i < pWindowArray->link.childsCnt; i++)
            {
                const auto *p_wgt = &g.pWndArray[pWindowArray->link.childsIdx + i];

                if (p_wgt->type == Widget::PageCtrl)
                {
                    int idx = g.pWndState->getPageCtrlPageIndex(p_wgt);
                    idx += kc.key == Key::PgDown ? 1 : -1;
                    if (idx < 0)                      idx = p_wgt->link.childsCnt -1;
                    if (idx >= p_wgt->link.childsCnt) idx = 0;

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
