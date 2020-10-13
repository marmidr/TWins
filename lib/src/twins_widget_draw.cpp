/******************************************************************************
 * @brief   TWins - widget drawing
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#include "twins_widget_prv.hpp"
#include "twins_utils.hpp"

#include <assert.h>
#include <functional>

// -----------------------------------------------------------------------------

// trick that can triple the interface drawing speed
#ifndef TWINS_FAST_FILL
# define TWINS_FAST_FILL    1
#endif

namespace twins
{

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

const char * const frame_listbox[] =
{
    "┌", "─", "┐",
    "│", " ", "▒",
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
// ---- TWINS PRIVATE FUNCTIONS ------------------------------------------------
// -----------------------------------------------------------------------------

static ColorBG getWidgetBgColor(const Widget *pWgt)
{
    if (!pWgt)
        return ColorBG::Default;

    switch (pWgt->type)
    {
    case Widget::Window:
        // this is terminating case
        return pWgt->window.bgColor;
    case Widget::Panel:
        if (pWgt->panel.bgColor != ColorBG::Inherit)
            return pWgt->panel.bgColor;
        break;
    case Widget::Label:
        if (pWgt->label.bgColor != ColorBG::Inherit)
            return pWgt->label.bgColor;
        break;
    case Widget::Edit:
        if (pWgt->edit.bgColor != ColorBG::Inherit)
            return pWgt->edit.bgColor;
        break;
    case Widget::Button:
        if (pWgt->button.bgColor != ColorBG::Inherit)
            return pWgt->button.bgColor;
        break;
    case Widget::ListBox:
        if (pWgt->listbox.bgColor != ColorBG::Inherit)
            return pWgt->listbox.bgColor;
        break;
    case Widget::ComboBox:
        if (pWgt->combobox.bgColor != ColorBG::Inherit)
            return pWgt->combobox.bgColor;
    default:
        break;
    }

    return getWidgetBgColor(g_ws.pWndWidgets + pWgt->link.parentIdx);
}

static ColorFG getWidgetFgColor(const Widget *pWgt)
{
    if (!pWgt)
        return ColorFG::Default;

    switch (pWgt->type)
    {
    case Widget::Window:
        // this is terminating case
        return pWgt->window.fgColor;
    case Widget::Panel:
        if (pWgt->panel.fgColor != ColorFG::Inherit)
            return pWgt->panel.fgColor;
        break;
    case Widget::Label:
        if (pWgt->label.fgColor != ColorFG::Inherit)
            return pWgt->label.fgColor;
        break;
    case Widget::Edit:
        if (pWgt->edit.fgColor != ColorFG::Inherit)
            return pWgt->edit.fgColor;
        break;
    case Widget::CheckBox:
        if (pWgt->checkbox.fgColor != ColorFG::Inherit)
            return pWgt->checkbox.fgColor;
        break;
    case Widget::Radio:
        if (pWgt->radio.fgColor != ColorFG::Inherit)
            return pWgt->radio.fgColor;
        break;
    case Widget::Button:
        if (pWgt->button.fgColor != ColorFG::Inherit)
            return pWgt->button.fgColor;
        break;
    case Widget::Led:
        if (pWgt->led.fgColor != ColorFG::Inherit)
            return pWgt->led.fgColor;
        break;
    case Widget::ProgressBar:
        if (pWgt->progressbar.fgColor != ColorFG::Inherit)
            return pWgt->progressbar.fgColor;
        break;
    case Widget::ListBox:
        if (pWgt->listbox.fgColor != ColorFG::Inherit)
            return pWgt->listbox.fgColor;
        break;
    case Widget::ComboBox:
        if (pWgt->combobox.fgColor != ColorFG::Inherit)
            return pWgt->combobox.fgColor;
        break;
    default:
        break;
    }

    return getWidgetFgColor(g_ws.pWndWidgets + pWgt->link.parentIdx);
}

static void drawArea(const Coord coord, const Size size, ColorBG clBg, ColorFG clFg, const FrameStyle style, bool filled = true, bool shadow = false)
{
    moveTo(coord.col, coord.row);

    const char * const * frame = frame_none;
    switch (style)
    {
    case FrameStyle::Single:    frame = frame_single; break;
    case FrameStyle::Double:    frame = frame_double; break;
    case FrameStyle::PgControl: frame = frame_pgcontrol; break;
    case FrameStyle::ListBox:   frame = frame_listbox; break;
    default: break;
    }

    // background and frame color
    if (clBg != ColorBG::Inherit) pushClBg(clBg);
    if (clFg != ColorFG::Inherit) pushClFg(clFg);

    // top line
    g_ws.str.clear();
    g_ws.str.append(frame[0]);
#if TWINS_FAST_FILL
    g_ws.str.append(frame[1]);
    g_ws.str.appendFmt(ESC_REPEAT_LAST_CHAR_FMT, size.width - 3);
#else
    g_ws.str.append(frame[1], size.width - 2);
#endif
    g_ws.str.append(frame[2]);
    writeStrLen(g_ws.str.cstr(), g_ws.str.size());
    moveBy(-size.width, 1);
    flushBuffer();

    // lines in the middle
    g_ws.str.clear();
    g_ws.str.append(frame[3]);
    if (filled)
    {
    #if TWINS_FAST_FILL
        g_ws.str.append(frame[4]);
        g_ws.str.appendFmt(ESC_REPEAT_LAST_CHAR_FMT, size.width - 3);
    #else
        g_ws.str.append(frame[4], size.width - 2);
    #endif
    }
    else
    {
        g_ws.str.appendFmt(ESC_CURSOR_FORWARD_FMT, size.width - 2);
    }
    g_ws.str.append(frame[5]);
    if (shadow)
    {
        // trailing shadow
        g_ws.str << ESC_FG_BLACK;
        g_ws.str << "█";
        g_ws.str << encodeCl(clFg);
    }

    for (int r = coord.row + 1; r < coord.row + size.height - 1; r++)
    {
        writeStrLen(g_ws.str.cstr(), g_ws.str.size());
        moveBy(-(size.width + shadow), 1);
        flushBuffer();
    }

    // bottom line
    g_ws.str.clear();
    g_ws.str.append(frame[6]);
#if TWINS_FAST_FILL
    g_ws.str.append(frame[7]);
    g_ws.str.appendFmt(ESC_REPEAT_LAST_CHAR_FMT, size.width - 3);
#else
    g_ws.str.append(frame[7], size.width - 2);
#endif
    g_ws.str.append(frame[8]);
    if (shadow)
    {
        // trailing shadow
        g_ws.str << ESC_FG_BLACK;
        g_ws.str << "█";
    }
    writeStrLen(g_ws.str.cstr(), g_ws.str.size());
    flushBuffer();

    if (shadow)
    {
        moveBy(-size.width, 1);
        g_ws.str.clear();
        // trailing shadow
        // g_ws.str = ESC_FG_BLACK;
    #if TWINS_FAST_FILL
        g_ws.str.append("█");
        g_ws.str.appendFmt(ESC_REPEAT_LAST_CHAR_FMT, size.width - 1);
    #else
        g_ws.str.append("█", size.width);
    #endif
        writeStrLen(g_ws.str.cstr(), g_ws.str.size());
        writeStr(encodeCl(clFg));
        flushBuffer();
    }

    // here the Fg and Bg colors are not restored
}

static void drawListScrollBarV(const Coord coord, int height, int max, int pos)
{
    if (pos > max)
    {
        // TWINS_LOG("pos (%d) > max (%d)", pos, max);
        return;
    }

    const int slider_at = ((height-1) * pos) / max;
    // "▲▴ ▼▾ ◄◂ ►▸ ◘ █";

    for (int i = 0; i < height; i++)
    {
        moveTo(coord.col, coord.row + i);
        writeStr(i == slider_at ? "◘" : "▒");
    }
}

static void drawWindow(const Widget *pWgt)
{
    Coord wnd_coord = pWgt->coord;
    g_ws.parentCoord = {0, 0};
    g_ws.pWndState->getWindowCoord(pWgt, wnd_coord);

    drawArea(wnd_coord, pWgt->size,
        pWgt->window.bgColor, pWgt->window.fgColor, FrameStyle::Double, true, pWgt->window.isPopup);

    // title
    String wnd_title;
    if (pWgt->window.title)
        wnd_title << pWgt->window.title;
    else
        g_ws.pWndState->getWindowTitle(pWgt, wnd_title);

    if (wnd_title.size())
    {
        auto capt_len = wnd_title.u8len(true);
        moveTo(wnd_coord.col + (pWgt->size.width - capt_len - 4)/2, wnd_coord.row);
        pushAttr(FontAttrib::Bold);
        writeStrFmt("╡ %s ╞", wnd_title.cstr());
        popAttr();
    }

    flushBuffer();
    g_ws.parentCoord = wnd_coord;

    for (int i = pWgt->link.childsIdx; i < pWgt->link.childsIdx + pWgt->link.childsCnt; i++)
        drawWidgetInternal(&g_ws.pWndWidgets[i]);

    // reset colors set by frame drawer
    popClBg();
    popClFg();
    moveTo(0, wnd_coord.row + pWgt->size.height);
}

static void drawPanel(const Widget *pWgt)
{
    FontMemento _m;
    const auto my_coord = g_ws.parentCoord + pWgt->coord;

    drawArea(my_coord, pWgt->size,
        pWgt->panel.bgColor, pWgt->panel.fgColor,
        pWgt->panel.noFrame ? FrameStyle::None : FrameStyle::Single);
    flushBuffer();

    // title
    if (pWgt->panel.title)
    {
        auto capt_len = utf8len(pWgt->panel.title);
        moveTo(my_coord.col + (pWgt->size.width - capt_len - 2)/2, my_coord.row);
        pushAttr(FontAttrib::Bold);
        writeStrFmt(" %s ", pWgt->panel.title);
        popAttr();
    }

    flushBuffer();
    auto coord_bkp = g_ws.parentCoord;
    g_ws.parentCoord = my_coord;

    for (int i = pWgt->link.childsIdx; i < pWgt->link.childsIdx + pWgt->link.childsCnt; i++)
        drawWidgetInternal(&g_ws.pWndWidgets[i]);

    g_ws.parentCoord = coord_bkp;
}

static void drawLabel(const Widget *pWgt)
{
    g_ws.str.clear();

    // label text
    if (pWgt->label.text)
        g_ws.str = pWgt->label.text;
    else
        g_ws.pWndState->getLabelText(pWgt, g_ws.str);

    // setup colors
    pushClFg(getWidgetFgColor(pWgt));
    pushClBg(getWidgetBgColor(pWgt));

    // print all lines
    const char *p_line = g_ws.str.cstr();
    String s_line;
    moveTo(g_ws.parentCoord.col + pWgt->coord.col, g_ws.parentCoord.row + pWgt->coord.row);

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

        s_line.setLength(pWgt->size.width, true, true);
        writeStrLen(s_line.cstr(), s_line.size());
        flushBuffer();
        moveBy(-pWgt->size.width, 1);
    }

    // restore colors
    popClFg();
    popClBg();
}

static void drawEdit(const Widget *pWgt)
{
    g_ws.str.clear();
    int16_t display_pos = 0;
    const int16_t max_w = pWgt->size.width-3;

    if (pWgt == g_ws.editState.pWgt)
    {
        // in edit mode; similar calculation in setCursorAt()
        g_ws.str = g_ws.editState.str;
        auto cursor_pos = g_ws.editState.cursorPos;
        auto delta = (max_w/2);

        while (cursor_pos >= max_w-1)
        {
            cursor_pos -= delta;
            display_pos += delta;
        }
    }
    else
    {
        g_ws.pWndState->getEditText(pWgt, g_ws.str);
    }

    const int txt_len = g_ws.str.u8len();

    if (display_pos > 0)
    {
        auto *str_beg = String::u8skipIgnoreEsc(g_ws.str.cstr(), display_pos + 1);
        String s("◁");
        s << str_beg;
        g_ws.str = std::move(s);
    }

    if (display_pos + max_w <= txt_len)
    {
        g_ws.str.setLength(pWgt->size.width-3-1);
        g_ws.str.append("▷");
    }
    else
    {
        g_ws.str.setLength(pWgt->size.width-3);
    }
    g_ws.str.append("[^]");

    bool focused = g_ws.pWndState->isFocused(pWgt);
    auto clbg = getWidgetBgColor(pWgt);
    intensifyClIf(focused, clbg);

    moveTo(g_ws.parentCoord.col + pWgt->coord.col, g_ws.parentCoord.row + pWgt->coord.row);
    pushClBg(clbg);
    pushClFg(getWidgetFgColor(pWgt));
    writeStrLen(g_ws.str.cstr(), g_ws.str.size());
    popClFg();
    popClBg();
}

static void drawLed(const Widget *pWgt)
{
    auto clbg = g_ws.pWndState->getLedLit(pWgt) ? pWgt->led.bgColorOn : pWgt->led.bgColorOff;
    g_ws.str.clear();

    if (pWgt->led.text)
        g_ws.str = pWgt->led.text;
    else
        g_ws.pWndState->getLedText(pWgt, g_ws.str);

    // led text
    moveTo(g_ws.parentCoord.col + pWgt->coord.col, g_ws.parentCoord.row + pWgt->coord.row);
    pushClBg(clbg);
    pushClFg(getWidgetFgColor(pWgt));
    writeStrLen(g_ws.str.cstr(), g_ws.str.size());
    popClFg();
    popClBg();
}

static void drawCheckbox(const Widget *pWgt)
{
    const char *s_chk_state = g_ws.pWndState->getCheckboxChecked(pWgt) ? "[■] " : "[ ] ";
    bool focused = g_ws.pWndState->isFocused(pWgt);
    auto clfg = getWidgetFgColor(pWgt);
    intensifyClIf(focused, clfg);

    moveTo(g_ws.parentCoord.col + pWgt->coord.col, g_ws.parentCoord.row + pWgt->coord.row);
    if (focused) pushAttr(FontAttrib::Bold);
    pushClFg(clfg);
    writeStr(s_chk_state);
    writeStr(pWgt->checkbox.text);
    popClFg();
    if (focused) popAttr();
}

static void drawRadio(const Widget *pWgt)
{
    const char *s_radio_state = pWgt->radio.radioId == g_ws.pWndState->getRadioIndex(pWgt) ? "(●) " : "( ) ";
    bool focused = g_ws.pWndState->isFocused(pWgt);
    auto clfg = getWidgetFgColor(pWgt);
    intensifyClIf(focused, clfg);

    moveTo(g_ws.parentCoord.col + pWgt->coord.col, g_ws.parentCoord.row + pWgt->coord.row);
    if (focused) pushAttr(FontAttrib::Bold);
    pushClFg(clfg);
    writeStr(s_radio_state);
    writeStr(pWgt->radio.text);
    popClFg();
    if (focused) popAttr();
}

static void drawButton(const Widget *pWgt)
{
    const bool focused = g_ws.pWndState->isFocused(pWgt);
    const bool pressed = pWgt == g_ws.pMouseDownWgt;
    auto clfg = getWidgetFgColor(pWgt);
    intensifyClIf(focused, clfg);

    if (pWgt->button.style == ButtonStyle::Simple)
    {
        FontMemento _m;
        g_ws.str.clear();
        g_ws.str.append(focused ? "[<" : "[ ");
        g_ws.str.append(pWgt->button.text);
        g_ws.str.append(focused ? ">]" : " ]");

        moveTo(g_ws.parentCoord.col + pWgt->coord.col, g_ws.parentCoord.row + pWgt->coord.row);
        if (focused) pushAttr(FontAttrib::Bold);
        if (pressed) pushAttr(FontAttrib::Inverse);
        pushClFg(clfg);
        writeStrLen(g_ws.str.cstr(), g_ws.str.size());
    }
    else
    {
        {
            FontMemento _m;
            g_ws.str.clear();
            g_ws.str << " " << pWgt->button.text << " ";

            moveTo(g_ws.parentCoord.col + pWgt->coord.col, g_ws.parentCoord.row + pWgt->coord.row);
            if (focused) pushAttr(FontAttrib::Bold);
            if (pressed) pushAttr(FontAttrib::Inverse);
            pushClBg(getWidgetBgColor(pWgt));
            pushClFg(clfg);
            writeStrLen(g_ws.str.cstr(), g_ws.str.size());
        }

        auto shadow_len = 2 + String::u8len(pWgt->button.text, nullptr, true);

        if (pressed)
        {
            // erase trailing shadow
            pushClBg(getWidgetBgColor(getParent(pWgt)));
            writeStr(" ");
            // erase shadow below
            moveTo(g_ws.parentCoord.col + pWgt->coord.col + 1, g_ws.parentCoord.row + pWgt->coord.row + 1);
            writeStr(" ", shadow_len);
            popClBg();
        }
        else
        {
            // trailing shadow
            pushClBg(getWidgetBgColor(getParent(pWgt)));
            pushClFg(ColorFG::Black);
            writeStr("▄");

            // shadow below
            moveTo(g_ws.parentCoord.col + pWgt->coord.col + 1, g_ws.parentCoord.row + pWgt->coord.row + 1);
            writeStr("▀", shadow_len);
            popClFg();
            popClBg();
        }
    }
}

static void drawPageControl(const Widget *pWgt)
{
    const auto my_coord = g_ws.parentCoord + pWgt->coord;

    pushClBg(getWidgetBgColor(pWgt));
    pushClFg(getWidgetFgColor(pWgt));
    drawArea(my_coord + Coord{pWgt->pagectrl.tabWidth, 0}, pWgt->size - Size{pWgt->pagectrl.tabWidth, 0},
        ColorBG::Inherit, ColorFG::Inherit, FrameStyle::PgControl);
    flushBuffer();

    auto coord_bkp = g_ws.parentCoord;
    g_ws.parentCoord = my_coord;
    // tabs title
    g_ws.str.clear();
    g_ws.str.append(' ', (pWgt->pagectrl.tabWidth-8) / 2);
    g_ws.str.append("≡ MENU ≡");
    g_ws.str.setLength(pWgt->pagectrl.tabWidth);
    moveTo(my_coord.col, my_coord.row);
    pushAttr(FontAttrib::Inverse);
    writeStrLen(g_ws.str.cstr(), g_ws.str.size());
    popAttr();

    // draw tabs and pages
    const int pg_idx = g_ws.pWndState->getPageCtrlPageIndex(pWgt);
    // const bool focused = g_ws.pWndState->isFocused(pWgt);
    moveTo(g_ws.parentCoord.col + pWgt->coord.col, g_ws.parentCoord.row + pWgt->coord.row);
    flushBuffer();

    for (int i = 0; i < pWgt->link.childsCnt; i++)
    {
        if (i == pWgt->size.height-1)
            break;

        const auto *p_page = &g_ws.pWndWidgets[pWgt->link.childsIdx + i];

        // draw page title
        g_ws.str.clear();
        g_ws.str.appendFmt("%s%s", i == pg_idx ? "►" : " ", p_page->page.title);
        g_ws.str.setLength(pWgt->pagectrl.tabWidth, true, true);

        moveTo(my_coord.col, my_coord.row + i + 1);

        // for Page we do not want inherit after it's title color
        auto clfg = p_page->page.fgColor;
        if (clfg == ColorFG::Inherit)
            clfg = getWidgetFgColor(p_page);

        pushClFg(clfg);
        if (i == pg_idx) pushAttr(FontAttrib::Inverse);
        writeStrLen(g_ws.str.cstr(), g_ws.str.size());
        if (i == pg_idx) popAttr();
        popClFg();

        if (g_ws.pWndState->isVisible(p_page))
        {
            flushBuffer();
            g_ws.parentCoord.col += pWgt->pagectrl.tabWidth;
            drawWidgetInternal(p_page);
            g_ws.parentCoord.col -= pWgt->pagectrl.tabWidth;
        }
    }

    popClBg();
    popClFg();
    g_ws.parentCoord = coord_bkp;
}

static void drawPage(const Widget *pWgt)
{
    // draw childrens
    for (int i = pWgt->link.childsIdx; i < pWgt->link.childsIdx + pWgt->link.childsCnt; i++)
        drawWidgetInternal(&g_ws.pWndWidgets[i]);
}

static void drawProgressBar(const Widget *pWgt)
{
    const char* style_data[][2] =
    {
        {"#", "."},
        {"█", "▒"},
        {"■", "□"}
    };

    int32_t pos = 0, max = 1;
    auto style = (short)pWgt->progressbar.style;
    g_ws.pWndState->getProgressBarState(pWgt, pos, max);

    if (max <= 0) max = 1;
    if (pos > max) pos = max;

    moveTo(g_ws.parentCoord.col + pWgt->coord.col, g_ws.parentCoord.row + pWgt->coord.row);
    g_ws.str.clear();
    int fill = pos * pWgt->size.width / max;
    g_ws.str.append(style_data[style][0], fill);
    g_ws.str.append(style_data[style][1], pWgt->size.width - fill);

    pushClFg(getWidgetFgColor(pWgt));
    writeStrLen(g_ws.str.cstr(), g_ws.str.size());
    popClFg();

    // ████░░░░░░░░░░░
    // [####.........]
    // [■■■■□□□□□□□□□]
    //  ▁▂▃▄▅▆▇█ - for vertical ▂▄▆█
}

struct DrawListParams
{
    Coord coord;
    int16_t item_idx;
    int16_t sel_idx;
    int16_t items_cnt;
    uint16_t items_visible;
    uint16_t top_item;
    bool focused;
    uint8_t wgt_width;
    uint8_t frame_size;
    std::function<void(int16_t idx, String &out)> getItem;
};

static void drawList(DrawListParams &p)
{
    if (p.items_cnt > p.items_visible)
    {
        drawListScrollBarV(p.coord + Size{uint8_t(p.wgt_width-1), p.frame_size},
            p.items_visible, p.items_cnt-1, p.sel_idx);
    }

    flushBuffer();

    for (int i = 0; i < p.items_visible; i++)
    {
        bool is_current_item = p.top_item + i == p.item_idx;
        bool is_sel_item = p.top_item + i == p.sel_idx;
        moveTo(p.coord.col + p.frame_size, p.coord.row + i + p.frame_size);

        g_ws.str.clear();

        if (p.top_item + i < p.items_cnt)
        {
            p.getItem(p.top_item + i, g_ws.str);
            g_ws.str.insert(0, is_current_item ? "►" : " ");
            g_ws.str.setLength(p.wgt_width - 1 - p.frame_size, true, true);
        }
        else
        {
            // empty string - to erase old content
            g_ws.str.setLength(p.wgt_width - 1 - p.frame_size);
        }

        if (p.focused && is_sel_item) pushAttr(FontAttrib::Inverse);
        writeStrLen(g_ws.str.cstr(), g_ws.str.size());
        if (p.focused && is_sel_item) popAttr();
    }
};

static void drawListBox(const Widget *pWgt)
{
    FontMemento _m;
    const auto my_coord = g_ws.parentCoord + pWgt->coord;
    drawArea(my_coord, pWgt->size,
        pWgt->listbox.bgColor, pWgt->listbox.fgColor,
        pWgt->listbox.noFrame ? FrameStyle::None : FrameStyle::ListBox, false);

    if (pWgt->size.height < 3)
        return;

    DrawListParams dlp = {};
    dlp.coord = my_coord;
    g_ws.pWndState->getListBoxState(pWgt, dlp.item_idx, dlp.sel_idx, dlp.items_cnt);
    dlp.frame_size = !pWgt->listbox.noFrame;
    dlp.items_visible = pWgt->size.height - (dlp.frame_size * 2);
    dlp.top_item = (dlp.sel_idx / dlp.items_visible) * dlp.items_visible;
    dlp.focused = g_ws.pWndState->isFocused(pWgt);
    dlp.wgt_width = pWgt->size.width;
    dlp.getItem = [pWgt](int16_t idx, String &out) { g_ws.pWndState->getListBoxItem(pWgt, idx, out); };
    drawList(dlp);
}

static void drawComboBox(const Widget *pWgt)
{
    FontMemento _m;
    const auto my_coord = g_ws.parentCoord + pWgt->coord;
    const bool focused = g_ws.pWndState->isFocused(pWgt);

    int16_t item_idx = 0; int16_t sel_idx = 0; int16_t items_count; bool drop_down = false;
    g_ws.pWndState->getComboBoxState(pWgt, item_idx, sel_idx, items_count, drop_down);

    {
        g_ws.str.clear();
        g_ws.pWndState->getComboBoxItem(pWgt, item_idx, g_ws.str);
        g_ws.str.insert(0, " ");
        g_ws.str.setLength(pWgt->size.width - 4, true, true);
        g_ws.str << " [▼]";

        moveTo(my_coord.col, my_coord.row);
        pushClFg(getWidgetFgColor(pWgt));
        pushClBg(getWidgetBgColor(pWgt));
        if (focused && !drop_down) pushAttr(FontAttrib::Inverse);
        if (drop_down) pushAttr(FontAttrib::Underline);
        writeStrLen(g_ws.str.cstr(), g_ws.str.size());
        if (focused && !drop_down) popAttr();
        if (drop_down) popAttr();
    }

    if (drop_down)
    {
        DrawListParams dlp = {};
        dlp.coord.col = my_coord.col;
        dlp.coord.row = my_coord.row+1;
        dlp.item_idx = item_idx;
        dlp.sel_idx = sel_idx;
        dlp.items_cnt = items_count;
        dlp.frame_size = 0;
        dlp.items_visible = pWgt->combobox.dropDownSize;
        dlp.top_item = (dlp.sel_idx / dlp.items_visible) * dlp.items_visible;
        dlp.focused = focused;
        dlp.wgt_width = pWgt->size.width;
        dlp.getItem = [pWgt](int16_t idx, String &out) { g_ws.pWndState->getComboBoxItem(pWgt, idx, out); };
        drawList(dlp);
    }
}

static void drawCustomWgt(const Widget *pWgt)
{
    g_ws.pWndState->onCustomWidgetDraw(pWgt);
}

static void drawTextBox(const Widget *pWgt)
{
    FontMemento _m;
    const auto my_coord = g_ws.parentCoord + pWgt->coord;

    drawArea(my_coord, pWgt->size,
        pWgt->textbox.bgColor, pWgt->textbox.fgColor,
        FrameStyle::ListBox, false, false);

    if (pWgt->size.height < 3)
        return;

    const uint8_t lines_visible = pWgt->size.height - 2;
    const twins::Vector<twins::StringRange> *p_lines = nullptr;
    int16_t top_line = 0;

    g_ws.pWndState->getTextBoxState(pWgt, &p_lines, top_line);

    if (!p_lines || !p_lines->size())
        return;

    if (top_line > (int)p_lines->size())
    {
        top_line = p_lines->size() - lines_visible;
        g_ws.pWndState->onTextBoxScroll(pWgt, top_line);
    }

    if (top_line < 0)
    {
        g_ws.pWndState->onTextBoxScroll(pWgt, top_line);
        top_line = 0;
    }

    drawListScrollBarV(my_coord + Size{uint8_t(pWgt->size.width-1), 1},
        lines_visible, p_lines->size() - lines_visible, top_line);

    flushBuffer();

    // scan invisible lines for ESC sequences: colors, font attributes
    g_ws.str.clear();
    for (int i = 0; i < top_line; i++)
    {
        auto sr = (*p_lines)[i];
        while (const char *esc = twins::util::strnchr(sr.data, sr.size, '\e'))
        {
            auto esclen = String::escLen(esc, sr.data + sr.size);
            g_ws.str.appendLen(esc, esclen);

            sr.size -= esc - sr.data + 1;
            sr.data = esc + 1;
        }
    }
    writeStrLen(g_ws.str.cstr(), g_ws.str.size());

    // draw lines
    for (int i = 0; i < lines_visible; i++)
    {
        if (top_line + i >= (int)p_lines->size())
            break;

        const auto &sr = (*p_lines)[top_line + i];
        g_ws.str.clear();
        g_ws.str.appendLen(sr.data, sr.size);
        g_ws.str.setLength(pWgt->size.width - 2, true, true);
        moveTo(my_coord.col + 1, my_coord.row + i + 1);
        writeStrLen(g_ws.str.cstr(), g_ws.str.size());
    }

    flushBuffer();
}

// -----------------------------------------------------------------------------

static void drawWidgetInternal(const Widget *pWgt)
{
    if (!g_ws.pWndState->isVisible(pWgt))
        return;

    bool en = g_ws.pWndState->isEnabled(pWgt);
    if (!en) pushAttr(FontAttrib::Faint);

    switch (pWgt->type)
    {
    case Widget::Window:        drawWindow(pWgt); break;
    case Widget::Panel:         drawPanel(pWgt); break;
    case Widget::Label:         drawLabel(pWgt); break;
    case Widget::Edit:          drawEdit(pWgt); break;
    case Widget::CheckBox:      drawCheckbox(pWgt); break;
    case Widget::Radio:         drawRadio(pWgt);  break;
    case Widget::Button:        drawButton(pWgt); break;
    case Widget::Led:           drawLed(pWgt); break;
    case Widget::PageCtrl:      drawPageControl(pWgt); break;
    case Widget::Page:          drawPage(pWgt); break;
    case Widget::ProgressBar:   drawProgressBar(pWgt); break;
    case Widget::ListBox:       drawListBox(pWgt); break;;
    case Widget::ComboBox:      drawComboBox(pWgt); break;
    case Widget::CustomWgt:     drawCustomWgt(pWgt); break;
    case Widget::TextBox:       drawTextBox(pWgt); break;
    default:                    break;
    }

    if (!en)
        popAttr();

    flushBuffer();
}

// -----------------------------------------------------------------------------
// ---- TWINS  P U B L I C  FUNCTIONS ------------------------------------------
// -----------------------------------------------------------------------------

void drawWidget(const Widget *pWindowWidgets, WID widgetId)
{
    // bool glob_clear = !(g_ws.pWndArray || g_ws.pWndState);
    assert(pWindowWidgets);
    assert(pWindowWidgets->type == Widget::Window);
    g_ws.pWndWidgets = pWindowWidgets;
    g_ws.pWndState = pWindowWidgets->window.getState();
    assert(g_ws.pWndState);
    g_ws.pFocusedWgt = getWidgetByWID(g_ws.pWndState->getFocusedID());
    cursorHide();
    flushBuffer();

    if (widgetId == WIDGET_ID_ALL)
    {
        drawWidgetInternal(pWindowWidgets);
    }
    else
    {
        WidgetSearchStruct wss { searchedID : widgetId };

        // search window for widget by it's Id
        if (getWidgetWSS(wss) && wss.isVisible)
        {
            g_ws.parentCoord = wss.parentCoord;
            // set parent's background color
            pushClBg(getWidgetBgColor(wss.pWidget));
            drawWidgetInternal(wss.pWidget);
            popClBg();
        }
    }

    // if (glob_clear) g_ws.pWndArray = nullptr, g_ws.pWndState = nullptr;
    resetAttr();
    resetClBg();
    resetClFg();
    setCursorAt(g_ws.pFocusedWgt);
    cursorShow();
    flushBuffer();
}

void drawWidgets(const Widget *pWindowWidgets, const WID *pWidgetIds, uint16_t count)
{
    // bool glob_clear = !(g_ws.pWndArray || g_ws.pWndState);
    assert(pWindowWidgets);
    assert(pWindowWidgets->type == Widget::Window);
    g_ws.pWndWidgets = pWindowWidgets;
    g_ws.pWndState = pWindowWidgets->window.getState();
    assert(g_ws.pWndState);
    g_ws.pFocusedWgt = getWidgetByWID(g_ws.pWndState->getFocusedID());
    cursorHide();
    flushBuffer();

    for (unsigned i = 0; i < count; i++)
    {
        WidgetSearchStruct wss { searchedID : pWidgetIds[i] };

        if (getWidgetWSS(wss) && wss.isVisible)
        {
            g_ws.parentCoord = wss.parentCoord;
            // set parent's background color
            pushClBg(getWidgetBgColor(wss.pWidget));
            drawWidgetInternal(wss.pWidget);
            popClBg();
        }
    }

    // if (glob_clear) g_ws.pWndArray = nullptr, g_ws.pWndState = nullptr;
    resetAttr();
    resetClBg();
    resetClFg();
    setCursorAt(g_ws.pFocusedWgt);
    cursorShow();
    flushBuffer();
}

// -----------------------------------------------------------------------------

}
