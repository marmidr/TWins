/******************************************************************************
 * @brief   TWins - widget drawing
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#include "twins_widget_prv.hpp"
#include "twins_utils.hpp"

#include <assert.h>

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
    default:
        break;
    }

    return getWidgetBgColor(g.pWndWidgets + pWgt->link.parentIdx);
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
    default:
        break;
    }

    return getWidgetFgColor(g.pWndWidgets + pWgt->link.parentIdx);
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
    default: break;
    }

    // background and frame color
    if (clBg != ColorBG::Inherit) pushClBg(clBg);
    if (clFg != ColorFG::Inherit) pushClFg(clFg);

    // top line
    g.str.clear();
    g.str.append(frame[0]);
#if TWINS_FAST_FILL
    g.str.append(frame[1]);
    g.str.appendFmt(ESC_REPEAT_LAST_CHAR_FMT, size.width - 3);
#else
    g.str.append(frame[1], size.width - 2);
#endif
    g.str.append(frame[2]);
    writeStrLen(g.str.cstr(), g.str.size());
    moveBy(-size.width, 1);
    flushBuffer();

    // lines in the middle
    g.str.clear();
    g.str.append(frame[3]);
    if (filled)
    {
    #if TWINS_FAST_FILL
        g.str.append(frame[4]);
        g.str.appendFmt(ESC_REPEAT_LAST_CHAR_FMT, size.width - 3);
    #else
        g.str.append(frame[4], size.width - 2);
    #endif
    }
    else
    {
        g.str.appendFmt(ESC_CURSOR_FORWARD_FMT, size.width - 2);
    }
    g.str.append(frame[5]);
    if (shadow)
    {
        // trailing shadow
        g.str << ESC_FG_BLACK;
        g.str << "█";
        g.str << encodeCl(clFg);
    }

    for (int r = coord.row + 1; r < coord.row + size.height - 1; r++)
    {
        writeStrLen(g.str.cstr(), g.str.size());
        moveBy(-(size.width + shadow), 1);
        flushBuffer();
    }

    // bottom line
    g.str.clear();
    g.str.append(frame[6]);
#if TWINS_FAST_FILL
    g.str.append(frame[7]);
    g.str.appendFmt(ESC_REPEAT_LAST_CHAR_FMT, size.width - 3);
#else
    g.str.append(frame[7], size.width - 2);
#endif
    g.str.append(frame[8]);
    if (shadow)
    {
        // trailing shadow
        g.str << ESC_FG_BLACK;
        g.str << "█";
    }
    writeStrLen(g.str.cstr(), g.str.size());
    flushBuffer();

    if (shadow)
    {
        moveBy(-size.width, 1);
        g.str.clear();
        // trailing shadow
        // g.str = ESC_FG_BLACK;
    #if TWINS_FAST_FILL
        g.str.append("█");
        g.str.appendFmt(ESC_REPEAT_LAST_CHAR_FMT, size.width - 1);
    #else
        g.str.append("█", size.width);
    #endif
        writeStrLen(g.str.cstr(), g.str.size());
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
    g.parentCoord = {0, 0};
    g.pWndState->getWindowCoord(pWgt, wnd_coord);

    drawArea(wnd_coord, pWgt->size,
        pWgt->window.bgColor, pWgt->window.fgColor, FrameStyle::Double, true, pWgt->window.isPopup);

    // title
    String wnd_title;
    if (pWgt->window.title)
        wnd_title << pWgt->window.title;
    else
        g.pWndState->getWindowTitle(pWgt, wnd_title);

    if (wnd_title.size())
    {
        auto capt_len = wnd_title.u8len(true);
        moveTo(wnd_coord.col + (pWgt->size.width - capt_len - 4)/2, wnd_coord.row);
        pushAttr(FontAttrib::Bold);
        writeStrFmt("╡ %s ╞", wnd_title.cstr());
        popAttr();
    }

    flushBuffer();
    g.parentCoord = wnd_coord;

    for (int i = pWgt->link.childsIdx; i < pWgt->link.childsIdx + pWgt->link.childsCnt; i++)
        drawWidgetInternal(&g.pWndWidgets[i]);

    // reset colors set by frame drawer
    popClBg();
    popClFg();
    moveTo(0, wnd_coord.row + pWgt->size.height);
}

static void drawPanel(const Widget *pWgt)
{
    FontMemento _m;
    const auto my_coord = g.parentCoord + pWgt->coord;

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
    auto coord_bkp = g.parentCoord;
    g.parentCoord = my_coord;

    for (int i = pWgt->link.childsIdx; i < pWgt->link.childsIdx + pWgt->link.childsCnt; i++)
        drawWidgetInternal(&g.pWndWidgets[i]);

    g.parentCoord = coord_bkp;
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
    pushClFg(getWidgetFgColor(pWgt));
    pushClBg(getWidgetBgColor(pWgt));

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
    g.str.clear();
    int16_t display_pos = 0;
    const int16_t max_w = pWgt->size.width-3;

    if (g.editState.pWgt)
    {
        // in edit mode; similar calculation in setCursorAt()
        g.str = g.editState.str;
        auto cursor_pos = g.editState.cursorPos;
        auto delta = (max_w/2);

        while (cursor_pos >= max_w-1)
        {
            cursor_pos -= delta;
            display_pos += delta;
        }
    }
    else
    {
        g.pWndState->getEditText(pWgt, g.str);
    }

    const int txt_len = g.str.u8len();

    if (display_pos > 0)
    {
        auto *str_beg = String::u8skipIgnoreEsc(g.str.cstr(), display_pos + 1);
        String s("◁");
        s << str_beg;
        g.str = std::move(s);
    }

    if (display_pos + max_w <= txt_len)
    {
        g.str.setLength(pWgt->size.width-3-1);
        g.str.append("▷");
    }
    else
    {
        g.str.setLength(pWgt->size.width-3);
    }
    g.str.append("[^]");

    bool focused = g.pWndState->isFocused(pWgt);
    auto clbg = getWidgetBgColor(pWgt);
    intensifyClIf(focused, clbg);

    moveTo(g.parentCoord.col + pWgt->coord.col, g.parentCoord.row + pWgt->coord.row);
    pushClBg(clbg);
    pushClFg(getWidgetFgColor(pWgt));
    writeStrLen(g.str.cstr(), g.str.size());
    popClFg();
    popClBg();
}

static void drawLed(const Widget *pWgt)
{
    auto clbg = g.pWndState->getLedLit(pWgt) ? pWgt->led.bgColorOn : pWgt->led.bgColorOff;
    g.str.clear();

    if (pWgt->led.text)
        g.str = pWgt->led.text;
    else
        g.pWndState->getLedText(pWgt, g.str);

    // led text
    moveTo(g.parentCoord.col + pWgt->coord.col, g.parentCoord.row + pWgt->coord.row);
    pushClBg(clbg);
    pushClFg(getWidgetFgColor(pWgt));
    writeStrLen(g.str.cstr(), g.str.size());
    popClFg();
    popClBg();
}

static void drawCheckbox(const Widget *pWgt)
{
    const char *s_chk_state = g.pWndState->getCheckboxChecked(pWgt) ? "[■] " : "[ ] ";
    bool focused = g.pWndState->isFocused(pWgt);
    auto clfg = getWidgetFgColor(pWgt);
    intensifyClIf(focused, clfg);

    moveTo(g.parentCoord.col + pWgt->coord.col, g.parentCoord.row + pWgt->coord.row);
    if (focused) pushAttr(FontAttrib::Bold);
    pushClFg(clfg);
    writeStr(s_chk_state);
    writeStr(pWgt->checkbox.text);
    popClFg();
    if (focused) popAttr();
}

static void drawRadio(const Widget *pWgt)
{
    const char *s_radio_state = pWgt->radio.radioId == g.pWndState->getRadioIndex(pWgt) ? "(●) " : "( ) ";
    bool focused = g.pWndState->isFocused(pWgt);
    auto clfg = getWidgetFgColor(pWgt);
    intensifyClIf(focused, clfg);

    moveTo(g.parentCoord.col + pWgt->coord.col, g.parentCoord.row + pWgt->coord.row);
    if (focused) pushAttr(FontAttrib::Bold);
    pushClFg(clfg);
    writeStr(s_radio_state);
    writeStr(pWgt->radio.text);
    popClFg();
    if (focused) popAttr();
}

static void drawButton(const Widget *pWgt)
{
    const bool focused = g.pWndState->isFocused(pWgt);
    const bool pressed = pWgt == g.pMouseDownWgt;
    auto clfg = getWidgetFgColor(pWgt);
    intensifyClIf(focused, clfg);

    if (pWgt->button.style == ButtonStyle::Simple)
    {
        FontMemento _m;
        g.str.clear();
        g.str.append(focused ? "[<" : "[ ");
        g.str.append(pWgt->button.text);
        g.str.append(focused ? ">]" : " ]");

        moveTo(g.parentCoord.col + pWgt->coord.col, g.parentCoord.row + pWgt->coord.row);
        if (focused) pushAttr(FontAttrib::Bold);
        if (pressed) pushAttr(FontAttrib::Inverse);
        pushClFg(clfg);
        writeStrLen(g.str.cstr(), g.str.size());
    }
    else
    {
        {
            FontMemento _m;
            g.str.clear();
            g.str << " " << pWgt->button.text << " ";

            moveTo(g.parentCoord.col + pWgt->coord.col, g.parentCoord.row + pWgt->coord.row);
            if (focused) pushAttr(FontAttrib::Bold);
            if (pressed) pushAttr(FontAttrib::Inverse);
            pushClBg(getWidgetBgColor(pWgt));
            pushClFg(clfg);
            writeStrLen(g.str.cstr(), g.str.size());
        }

        auto shadow_len = 2 + String::u8len(pWgt->button.text, nullptr, true);

        if (pressed)
        {
            // erase trailing shadow
            pushClBg(getWidgetBgColor(getParent(pWgt)));
            writeStr(" ");
            // erase shadow below
            moveTo(g.parentCoord.col + pWgt->coord.col + 1, g.parentCoord.row + pWgt->coord.row + 1);
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
            moveTo(g.parentCoord.col + pWgt->coord.col + 1, g.parentCoord.row + pWgt->coord.row + 1);
            writeStr("▀", shadow_len);
            popClFg();
            popClBg();
        }
    }
}

static void drawPageControl(const Widget *pWgt)
{
    const auto my_coord = g.parentCoord + pWgt->coord;

    pushClBg(getWidgetBgColor(pWgt));
    pushClFg(getWidgetFgColor(pWgt));
    drawArea(my_coord + Coord{pWgt->pagectrl.tabWidth, 0}, pWgt->size - Size{pWgt->pagectrl.tabWidth, 0},
        ColorBG::Inherit, ColorFG::Inherit, FrameStyle::PgControl);
    flushBuffer();

    auto coord_bkp = g.parentCoord;
    g.parentCoord = my_coord;
    // tabs title
    g.str.clear();
    g.str.append(' ', (pWgt->pagectrl.tabWidth-8) / 2);
    g.str.append("≡ MENU ≡");
    g.str.setLength(pWgt->pagectrl.tabWidth);
    moveTo(my_coord.col, my_coord.row);
    pushAttr(FontAttrib::Inverse);
    writeStrLen(g.str.cstr(), g.str.size());
    popAttr();

    // draw tabs and pages
    const int pg_idx = g.pWndState->getPageCtrlPageIndex(pWgt);
    // const bool focused = g.pWndState->isFocused(pWgt);
    moveTo(g.parentCoord.col + pWgt->coord.col, g.parentCoord.row + pWgt->coord.row);
    flushBuffer();

    for (int i = 0; i < pWgt->link.childsCnt; i++)
    {
        if (i == pWgt->size.height-1)
            break;

        const auto *p_page = &g.pWndWidgets[pWgt->link.childsIdx + i];

        // draw page title
        g.str.clear();
        g.str.appendFmt("%s%s", i == pg_idx ? "►" : " ", p_page->page.title);
        g.str.setLength(pWgt->pagectrl.tabWidth, true, true);

        moveTo(my_coord.col, my_coord.row + i + 1);

        // for Page we do not want inherit after it's title color
        auto clfg = p_page->page.fgColor;
        if (clfg == ColorFG::Inherit)
            clfg = getWidgetFgColor(p_page);

        pushClFg(clfg);
        if (i == pg_idx) pushAttr(FontAttrib::Inverse);
        writeStrLen(g.str.cstr(), g.str.size());
        if (i == pg_idx) popAttr();
        popClFg();

        if (g.pWndState->isVisible(p_page))
        {
            flushBuffer();
            g.parentCoord.col += pWgt->pagectrl.tabWidth;
            drawWidgetInternal(p_page);
            g.parentCoord.col -= pWgt->pagectrl.tabWidth;
        }
    }

    popClBg();
    popClFg();
    g.parentCoord = coord_bkp;
}

static void drawPage(const Widget *pWgt)
{
    // draw childrens
    for (int i = pWgt->link.childsIdx; i < pWgt->link.childsIdx + pWgt->link.childsCnt; i++)
        drawWidgetInternal(&g.pWndWidgets[i]);
}

static void drawProgressBar(const Widget *pWgt)
{
    const char* style_data[][2] =
    {
        {"#", "."},
        {"█", "▒"},
        {"■", "□"}
    };

    int pos = 0, max = 1;
    auto style = (short)pWgt->progressbar.style;
    g.pWndState->getProgressBarState(pWgt, pos, max);

    if (max <= 0) max = 1;
    if (pos > max) pos = max;

    moveTo(g.parentCoord.col + pWgt->coord.col, g.parentCoord.row + pWgt->coord.row);
    g.str.clear();
    int fill = pos * pWgt->size.width / max;
    g.str.append(style_data[style][0], fill);
    g.str.append(style_data[style][1], pWgt->size.width - fill);

    pushClFg(getWidgetFgColor(pWgt));
    writeStrLen(g.str.cstr(), g.str.size());
    popClFg();

    // ████░░░░░░░░░░░
    // [####.........]
    // [■■■■□□□□□□□□□]
    //  ▁▂▃▄▅▆▇█ - for vertical ▂▄▆█
}

static void drawListBox(const Widget *pWgt)
{
    FontMemento _m;
    const auto my_coord = g.parentCoord + pWgt->coord;
    drawArea(my_coord, pWgt->size,
        pWgt->listbox.bgColor, pWgt->listbox.fgColor,
        pWgt->listbox.noFrame ? FrameStyle::None : FrameStyle::Single, false);

    if (pWgt->size.height < 3)
        return;

    int16_t idx = 0, selidx = 0, cnt = 0;
    const uint8_t frame_size = !pWgt->listbox.noFrame;
    g.pWndState->getListBoxState(pWgt, idx, selidx, cnt);

    const uint16_t items_visible = pWgt->size.height - (frame_size * 2);
    const uint16_t top_item = (selidx / items_visible) * items_visible;
    const bool focused = g.pWndState->isFocused(pWgt);

    if (cnt > items_visible)
    {
        drawListScrollBarV(my_coord + Size{uint8_t(pWgt->size.width-1), frame_size},
            items_visible, cnt-1, selidx);
    }
    flushBuffer();

    for (int i = 0; i < items_visible; i++)
    {
        bool is_current_item = top_item + i == idx;
        bool is_sel_item = top_item + i == selidx;
        moveTo(my_coord.col + frame_size, my_coord.row + i + frame_size);

        g.str.clear();

        if (top_item + i < cnt)
        {
            g.str.append(is_current_item ? "►" : " ");
            g.pWndState->getListBoxItem(pWgt, top_item + i, g.str);
            g.str.setLength(pWgt->size.width - 1 - frame_size, true, true);
        }
        else
        {
            // empty string - to erase old content
            g.str.setLength(pWgt->size.width - (frame_size * 2));
        }

        if (focused && is_sel_item) pushAttr(FontAttrib::Inverse);
        writeStrLen(g.str.cstr(), g.str.size());
        if (focused && is_sel_item) popAttr();
    }
}

static void drawDropDownList(const Widget *pWgt)
{
    // TODO:
}

static void drawCustomWgt(const Widget *pWgt)
{
    g.pWndState->onCustomWidgetDraw(pWgt);
}

static void drawTextBox(const Widget *pWgt)
{
    FontMemento _m;
    const auto my_coord = g.parentCoord + pWgt->coord;

    drawArea(my_coord, pWgt->size,
        pWgt->textbox.bgColor, pWgt->textbox.fgColor,
        FrameStyle::Single);

    if (pWgt->size.height < 3)
        return;

    const uint8_t lines_visible = pWgt->size.height - 2;
    const twins::Vector<twins::StringRange> *p_lines = nullptr;
    bool changed = false;
    g.pWndState->getTextBoxLines(pWgt, &p_lines, changed);
    if (!p_lines)
        return;

    if (changed)
    {
        if (g.textboxTopLine > (int)p_lines->size())
            g.textboxTopLine = p_lines->size() - lines_visible;
        if (g.textboxTopLine < 0)
            g.textboxTopLine = 0;
    }

    assert(g.textboxTopLine >= 0);

    drawListScrollBarV(my_coord + Size{uint8_t(pWgt->size.width-1), 1},
        lines_visible, p_lines->size() - lines_visible, g.textboxTopLine);

    flushBuffer();

    // scan invisible lines for ESC sequences: colors, font attributes
    g.str.clear();
    for (int i = 0; i < g.textboxTopLine; i++)
    {
        auto sr = (*p_lines)[i];
        while (const char *esc = twins::util::strnchr(sr.data, sr.size, '\e'))
        {
            auto esclen = String::escLen(esc, sr.data + sr.size);
            g.str.appendLen(esc, esclen);

            sr.size -= esc - sr.data + 1;
            sr.data = esc + 1;
        }
    }
    writeStrLen(g.str.cstr(), g.str.size());

    // draw lines
    for (int i = 0; i < lines_visible; i++)
    {
        if (g.textboxTopLine + i >= (int)p_lines->size())
            break;

        const auto &sr = (*p_lines)[g.textboxTopLine + i];
        g.str.clear();
        g.str.appendLen(sr.data, sr.size);
        g.str.setLength(pWgt->size.width - 2, true, true);
        moveTo(my_coord.col + 1, my_coord.row + i + 1);
        writeStrLen(g.str.cstr(), g.str.size());
    }

    flushBuffer();
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
    case Widget::Edit:          drawEdit(pWgt); break;
    case Widget::CheckBox:      drawCheckbox(pWgt); break;
    case Widget::Radio:         drawRadio(pWgt);  break;
    case Widget::Button:        drawButton(pWgt); break;
    case Widget::Led:           drawLed(pWgt); break;
    case Widget::PageCtrl:      drawPageControl(pWgt); break;
    case Widget::Page:          drawPage(pWgt); break;
    case Widget::ProgressBar:   drawProgressBar(pWgt); break;
    case Widget::ListBox:       drawListBox(pWgt); break;;
    case Widget::DropDownList:  drawDropDownList(pWgt); break;
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
    // bool glob_clear = !(g.pWndArray || g.pWndState);
    assert(pWindowWidgets);
    assert(pWindowWidgets->type == Widget::Window);
    g.pWndWidgets = pWindowWidgets;
    g.pWndState = pWindowWidgets->window.getState();
    assert(g.pWndState);
    cursorHide();
    flushBuffer();

    if (widgetId == WIDGET_ID_ALL)
    {
        if (g.pWndState->isVisible(pWindowWidgets))
            drawWidgetInternal(pWindowWidgets);
    }
    else
    {
        WidgetSearchStruct wss { searchedID : widgetId };

        // search window for widget by it's Id
        if (getWidgetWSS(wss) && wss.isVisible)
        {
            g.parentCoord = wss.parentCoord;
            // set parent's background color
            pushClBg(getWidgetBgColor(wss.pWidget));
            drawWidgetInternal(wss.pWidget);
            popClBg();
        }
    }

    // if (glob_clear) g.pWndArray = nullptr, g.pWndState = nullptr;
    resetAttr();
    resetClBg();
    resetClFg();
    setCursorAt(g.pFocusedWgt);
    cursorShow();
}

void drawWidgets(const Widget *pWindowWidgets, const WID *pWidgetIds, uint16_t count)
{
    // bool glob_clear = !(g.pWndArray || g.pWndState);
    assert(pWindowWidgets);
    assert(pWindowWidgets->type == Widget::Window);
    g.pWndWidgets = pWindowWidgets;
    g.pWndState = pWindowWidgets->window.getState();
    assert(g.pWndState);
    cursorHide();
    flushBuffer();

    for (unsigned i = 0; i < count; i++)
    {
        WidgetSearchStruct wss { searchedID : pWidgetIds[i] };

        if (getWidgetWSS(wss) && wss.isVisible)
        {
            g.parentCoord = wss.parentCoord;
            // set parent's background color
            pushClBg(getWidgetBgColor(wss.pWidget));
            drawWidgetInternal(wss.pWidget);
            popClBg();
        }
    }

    // if (glob_clear) g.pWndArray = nullptr, g.pWndState = nullptr;
    resetAttr();
    resetClBg();
    resetClFg();
    setCursorAt(g.pFocusedWgt);
    cursorShow();
}

// -----------------------------------------------------------------------------

}
