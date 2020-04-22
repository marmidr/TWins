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

struct Rect
{
    Coord coord;
    Size  size;

    void setMax()
    {
        coord.col = 1;
        coord.row = 1;
        size.width = 0xff;
        size.height = 0xff;
    }
};

struct EditState
{
    uint8_t cursorCol;
    String  txt;
};

/** Global state object */
static struct
{
    Coord   parentCoord;            // current widget left-top position
    String  str;                    // common string buff for widget renderers
    const Widget *pWndArray = {};   // array of Window widgets
    IWindowState *pWndState = {};   //
    EditState editState;            // state of Edit being modified
    int listboxHighlightIdx;        // focused Listbox index of highlighted row
} g;

// -----------------------------------------------------------------------------

void log(const char *file, const char *func, unsigned line, const char *fmt, ...)
{
    FontMemento _m;
    cursorSavePos();

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

    cursorRestorePos();
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
    CASE_WGT_STR(Radio)
    CASE_WGT_STR(Button)
    CASE_WGT_STR(Led)
    CASE_WGT_STR(PageCtrl)
    CASE_WGT_STR(Page)
    CASE_WGT_STR(ProgressBar)
    CASE_WGT_STR(ListBox)
    CASE_WGT_STR(DropDownList)
    default: return "?";
    }
}

// -----------------------------------------------------------------------------

static void operator += (Coord &cord, const Coord &offs)
{
    cord.col += offs.col;
    cord.row += offs.row;
}

static void operator -= (Coord &cord, const Coord &offs)
{
    cord.col -= offs.col;
    cord.row -= offs.row;
}

static Coord operator + (const Coord &cord1, const Coord &cord2)
{
    Coord ret = {
        uint8_t(cord1.col + cord2.col),
        uint8_t(cord1.row + cord2.row)
    };
    return ret;
}

