/******************************************************************************
 * @brief   TWins - widget drawing
 * @author  Mariusz Midor
 *          https://bitbucket.org/marmidr/twins
 *          https://github.com/marmidr/twins
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
static void drawWidgetInternal(CallEnv &env, const Widget *pWgt);
static void drawPage(CallEnv &env, const Widget *pWgt, bool eraseBg = false);

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
    case Widget::TextEdit:
        if (pWgt->textedit.bgColor != ColorBG::Inherit)
            return pWgt->textedit.bgColor;
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
        break;
    default:
        break;
    }

    return getWidgetBgColor(getParent(pWgt));
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
    case Widget::TextEdit:
        if (pWgt->textedit.fgColor != ColorFG::Inherit)
            return pWgt->textedit.fgColor;
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

    return getWidgetFgColor(getParent(pWgt));
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
    g_ws.str.appendFmt(ESC_CHAR_REPEAT_LAST_FMT, size.width - 3);
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
        g_ws.str.appendFmt(ESC_CHAR_REPEAT_LAST_FMT, size.width - 3);
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
    g_ws.str.appendFmt(ESC_CHAR_REPEAT_LAST_FMT, size.width - 3);
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
        g_ws.str.appendFmt(ESC_CHAR_REPEAT_LAST_FMT, size.width - 1);
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
        // TWINS_LOG_D("pos (%d) > max (%d)", pos, max);
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

static void drawWindow(CallEnv &env, const Widget *pWgt)
{
    Coord wnd_coord = pWgt->coord;
    env.parentCoord = {0, 0};
    env.pState->getWindowCoord(pWgt, wnd_coord);

    drawArea(wnd_coord, pWgt->size,
        pWgt->window.bgColor, pWgt->window.fgColor, FrameStyle::Double, true, pWgt->window.isPopup);

    // title
    String wnd_title;
    if (pWgt->window.title)
        wnd_title << pWgt->window.title;
    else
        env.pState->getWindowTitle(pWgt, wnd_title);

    if (wnd_title.size())
    {
        auto title_width = wnd_title.width();
        moveTo(wnd_coord.col + (pWgt->size.width - title_width - 4)/2, wnd_coord.row);
        pushAttr(FontAttrib::Bold);
        writeStrFmt("╡ %s ╞", wnd_title.cstr());
        popAttr();
    }

    flushBuffer();
    env.parentCoord = wnd_coord;

    for (int i = pWgt->link.childsIdx; i < pWgt->link.childsIdx + pWgt->link.childsCnt; i++)
        drawWidgetInternal(env, &env.pWidgets[i]);

    // reset colors set by frame drawer
    popClBg();
    popClFg();
    moveTo(0, wnd_coord.row + pWgt->size.height);
}

static void drawPanel(CallEnv &env, const Widget *pWgt)
{
    FontMemento _m;
    const auto my_coord = env.parentCoord + pWgt->coord;

    drawArea(my_coord, pWgt->size,
        pWgt->panel.bgColor, pWgt->panel.fgColor,
        pWgt->panel.noFrame ? FrameStyle::None : FrameStyle::Single);
    flushBuffer();

    // title
    if (pWgt->panel.title)
    {
        auto title_width = String::width(pWgt->panel.title);
        moveTo(my_coord.col + (pWgt->size.width - title_width - 2)/2, my_coord.row);
        pushAttr(FontAttrib::Bold);
        writeStrFmt(" %s ", pWgt->panel.title);
        popAttr();
    }

    flushBuffer();
    auto coord_bkp = env.parentCoord;
    env.parentCoord = my_coord;

    for (int i = pWgt->link.childsIdx; i < pWgt->link.childsIdx + pWgt->link.childsCnt; i++)
        drawWidgetInternal(env, &env.pWidgets[i]);

    env.parentCoord = coord_bkp;
}

static void drawLabel(CallEnv &env, const Widget *pWgt)
{
    g_ws.str.clear();

    // label text
    if (pWgt->label.text)
        g_ws.str = pWgt->label.text;
    else
        env.pState->getLabelText(pWgt, g_ws.str);

    FontMemento _m;

    // setup colors
    pushClFg(getWidgetFgColor(pWgt));
    pushClBg(getWidgetBgColor(pWgt));

    // print all lines
    const char *p_line = g_ws.str.cstr();
    String s_line;
    moveTo(env.parentCoord.col + pWgt->coord.col, env.parentCoord.row + pWgt->coord.row);
    const uint8_t max_lines = pWgt->size.height ? pWgt->size.height : 50;
    const uint8_t line_width = pWgt->size.width;

    for (uint16_t line = 0; line < max_lines; line++)
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

        if (line_width)
            s_line.setWidth(line_width, true);

        writeStrLen(s_line.cstr(), s_line.size());
        moveBy(-(int16_t)s_line.width(), 1);
        flushBuffer();

        if (!p_eol && !pWgt->size.height)
            break;
    }
}

static void drawTextEdit(CallEnv &env, const Widget *pWgt)
{
    g_ws.str.clear();
    int16_t display_pos = 0;
    const int16_t max_w = pWgt->size.width-3;

    if (pWgt == g_ws.textEditState.pWgt)
    {
        // in edit mode; similar calculation in setCursorAt()
        g_ws.str = g_ws.textEditState.str;
        auto cursor_pos = g_ws.textEditState.cursorPos;
        auto delta = (max_w/2);

        while (cursor_pos >= max_w-1)
        {
            cursor_pos -= delta;
            display_pos += delta;
        }
    }
    else
    {
        env.pState->getTextEditText(pWgt, g_ws.str);
    }

    const int txt_width = g_ws.str.width();

    if (display_pos > 0)
    {
        auto *str_beg = String::u8skip(g_ws.str.cstr(), display_pos + 1);
        String s("◁");
        s << str_beg;
        g_ws.str = std::move(s);
    }

    if (display_pos + max_w <= txt_width)
    {
        g_ws.str.setWidth(pWgt->size.width-3-1);
        g_ws.str.append("▷");
    }
    else
    {
        g_ws.str.setWidth(pWgt->size.width-3);
    }
    g_ws.str.append("[^]");

    bool focused = env.pState->isFocused(pWgt);
    auto clbg = getWidgetBgColor(pWgt);
    intensifyClIf(focused, clbg);

    FontMemento _m;
    moveTo(env.parentCoord.col + pWgt->coord.col, env.parentCoord.row + pWgt->coord.row);
    pushClBg(clbg);
    pushClFg(getWidgetFgColor(pWgt));
    writeStrLen(g_ws.str.cstr(), g_ws.str.size());
}

static void drawLed(CallEnv &env, const Widget *pWgt)
{
    auto clbg = env.pState->getLedLit(pWgt) ? pWgt->led.bgColorOn : pWgt->led.bgColorOff;
    g_ws.str.clear();

    if (pWgt->led.text)
        g_ws.str = pWgt->led.text;
    else
        env.pState->getLedText(pWgt, g_ws.str);

    // led text
    FontMemento _m;
    moveTo(env.parentCoord.col + pWgt->coord.col, env.parentCoord.row + pWgt->coord.row);
    pushClBg(clbg);
    pushClFg(getWidgetFgColor(pWgt));
    writeStrLen(g_ws.str.cstr(), g_ws.str.size());
}

static void drawCheckbox(CallEnv &env, const Widget *pWgt)
{
    const char *s_chk_state = env.pState->getCheckboxChecked(pWgt) ? "[■] " : "[ ] ";
    bool focused = env.pState->isFocused(pWgt);
    auto clfg = getWidgetFgColor(pWgt);
    intensifyClIf(focused, clfg);

    FontMemento _m;
    moveTo(env.parentCoord.col + pWgt->coord.col, env.parentCoord.row + pWgt->coord.row);
    if (focused) pushAttr(FontAttrib::Bold);
    pushClFg(clfg);
    writeStr(s_chk_state);
    writeStr(pWgt->checkbox.text);
}

static void drawRadio(CallEnv &env, const Widget *pWgt)
{
    const char *s_radio_state = pWgt->radio.radioId == env.pState->getRadioIndex(pWgt) ? "(●) " : "( ) ";
    bool focused = env.pState->isFocused(pWgt);
    auto clfg = getWidgetFgColor(pWgt);
    intensifyClIf(focused, clfg);

    FontMemento _m;
    moveTo(env.parentCoord.col + pWgt->coord.col, env.parentCoord.row + pWgt->coord.row);
    if (focused) pushAttr(FontAttrib::Bold);
    pushClFg(clfg);
    writeStr(s_radio_state);
    writeStr(pWgt->radio.text);
}

static void drawButton(CallEnv &env, const Widget *pWgt)
{
    const bool focused = env.pState->isFocused(pWgt);
    const bool pressed = pWgt == g_ws.pMouseDownWgt;
    auto clfg = getWidgetFgColor(pWgt);
    intensifyClIf(focused, clfg);

    String txt;
    if (pWgt->button.text)
        txt = pWgt->button.text;
    else
        env.pState->getButtonText(pWgt, txt);

    if (pWgt->button.style == ButtonStyle::Simple)
    {
        FontMemento _m;
        g_ws.str.clear()
                .append("[ ")
                .append(txt)
                .append(" ]");

        moveTo(env.parentCoord.col + pWgt->coord.col, env.parentCoord.row + pWgt->coord.row);
        if (focused) pushAttr(FontAttrib::Bold);
        if (pressed) pushAttr(FontAttrib::Inverse);
        auto clbg = pressed ? getWidgetBgColor(pWgt) : getWidgetBgColor(getParent(pWgt));
        pushClBg(clbg);
        pushClFg(clfg);
        writeStrLen(g_ws.str.cstr(), g_ws.str.size());
    }
    else if (pWgt->button.style == ButtonStyle::Solid)
    {
        {
            FontMemento _m;
            g_ws.str.clear();
            g_ws.str << " " << txt << " ";

            auto clbg = getWidgetBgColor(pWgt);
            moveTo(env.parentCoord.col + pWgt->coord.col, env.parentCoord.row + pWgt->coord.row);
            if (focused) pushAttr(FontAttrib::Bold);
            if (pressed) pushAttr(FontAttrib::Inverse);
            pushClBg(clbg);
            pushClFg(clfg);
            writeStrLen(g_ws.str.cstr(), g_ws.str.size());
        }

        auto shadow_len = 2 + txt.width();

        if (pressed)
        {
            // erase trailing shadow
            pushClBg(getWidgetBgColor(getParent(pWgt)));
            writeChar(' ');
            // erase shadow below
            moveTo(env.parentCoord.col + pWgt->coord.col + 1, env.parentCoord.row + pWgt->coord.row + 1);
            writeStr(" ", shadow_len);
            popClBg();
        }
        else
        {
            FontMemento _m;
            // trailing shadow
            pushClBg(getWidgetBgColor(getParent(pWgt)));
            writeStr(ESC_FG_COLOR(233));
            writeStr("▄");
            // shadow below
            moveTo(env.parentCoord.col + pWgt->coord.col + 1, env.parentCoord.row + pWgt->coord.row + 1);
            writeStr("▀", shadow_len);
        }
    }
    else if (pWgt->button.style == ButtonStyle::Solid1p5)
    {
        g_ws.str.clear();
        g_ws.str << " " << txt << " ";
        auto clbg = getWidgetBgColor(pWgt);
        auto clparbg = getWidgetBgColor(getParent(pWgt));
        const int16_t bnt_len = 2 + txt.width();
        const char* scl_shadow = ESC_BG_COLOR(233);
        const char* scl_bg2fg = transcodeClBg2Fg(encodeCl(clbg));
        FontMemento _m;

        // upper half line
        moveTo(env.parentCoord.col + pWgt->coord.col, env.parentCoord.row + pWgt->coord.row);
        pushClBg(clparbg);
        if (pressed)
            pushClFg(clfg);
        else
            writeStr(scl_bg2fg);
        writeStr("▄", bnt_len);

        // middle line - text
        moveBy(-bnt_len, 1);
        pushClBg(clbg);
        pushClFg(clfg);
        if (pressed) pushAttr(FontAttrib::Inverse);
        if (focused) pushAttr(FontAttrib::Bold);
        writeStrLen(g_ws.str.cstr(), g_ws.str.size());
        if (focused) popAttr();
        if (pressed) popAttr();

        // middle-shadow
        if (pressed)
            pushClBg(clparbg);
        else
            writeStr(scl_shadow);
        writeChar(' ');

        // lower half-line
        moveBy(-bnt_len-1, 1);
        if (pressed)
        {
            pushClFg(clfg);
            pushClBg(clparbg);
            writeStr("▀");
            pushClBg(clparbg);
        }
        else
        {
            writeStr(scl_bg2fg);
            pushClBg(clparbg);
            writeStr("▀");
            writeStr(scl_shadow);
        }
        writeStr("▀", bnt_len-1);

        // trailing shadow
        writeChar(' ');
    }
}

static void drawPageControl(CallEnv &env, const Widget *pWgt)
{
    const auto my_coord = env.parentCoord + pWgt->coord;
    FontMemento _m;
    pushClBg(getWidgetBgColor(pWgt));
    pushClFg(getWidgetFgColor(pWgt));
    drawArea(my_coord + Coord{pWgt->pagectrl.tabWidth, 0}, pWgt->size - Size{pWgt->pagectrl.tabWidth, 0},
        ColorBG::Inherit, ColorFG::Inherit, FrameStyle::PgControl);
    flushBuffer();

    auto coord_bkp = env.parentCoord;
    env.parentCoord = my_coord;
    // tabs title
    g_ws.str.clear();
    g_ws.str.append(' ', (pWgt->pagectrl.tabWidth-8) / 2);
    g_ws.str.append("≡ MENU ≡");
    g_ws.str.setWidth(pWgt->pagectrl.tabWidth);
    moveTo(my_coord.col, my_coord.row + pWgt->pagectrl.vertOffs);
    pushAttr(FontAttrib::Inverse);
    writeStrLen(g_ws.str.cstr(), g_ws.str.size());
    popAttr();

    // draw tabs and pages
    const int pg_idx = env.pState->getPageCtrlPageIndex(pWgt);
    // const bool focused = env.pState->isFocused(pWgt);
    // moveTo(env.parentCoord.col + pWgt->coord.col, env.parentCoord.row + pWgt->coord.row);
    flushBuffer();

    for (int i = 0; i < pWgt->link.childsCnt; i++)
    {
        if (i == pWgt->size.height - 1 - pWgt->pagectrl.vertOffs)
            break;

        const auto *p_page = &env.pWidgets[pWgt->link.childsIdx + i];

        // draw page title
        g_ws.str.clear();
        g_ws.str.appendFmt("%s%s", i == pg_idx ? "►" : " ", p_page->page.title);
        g_ws.str.setWidth(pWgt->pagectrl.tabWidth, true);

        moveTo(my_coord.col, my_coord.row + pWgt->pagectrl.vertOffs + i + 1);

        // for Page we do not want inherit after it's title color
        auto clfg = p_page->page.fgColor;
        if (clfg == ColorFG::Inherit)
            clfg = getWidgetFgColor(p_page);

        pushClFg(clfg);
        if (i == pg_idx) pushAttr(FontAttrib::Inverse);
        writeStrLen(g_ws.str.cstr(), g_ws.str.size());
        if (i == pg_idx) popAttr();
        popClFg();

        if (env.pState->isVisible(p_page))
        {
            flushBuffer();
            env.parentCoord.col += pWgt->pagectrl.tabWidth;
            drawPage(env, p_page);
            env.parentCoord.col -= pWgt->pagectrl.tabWidth;
        }
    }

    env.parentCoord = coord_bkp;
}

static void drawPage(CallEnv &env, const Widget *pWgt, bool eraseBg)
{
    if (eraseBg)
    {
        const Widget *p_pgctrl = getParent(pWgt);
        auto page_coord = getScreenCoord(p_pgctrl);
        page_coord.col += p_pgctrl->pagectrl.tabWidth;
        drawArea(page_coord, p_pgctrl->size - Size{p_pgctrl->pagectrl.tabWidth, 0},
            ColorBG::Inherit, ColorFG::Inherit, FrameStyle::PgControl);
    }

    // draw childrens
    for (int i = pWgt->link.childsIdx; i < pWgt->link.childsIdx + pWgt->link.childsCnt; i++)
        drawWidgetInternal(env, &env.pWidgets[i]);
}

static void drawProgressBar(CallEnv &env, const Widget *pWgt)
{
    const char* style_data[][2] =
    {
        {"#", "."},
        {"█", "▒"},
        {"■", "□"}
    };

    int32_t pos = 0, max = 1;
    auto style = (short)pWgt->progressbar.style;
    env.pState->getProgressBarState(pWgt, pos, max);

    if (max <= 0) max = 1;
    if (pos > max) pos = max;

    moveTo(env.parentCoord.col + pWgt->coord.col, env.parentCoord.row + pWgt->coord.row);
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
        drawListScrollBarV(p.coord + Coord{uint8_t(p.wgt_width-1), p.frame_size},
            p.items_visible, p.items_cnt-1, p.sel_idx);
    }

    flushBuffer();

    for (int i = 0; i < p.items_visible; i++)
    {
        bool is_current_item = p.items_cnt ? (p.top_item + i == p.item_idx) : false;
        bool is_sel_item = p.top_item + i == p.sel_idx;
        moveTo(p.coord.col + p.frame_size, p.coord.row + i + p.frame_size);

        g_ws.str.clear();

        if (p.top_item + i < p.items_cnt)
        {
            p.getItem(p.top_item + i, g_ws.str);
            g_ws.str.insert(0, is_current_item ? "►" : " ");
            g_ws.str.setWidth(p.wgt_width - 1 - p.frame_size, true);
        }
        else
        {
            // empty string - to erase old content
            g_ws.str.setWidth(p.wgt_width - 1 - p.frame_size);
        }

        if (p.focused && is_sel_item) pushAttr(FontAttrib::Inverse);
        if (is_current_item) pushAttr(FontAttrib::Underline);
        writeStrLen(g_ws.str.cstr(), g_ws.str.size());
        if (is_current_item) popAttr();
        if (p.focused && is_sel_item) popAttr();
    }
};

static void drawListBox(CallEnv &env, const Widget *pWgt)
{
    FontMemento _m;
    const auto my_coord = env.parentCoord + pWgt->coord;
    drawArea(my_coord, pWgt->size,
        pWgt->listbox.bgColor, pWgt->listbox.fgColor,
        pWgt->listbox.noFrame ? FrameStyle::None : FrameStyle::ListBox, false);

    if (pWgt->size.height < 3)
        return;

    DrawListParams dlp = {};
    dlp.coord = my_coord;
    env.pState->getListBoxState(pWgt, dlp.item_idx, dlp.sel_idx, dlp.items_cnt);
    dlp.frame_size = !pWgt->listbox.noFrame;
    dlp.items_visible = pWgt->size.height - (dlp.frame_size * 2);
    dlp.top_item = (dlp.sel_idx / dlp.items_visible) * dlp.items_visible;
    dlp.focused = env.pState->isFocused(pWgt);
    dlp.wgt_width = pWgt->size.width;
    dlp.getItem = [pWgt, &env](int16_t idx, String &out) { env.pState->getListBoxItem(pWgt, idx, out); };
    drawList(dlp);
}

static void drawComboBox(CallEnv &env, const Widget *pWgt)
{
    FontMemento _m;
    const auto my_coord = env.parentCoord + pWgt->coord;
    const bool focused = env.pState->isFocused(pWgt);

    int16_t item_idx = 0; int16_t sel_idx = 0; int16_t items_count; bool drop_down = false;
    env.pState->getComboBoxState(pWgt, item_idx, sel_idx, items_count, drop_down);

    {
        g_ws.str.clear();
        env.pState->getComboBoxItem(pWgt, item_idx, g_ws.str);
        g_ws.str.insert(0, " ");
        g_ws.str.setWidth(pWgt->size.width - 4, true);
        g_ws.str << " [▼]";

        moveTo(my_coord.col, my_coord.row);
        pushClFg(getWidgetFgColor(pWgt));
        pushClBg(getWidgetBgColor(pWgt));
        if (focused && !drop_down) pushAttr(FontAttrib::Inverse);
        if (drop_down) pushAttr(FontAttrib::Underline);
        if (focused) pushAttr(FontAttrib::Bold);
        writeStrLen(g_ws.str.cstr(), g_ws.str.size());
        if (focused) popAttr();
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
        dlp.getItem = [pWgt, &env](int16_t idx, String &out) { env.pState->getComboBoxItem(pWgt, idx, out); };
        drawList(dlp);
    }
}

static void drawCustomWgt(CallEnv &env, const Widget *pWgt)
{
    env.pState->onCustomWidgetDraw(pWgt);
}

static void drawTextBox(CallEnv &env, const Widget *pWgt)
{
    FontMemento _m;
    const auto my_coord = env.parentCoord + pWgt->coord;

    drawArea(my_coord, pWgt->size,
        pWgt->textbox.bgColor, pWgt->textbox.fgColor,
        FrameStyle::ListBox, false, false);

    if (pWgt->size.height < 3)
        return;

    const uint8_t lines_visible = pWgt->size.height - 2;
    const twins::Vector<twins::CStrView> *p_lines = nullptr;
    int16_t top_line = 0;

    env.pState->getTextBoxState(pWgt, &p_lines, top_line);

    if (!p_lines || !p_lines->size())
        return;

    if (top_line > (int)p_lines->size())
    {
        top_line = p_lines->size() - lines_visible;
        env.pState->onTextBoxScroll(pWgt, top_line);
    }

    if (top_line < 0)
    {
        env.pState->onTextBoxScroll(pWgt, top_line);
        top_line = 0;
    }

    drawListScrollBarV(my_coord + Coord{uint8_t(pWgt->size.width-1), 1},
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
        g_ws.str.clear();
        if (top_line + i < (int)p_lines->size())
        {
            const auto &sr = (*p_lines)[top_line + i];
            g_ws.str.appendLen(sr.data, sr.size);
        }
        g_ws.str.setWidth(pWgt->size.width - 2, true);
        moveTo(my_coord.col + 1, my_coord.row + i + 1);
        writeStrLen(g_ws.str.cstr(), g_ws.str.size());
    }

    flushBuffer();
}

static void drawLayer(CallEnv &env, const Widget *pWgt)
{
    // draw only childrens; to erase, redraw layer's parent
    for (int i = pWgt->link.childsIdx; i < pWgt->link.childsIdx + pWgt->link.childsCnt; i++)
        drawWidgetInternal(env, &env.pWidgets[i]);
}

// -----------------------------------------------------------------------------

static void drawWidgetInternal(CallEnv &env, const Widget *pWgt)
{
    if (!env.pState->isVisible(pWgt))
        return;

    bool en = isEnabled(env, pWgt);
    if (!en) pushAttr(FontAttrib::Faint);

    switch (pWgt->type)
    {
    case Widget::Window:        drawWindow(env, pWgt); break;
    case Widget::Panel:         drawPanel(env, pWgt); break;
    case Widget::Label:         drawLabel(env, pWgt); break;
    case Widget::TextEdit:      drawTextEdit(env, pWgt); break;
    case Widget::CheckBox:      drawCheckbox(env, pWgt); break;
    case Widget::Radio:         drawRadio(env, pWgt);  break;
    case Widget::Button:        drawButton(env, pWgt); break;
    case Widget::Led:           drawLed(env, pWgt); break;
    case Widget::PageCtrl:      drawPageControl(env, pWgt); break;
    case Widget::Page:          drawPage(env, pWgt, true); break;
    case Widget::ProgressBar:   drawProgressBar(env, pWgt); break;
    case Widget::ListBox:       drawListBox(env, pWgt); break;;
    case Widget::ComboBox:      drawComboBox(env, pWgt); break;
    case Widget::CustomWgt:     drawCustomWgt(env, pWgt); break;
    case Widget::TextBox:       drawTextBox(env, pWgt); break;
    case Widget::Layer:         drawLayer(env, pWgt); break;
    default:                    break;
    }

    if (!en)
        popAttr();

    flushBuffer();
}

// -----------------------------------------------------------------------------
// ---- TWINS  P U B L I C  FUNCTIONS ------------------------------------------
// -----------------------------------------------------------------------------

void drawWidgets(const Widget *pWindowWidgets, const WID *pWidgetIds, uint16_t count)
{
    if (count == 0)
        return;

    CallEnv env(pWindowWidgets);
    assert(pWidgetIds);
    g_ws.pFocusedWgt = getWidgetByWID(env, env.pState->getFocusedID());
    cursorHide();
    flushBuffer();

    if (count == 1 && *pWidgetIds == WIDGET_ID_ALL)
    {
        drawWidgetInternal(env, pWindowWidgets);
    }
    else
    {
        for (uint16_t i = 0; i < count; i++)
        {
            WidgetSearchStruct wss { searchedID : pWidgetIds[i] };

            if (getWidgetWSS(env, wss) && wss.isVisible)
            {
                env.parentCoord = wss.parentCoord;
                // set parent's background color
                pushClBg(getWidgetBgColor(wss.pWidget));
                drawWidgetInternal(env, wss.pWidget);
                popClBg();
            }
        }
    }

    resetAttr();
    resetClBg();
    resetClFg();
    setCursorAt(env, g_ws.pFocusedWgt);
    cursorShow();
    flushBuffer();
}

// -----------------------------------------------------------------------------

}