static Coord operator + (const Coord &cord, const Size offs)
{
    Coord ret = {
        uint8_t(cord.col + offs.width),
        uint8_t(cord.row + offs.height)
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

static bool isPointWithin(uint8_t col, uint8_t row, const Rect& e)
{
    return col >= e.coord.col &&
           col <  e.coord.col + e.size.width &&
           row >= e.coord.row &&
           row <  e.coord.row + e.size.height;
}

static bool isRectWithin(const Rect& i, const Rect& e)
{
    return i.coord.col                 >= e.coord.col &&
           i.coord.col + i.size.width  <  e.coord.col + e.size.width &&
           i.coord.row                 >= e.coord.row &&
           i.coord.row + i.size.height <  e.coord.row + e.size.height;
}

// -----------------------------------------------------------------------------

static void drawArea(const Coord coord, const Size size, ColorBG clBg, ColorFG clFg, const FrameStyle style, bool filled = true)
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
    if (filled)
        g.str.append(frame[4], size.width - 2);
    else
        g.str.appendFmt(ESC_CURSOR_FORWARD_FMT, size.width - 2);
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

static void drawScrollBarV(const Coord coord, int height, int max, int pos)
{
    if (pos > max)
    {
        TWINS_LOG("pos %d > max %d", pos, max);
        return;
    }

    bufferBegin();
    const int slider_at = ((height-1) * pos) / max;
    pushClFg(ColorFG::Default);
    // "▲▴ ▼▾ ◄◂ ►▸ ◘ █";

    for (int i = 0; i < height; i++)
    {
        moveTo(coord.col, coord.row + i);
        pIOs->writeStr(i == slider_at ? "◘" : "▒");
    }

    popClFg();
    bufferEnd();
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
    g.str.clear();

    // label text
    if (pWgt->label.text)
        g.str = pWgt->label.text;
    else
        g.pWndState->getLabelText(pWgt, g.str);

    // setup colors
    pushClBg(pWgt->label.bgColor);
    pushClFg(pWgt->label.fgColor);

    // print all lines
    const char *p_line = g.str.cstr();
    String s_line;
    moveTo(g.parentCoord.col + pWgt->coord.col, g.parentCoord.row + pWgt->coord.row);

    for (int line = 0; line < pWgt->size.height; line++)
    {
        s_line.clear();
        const char *p_eol = strchr(p_line, '\n');

        if (p_eol)
        {
            // one or 2+ lines
            s_line.appendLen(p_line, p_eol - p_line);
            p_line = p_eol + 1;
        }
        else
        {
            // only or last line of text
            s_line.append(p_line);
            p_line = " ";
        }

        // limit the length only of text is plain (has no ANSI ESC sequences)
        if (strchr(s_line.cstr(), '\e') == nullptr)
            s_line.setLength(pWgt->size.width, true);
        writeStr(s_line.cstr());
        moveBy(-pWgt->size.width, 1);
    }

    // restore colors
    popClFg();
    popClBg();
}

static void drawLed(const Widget *pWgt)
{
    auto bgCl = g.pWndState->getLedLit(pWgt) ? pWgt->led.bgColorOn : pWgt->led.bgColorOff;
    g.str.clear();

    if (pWgt->led.text)
        g.str = pWgt->led.text;
    else
        g.pWndState->getLedText(pWgt, g.str);

    // led text
    moveTo(g.parentCoord.col + pWgt->coord.col, g.parentCoord.row + pWgt->coord.row);
    pushClBg(bgCl);
    pushClFg(pWgt->led.fgColor);
    writeStr(g.str.cstr());
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

static void drawRadio(const Widget *pWgt)
{
    const char *s_radio_state = pWgt->radio.radioId == g.pWndState->getRadioIndex(pWgt) ? "(●) " : "( ) ";
    bool focused = g.pWndState->isFocused(pWgt);

    moveTo(g.parentCoord.col + pWgt->coord.col, g.parentCoord.row + pWgt->coord.row);
    if (focused) pushAttr(FontAttrib::Inverse);
    writeStr(s_radio_state);
    writeStr(pWgt->radio.text);
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

    // TODO: set colors because color after terminal is used frequently

    drawArea(my_coord + Coord{pWgt->pagectrl.tabWidth, 0}, pWgt->size - Size{pWgt->pagectrl.tabWidth, 0},
        ColorBG::None, ColorFG::None, FrameStyle::PgControl);

    auto coord_bkp = g.parentCoord;
    g.parentCoord = my_coord;
    // tabs title
    g.str.clear();
    g.str.append(" ≡ MENU ≡ ");
    g.str.setLength(pWgt->pagectrl.tabWidth);
    moveTo(my_coord.col, my_coord.row);
    pushAttr(FontAttrib::Inverse);
    writeStr(g.str.cstr());
    popAttr();

    // draw tabs and pages
    const int pg_idx = g.pWndState->getPageCtrlPageIndex(pWgt);
    // const bool focused = g.pWndState->isFocused(pWgt);
    moveTo(g.parentCoord.col + pWgt->coord.col, g.parentCoord.row + pWgt->coord.row);

    for (int i = 0; i < pWgt->link.childsCnt; i++)
    {
        if (i == pWgt->size.height-1)
            break;

        const auto *p_page = &g.pWndArray[pWgt->link.childsIdx + i];

        // draw page title
        g.str.clear();
        g.str.appendFmt("%s%s", i == pg_idx ? "►" : " ", p_page->page.title);
        g.str.setLength(pWgt->pagectrl.tabWidth, true);

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

    // ████░░░░░░░░░░░
    // [####.........]
    // [■■■■□□□□□□□□□]
}

static void drawListBox(const Widget *pWgt)
{
    const auto my_coord = g.parentCoord + pWgt->coord;
    drawArea(my_coord, pWgt->size,
        ColorBG::None, ColorFG::None, FrameStyle::Single, false);

    if (pWgt->size.height < 3)
        return;

    int item_idx = 0;
    int items_cnt = 0;
    g.pWndState->getListBoxState(pWgt, item_idx, items_cnt);

    const uint8_t items_visible = pWgt->size.height - 2;
    const uint8_t topitem = (g.listboxHighlightIdx / items_visible) * items_visible;
    const bool focused = g.pWndState->isFocused(pWgt);

    if (items_cnt > pWgt->size.height-2)
        drawScrollBarV(my_coord + Size{uint8_t(pWgt->size.width-1), 1}, pWgt->size.height-2, items_cnt-1, g.listboxHighlightIdx);

    String s;

    for (int i = 0; i < items_visible; i++)
    {
        bool is_current_item = topitem + i == item_idx;
        bool is_hl_item = topitem + i == g.listboxHighlightIdx;
        moveTo(my_coord.col+1, my_coord.row + i + 1);

        s.clear();

        if (topitem + i < items_cnt)
        {
            s.append(is_current_item ? "►" : " ");
            g.pWndState->getListBoxItem(pWgt, topitem + i, s);
            s.setLength(pWgt->size.width-2, true);
        }
        else
        {
            s.setLength(pWgt->size.width-2);
        }

        if (focused && is_hl_item) pushAttr(FontAttrib::Inverse);
        writeStr(s.cstr());
        if (focused && is_hl_item) popAttr();
    }
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
    case Widget::Radio:         drawRadio(pWgt);  break;
    case Widget::Button:        drawButton(pWgt); break;
    case Widget::Led:           drawLed(pWgt); break;
    case Widget::PageCtrl:      drawPageControl(pWgt); break;
    case Widget::Page:          drawPage(pWgt); break;
    case Widget::ProgressBar:   drawProgressBar(pWgt); break;
    case Widget::ListBox:       drawListBox(pWgt); break;;
    case Widget::DropDownList:  break;
    default:                    break;
    }

    if (!en) popAttr();
}

static void invalidateRadioGroup(const Widget *pRadio)
{
    const Widget *p_parent = g.pWndArray + pRadio->link.parentIdx;
    const auto group_id = pRadio->radio.groupId;

    for (unsigned i = 0; i < p_parent->link.childsCnt; i++)
    {
        const auto *p_wgt = g.pWndArray + p_parent->link.childsIdx + i;
        if (p_wgt->type == Widget::Type::Radio && p_wgt->radio.groupId == group_id)
            g.pWndState->invalidate(p_wgt->id);
    }
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

static Coord getScreenCoord(const Widget *pWgt)
{
    Coord coord = pWgt->coord;

    // go up the widgets hierarchy
    int parent_idx = pWgt->link.parentIdx;

    for (;;)
    {
        const auto *p_parent = g.pWndArray + parent_idx;
        coord += p_parent->coord;

        if (p_parent->type == Widget::Type::PageCtrl)
            coord.col += p_parent->pagectrl.tabWidth;

        if (parent_idx == 0)
            break;

        parent_idx = p_parent->link.parentIdx;
    }

    return coord;
}

static bool isVisible(const Widget *pWgt)
{
    bool vis = g.pWndState->isVisible(pWgt);
    int parent_idx = pWgt->link.parentIdx;

    for (; vis;)
    {
        const auto *p_parent = g.pWndArray + parent_idx;
        vis &= g.pWndState->isVisible(p_parent);

        if (parent_idx == 0)
            break;

        parent_idx = p_parent->link.parentIdx;
    }

    return vis;
}

static const Widget* getWidgetAt(uint8_t col, uint8_t row, Rect &wgtRect)
{
    const Widget *p_wgt_at = nullptr;
    Rect best_rect;
    best_rect.setMax();

    for (unsigned i = 0; g.pWndArray[i].type != Widget::None; i++)
    {
        Rect r;
        const auto *p_wgt = g.pWndArray + i;
        r.coord = getScreenCoord(p_wgt);
        r.size = p_wgt->size;

        // correct the widget size
        switch (p_wgt->type)
        {
        case Widget::Edit:
            break;
        case Widget::CheckBox:
            r.size.height = 1;
            r.size.width = utf8len(p_wgt->checkbox.text);
            break;
        case Widget::Radio:
            r.size.height = 1;
            r.size.width = 4 + utf8len(p_wgt->radio.text);
            break;
        case Widget::Button:
            r.size.height = 1;
            r.size.width = 4 + utf8len(p_wgt->button.text);
            break;
        case Widget::PageCtrl:
            r.size.width = p_wgt->pagectrl.tabWidth;
            break;
        case Widget::ListBox:
            break;
        case Widget::DropDownList:
            break;
        default:
            break;
        }

        if (isPointWithin(col, row, r))
        {
            bool is_visible = isVisible(p_wgt); // controls on tabs? solved

            if (is_visible && isRectWithin(r, best_rect))
            {
                p_wgt_at = p_wgt;
                best_rect = r;
                wgtRect = r;
            }
        }
    }

    return p_wgt_at;
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
    case Widget::Radio:
    case Widget::Button:
    //case Widget::PageCtrl:
    case Widget::ListBox:
    case Widget::DropDownList:
        return true;
    default:
        return false;
    }
}

static const Widget* getNextFocusable(const Widget *pParent, const WID focusedID, bool forward)
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
                if ((p_wgt = getNextFocusable(p_wgt, focusedID, forward)))
                    return p_wgt;
            }
        }
    }
    else
    {
        const Widget *p_wgt = {};

        // find widget next (prev) to current
        for (uint16_t i = 0; i < child_cnt && !p_wgt; i++)
        {
            if (p_childs[i].id == focusedID)
            {
                if (forward)
                {
                    if (i + 1 == child_cnt)
                        p_wgt = &p_childs[0];
                    else
                        p_wgt = &p_childs[i+1];
                }
                else
                {
                    if (i > 0)
                        p_wgt = &p_childs[i-1];
                    else
                        p_wgt = &p_childs[child_cnt-1];
                }

                break;
            }
        }

        if (p_wgt)
        {
            // TWINS_LOG("Search in %s childs[%d]", toString(pParent->type), child_cnt);

            // iterate until focusable found
            for (uint16_t i = 0; i < child_cnt; i++)
            {
                // TWINS_LOG("  %s(%d)", toString(p_wgt->type), p_wgt->id);

                if (isFocusable(p_wgt))
                    return p_wgt;

                if (isParent(p_wgt))
                {
                    //TWINS_LOG("Search parent %s(%d)", toString(p_wgt->type), p_wgt->id);
                    if (const auto *p = getNextFocusable(p_wgt, focusedID, forward))
                        return p;
                }

                if (forward)
                {
                    p_wgt++;
                    if (p_wgt == p_childs + child_cnt)
                        p_wgt = p_childs;
                }
                else
                {
                    p_wgt--;
                    if (p_wgt < p_childs)
                        p_wgt = p_childs + child_cnt - 1;
                }
            }
        }
    }

    return nullptr;
}

static void setCursorAt(const Widget *pWgt)
{
    if (!pWgt)
     return;

    Coord coord = getScreenCoord(pWgt);

    switch (pWgt->type)
    {
    case Widget::Edit:
        break;
    case Widget::CheckBox:
        coord.col += 1;
        break;
    case Widget::Radio:
        coord.col += 1;
        break;
    case Widget::Button:
        coord.col += 2;
        break;
    case Widget::ListBox:
        coord.col += 1;
        coord.row += 1;
        break;
    case Widget::DropDownList:
        break;
    default:
        break;
    }

    moveTo(coord.col, coord.row);
}

static WID getNextToFocus(const WID focusedID, bool forward)
{
    WidgetSearchStruct wss { searchedID : focusedID };

    if (!findWidget(wss))
    {
        // here, find may fail only if invalid focusedID was given
        wss.pWidget = g.pWndArray;
    }

    // use the parent to get next widget
    if (auto *p_next = getNextFocusable(g.pWndArray + wss.pWidget->link.parentIdx, focusedID, forward))
    {
        // setCursorAt(p_next);
        return p_next->id;
    }

    return WIDGET_ID_NONE;
}

static WID getParentToFocus(WID focusedID)
{
    if (focusedID == WIDGET_ID_NONE)
        return g.pWndArray[0].id;

    WidgetSearchStruct wss { searchedID : focusedID };

    if (findWidget(wss))
    {
        const auto *p_wgt = &g.pWndArray[wss.pWidget->link.parentIdx];
        g.parentCoord -= wss.pWidget->coord;
        // setCursorAt(p_wgt);
        return p_wgt->id;
    }

    return WIDGET_ID_NONE;
}

static bool changeFocusTo(WID newID)
{
    auto &curr_id = g.pWndState->getFocusedID();

    if (newID != curr_id)
    {
        auto prev_id = curr_id;
        curr_id = newID;

        WidgetSearchStruct wss { searchedID : newID };
        if (findWidget(wss))
        {
            if (wss.pWidget->type == Widget::ListBox)
            {
                int idx, cnt;
                g.pWndState->getListBoxState(wss.pWidget, idx, cnt);
                g.listboxHighlightIdx = idx;
            }
        }

        g.pWndState->invalidate(prev_id);
        g.pWndState->invalidate(newID);
        setCursorAt(wss.pWidget);
        return true;
    }

    return false;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

static bool processKey_Edit(const Widget *pWgt, const KeyCode &kc)
{
    bool key_handled = false;

    if (kc.m_spec)
    {
        switch (kc.key)
        {
        case Key::Enter:
            g.pWndState->onEditChange(pWgt, g.str);
            g.pWndState->invalidate(pWgt->id);
            key_handled = true;
            break;
        case Key::Backspace:
        case Key::Delete:
        case Key::Up:
        case Key::Down:
        case Key::Left:
        case Key::Right:
        case Key::Home:
        case Key::End:
            break;
        default:
            break;
        }
    }
    else
    {
        /* append char code */
    }

    return key_handled;
}

static bool processKey_CheckBox(const Widget *pWgt, const KeyCode &kc)
{
    if (kc.mod_all == KEY_MOD_NONE && kc.utf8[0] == ' ')
    {
        g.pWndState->onCheckboxToggle(pWgt);
        g.pWndState->invalidate(pWgt->id);
        return true;
    }

    if (kc.key == Key::Enter)
    {
        g.pWndState->onCheckboxToggle(pWgt);
        g.pWndState->invalidate(pWgt->id);
        return true;
    }

    return false;
}

static bool processKey_Radio(const Widget *pWgt, const KeyCode &kc)
{
    if (kc.mod_all == KEY_MOD_NONE && kc.utf8[0] == ' ')
    {
        g.pWndState->onRadioSelect(pWgt);
        invalidateRadioGroup(pWgt);
        return true;
    }

    if (kc.key == Key::Enter)
    {
        g.pWndState->onRadioSelect(pWgt);
        invalidateRadioGroup(pWgt);
        return true;
    }

    return false;
}

static bool processKey_Button(const Widget *pWgt, const KeyCode &kc)
{
    if (kc.key == Key::Enter)
    {
        g.pWndState->onButtonClick(pWgt);
        g.pWndState->invalidate(pWgt->id);
        return true;
    }

    return false;
}

static bool processKey_PageCtrl(const Widget *pWgt, const KeyCode &kc)
{
    if (kc.key == Key::PgDown || kc.key == Key::PgUp)
    {
        int idx = g.pWndState->getPageCtrlPageIndex(pWgt);
        idx += kc.key == Key::PgDown ? 1 : -1;
        if (idx < 0)                     idx = pWgt->link.childsCnt -1;
        if (idx >= pWgt->link.childsCnt) idx = 0;

        //TWINS_LOG("PG.UP/DWN: newPage%d", idx);
        g.pWndState->onPageControlPageChange(pWgt, idx);
        g.pWndState->invalidate(pWgt->id);
        return true;
    }

    return false;
}

static bool processKey_ListBox(const Widget *pWgt, const KeyCode &kc)
{
    if (kc.key == Key::Enter)
    {
        g.pWndState->onListBoxSelect(pWgt, g.listboxHighlightIdx);
        g.pWndState->invalidate(pWgt->id);
        return true;
    }
    else if (kc.key == Key::Up || kc.key == Key::Down)
    {
        int idx, cnt;
        g.pWndState->getListBoxState(pWgt, idx, cnt);
        int delta = kc.key == Key::Up ? -1 : 1;
        if (kc.m_ctrl) delta *= pWgt->size.height-2;
        g.listboxHighlightIdx += delta;

        if (g.listboxHighlightIdx < 0)
            g.listboxHighlightIdx = cnt - 1;

        if (g.listboxHighlightIdx >= cnt)
            g.listboxHighlightIdx = 0;

        g.pWndState->invalidate(pWgt->id);
        return true;
    }

    return false;
}

static bool processKey_DropDownList(const Widget *pWgt, const KeyCode &kc)
{
    return false;
}

static bool processKey(const KeyCode &kc)
{
    auto focused_id = g.pWndState->getFocusedID();
    const Widget* p_wgt = findWidget(focused_id);
    bool key_handled = false;

    if (!p_wgt)
        return false;

    switch (p_wgt->type)
    {
        case Widget::Edit:
            key_handled = processKey_Edit(p_wgt, kc);
            break;
        case Widget::CheckBox:
            key_handled = processKey_CheckBox(p_wgt, kc);
            break;
        case Widget::Radio:
            key_handled = processKey_Radio(p_wgt, kc);
            break;
        case Widget::Button:
            key_handled = processKey_Button(p_wgt, kc);
            break;
        case Widget::PageCtrl:
            key_handled = processKey_PageCtrl(p_wgt, kc);
            break;
        case Widget::ListBox:
            key_handled = processKey_ListBox(p_wgt, kc);
            break;
        case Widget::DropDownList:
            key_handled = processKey_DropDownList(p_wgt, kc);
            break;
        default:
            break;
    }

    return key_handled;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

static void processMouse_Edit(const Widget *pWgt, const Rect &wgtRect, const KeyCode &kc)
{
    moveTo(kc.mouse.col, kc.mouse.row);

    if (kc.mouse.btn == MouseBtn::ButtonLeft)
    {
        changeFocusTo(pWgt->id);
    }
}

static void processMouse_CheckBox(const Widget *pWgt, const Rect &wgtRect, const KeyCode &kc)
{
    moveTo(wgtRect.coord.col + 1, wgtRect.coord.row);

    if (kc.mouse.btn == MouseBtn::ButtonLeft)
    {
        changeFocusTo(pWgt->id);
        g.pWndState->onCheckboxToggle(pWgt);
        g.pWndState->invalidate(pWgt->id);
    }
}

static void processMouse_Radio(const Widget *pWgt, const Rect &wgtRect, const KeyCode &kc)
{
    moveTo(wgtRect.coord.col + 1, wgtRect.coord.row);

    if (kc.mouse.btn == MouseBtn::ButtonLeft)
    {
        changeFocusTo(pWgt->id);
        g.pWndState->onRadioSelect(pWgt);
        invalidateRadioGroup(pWgt);
    }
}

static void processMouse_Button(const Widget *pWgt, const Rect &wgtRect, const KeyCode &kc)
{
    moveTo(wgtRect.coord.col + wgtRect.size.width/2, wgtRect.coord.row);

    if (kc.mouse.btn == MouseBtn::ButtonLeft)
    {
        changeFocusTo(pWgt->id);
    }
    else if (kc.mouse.btn == MouseBtn::ButtonReleased)
    {
        g.pWndState->onButtonClick(pWgt);
        g.pWndState->invalidate(pWgt->id);
    }
}

static void processMouse_PageCtrl(const Widget *pWgt, const Rect &wgtRect, const KeyCode &kc)
{
    moveTo(wgtRect.coord.col, kc.mouse.row);

    if (kc.mouse.btn == MouseBtn::ButtonLeft)
    {
        changeFocusTo(pWgt->id);
        int idx = kc.mouse.row - wgtRect.coord.row - 1;

        if (idx >= 0 && idx < pWgt->link.childsCnt)
        {
            g.pWndState->onPageControlPageChange(pWgt, idx);
            g.pWndState->invalidate(pWgt->id);
        }
    }
    else if (kc.mouse.btn == MouseBtn::WheelUp || kc.mouse.btn == MouseBtn::WheelDown)
    {
        int idx = g.pWndState->getPageCtrlPageIndex(pWgt);
        idx += kc.mouse.btn == MouseBtn::WheelDown ? 1 : -1;
        if (idx < 0)                     idx = pWgt->link.childsCnt -1;
        if (idx >= pWgt->link.childsCnt) idx = 0;

        g.pWndState->onPageControlPageChange(pWgt, idx);
        g.pWndState->invalidate(pWgt->id);
    }
}

static void processMouse_ListBox(const Widget *pWgt, const Rect &wgtRect, const KeyCode &kc)
{
    moveTo(wgtRect.coord.col+1, kc.mouse.row);

    if (kc.mouse.btn == MouseBtn::ButtonLeft)
    {
        changeFocusTo(pWgt->id);
    }
    else if (kc.mouse.btn == MouseBtn::WheelUp || kc.mouse.btn == MouseBtn::WheelDown)
    {
        int idx, cnt;
        g.pWndState->getListBoxState(pWgt, idx, cnt);

        if (kc.mouse.btn == MouseBtn::WheelUp)
        {
            if (--g.listboxHighlightIdx < 0)
                g.listboxHighlightIdx = cnt - 1;
        }
        else
        {
            if (++g.listboxHighlightIdx == cnt)
                g.listboxHighlightIdx = 0;
        }

        g.pWndState->invalidate(pWgt->id);
    }
}

static void processMouse_DropDownList(const Widget *pWgt, const Rect &wgtRect, const KeyCode &kc)
{
    moveTo(wgtRect.coord.col, kc.mouse.row);

    if (kc.mouse.btn == MouseBtn::ButtonLeft)
    {
        changeFocusTo(pWgt->id);
    }
}

static bool processMouse(const KeyCode &kc)
{
    Rect rct;
    const Widget *p_wgt = getWidgetAt(kc.mouse.col, kc.mouse.row, rct);

    if (!p_wgt)
        return false;

    //TWINS_LOG("WidgetAt(%2d:%2d)=%s ID:%u", kc.mouse.col, kc.mouse.row, toString(p_wgt->type), p_wgt->id);

    switch (p_wgt->type)
    {
        case Widget::Edit:
            processMouse_Edit(p_wgt, rct, kc);
            break;
        case Widget::CheckBox:
            processMouse_CheckBox(p_wgt, rct, kc);
            break;
        case Widget::Radio:
            processMouse_Radio(p_wgt, rct, kc);
            break;
        case Widget::Button:
            processMouse_Button(p_wgt, rct, kc);
            break;
        case Widget::PageCtrl:
            processMouse_PageCtrl(p_wgt, rct, kc);
            break;
        case Widget::ListBox:
            processMouse_ListBox(p_wgt, rct, kc);
            break;
        case Widget::DropDownList:
            processMouse_DropDownList(p_wgt, rct, kc);
            break;
        default:
            moveToHome();
            return false;
    }

    return true;
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
    // moveTo(g.focusedCursorPos.col, g.focusedCursorPos.row);
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
    // moveTo(g.focusedCursorPos.col, g.focusedCursorPos.row);
    cursorShow();
}

// -----------------------------------------------------------------------------

bool processKey(const Widget *pWindowArray, const KeyCode &kc)
{
    assert(pWindowArray);
    assert(pWindowArray->type == Widget::Window);
    g.pWndArray = pWindowArray;
    g.pWndState = pWindowArray->window.getState();
    assert(g.pWndState);
    bool key_processed = false;

    if (kc.key == Key::None)
        return true;

    //TWINS_LOG("---");

    if (kc.key == Key::MouseEvent)
    {
        key_processed = processMouse(kc);
    }
    else
    {
        key_processed = processKey(kc);

        if (!key_processed && kc.m_spec)
        {
            switch (kc.key)
            {
            case Key::Esc:
            {
                auto curr_id = g.pWndState->getFocusedID();
                auto new_id = getParentToFocus(curr_id);
                key_processed = changeFocusTo(new_id);
                break;
            }
            case Key::Tab:
            {
                auto curr_id = g.pWndState->getFocusedID();
                auto new_id = getNextToFocus(curr_id, !kc.m_shift);
                key_processed = changeFocusTo(new_id);
                break;
            }
            case Key::PgUp:
            case Key::PgDown:
            {
                // Ctrl+PgUp/PgDown will be directed to window's first PageControl widget
                if (kc.m_ctrl)
                {
                    for (unsigned i = 0; i < g.pWndArray[0].link.childsCnt; i++)
                    {
                        const auto *p_wgt = &g.pWndArray[g.pWndArray[0].link.childsIdx + i];

                        if (p_wgt->type == Widget::PageCtrl)
                        {
                            processKey_PageCtrl(p_wgt, kc);
                            key_processed = true;
                            break;
                        }
                    }
                }
                break;
            }
            default:
                break;
            }
        }
    }

    return key_processed;
}

// -----------------------------------------------------------------------------

}
