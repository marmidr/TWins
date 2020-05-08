/******************************************************************************
 * @brief   TWins - widget drawing
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#include "twins_widget_prv.hpp"
#include <assert.h>

// -----------------------------------------------------------------------------

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

    const int slider_at = ((height-1) * pos) / max;
    pushClFg(ColorFG::Default);
    // "▲▴ ▼▾ ◄◂ ►▸ ◘ █";

    for (int i = 0; i < height; i++)
    {
        moveTo(coord.col, coord.row + i);
        pIOs->writeStr(i == slider_at ? "◘" : "▒");
    }

    popClFg();
}

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
        auto capt_len = utf8len(pWgt->panel.title);
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

        s_line.setLength(pWgt->size.width, true, true);
        writeStr(s_line.cstr());
        moveBy(-pWgt->size.width, 1);
    }

    // restore colors
    popClFg();
}

static void drawEdit(const Widget *pWgt)
{
    g.str.clear();

    if (g.editState.pWgt)
        g.str = g.editState.str;
    else
        g.pWndState->getEditText(pWgt, g.str);

    g.str.setLength(pWgt->size.width-3, true);
    g.str.append("[^]");

    moveTo(g.parentCoord.col + pWgt->coord.col, g.parentCoord.row + pWgt->coord.row);
    pushClBg(pWgt->edit.bgColor);
    pushClFg(pWgt->edit.fgColor);
    writeStr(g.str.cstr());
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

    pushClBg(getWidgetBgColor(g.pWndArray + pWgt->link.parentIdx));
    drawArea(my_coord + Coord{pWgt->pagectrl.tabWidth, 0}, pWgt->size - Size{pWgt->pagectrl.tabWidth, 0},
        ColorBG::None, ColorFG::None, FrameStyle::PgControl);

    auto coord_bkp = g.parentCoord;
    g.parentCoord = my_coord;
    // tabs title
    g.str.clear();
    g.str.append(' ', (pWgt->pagectrl.tabWidth-8) / 2);
    g.str.append("≡ MENU ≡");
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
        g.str.setLength(pWgt->pagectrl.tabWidth, true, true);

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

    popClBg();
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
            s.setLength(pWgt->size.width-2, true, true);
        }
        else
        {
            // empty string - to erase old content
            s.setLength(pWgt->size.width-2);
        }

        if (focused && is_hl_item) pushAttr(FontAttrib::Inverse);
        writeStr(s.cstr());
        if (focused && is_hl_item) popAttr();
    }
}

static void drawDropDownList(const Widget *pWgt)
{
}

static void drawCanvas(const Widget *pWgt)
{
    g.pWndState->onCanvasDraw(pWgt);
}

// -----------------------------------------------------------------------------

static void drawWidgetInternal(const Widget *pWgt)
{
    bufferBegin();

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
    case Widget::Canvas:        drawCanvas(pWgt); break;
    default:                    break;
    }

    if (!en)
        popAttr();
    bufferEnd();
}

// -----------------------------------------------------------------------------
// ---- TWINS  P U B L I C  FUNCTIONS ------------------------------------------
// -----------------------------------------------------------------------------

void drawWidget(const Widget *pWindowArray, WID widgetId)
{
    // bool glob_clear = !(g.pWndArray || g.pWndState);
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
        if (getWidgetWSS(wss) && wss.isVisible)
        {
            g.parentCoord = wss.parentCoord;
            // set parent's background color
            pushClBg(getWidgetBgColor(g.pWndArray + wss.pWidget->link.parentIdx));
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

void drawWidgets(const Widget *pWindowArray, const WID *pWidgetIds, uint16_t count)
{
    // bool glob_clear = !(g.pWndArray || g.pWndState);
    assert(pWindowArray);
    assert(pWindowArray->type == Widget::Window);
    g.pWndArray = pWindowArray;
    g.pWndState = pWindowArray->window.getState();
    assert(g.pWndState);
    cursorHide();

    for (unsigned i = 0; i < count; i++)
    {
        WidgetSearchStruct wss { searchedID : pWidgetIds[i] };

        if (getWidgetWSS(wss) && wss.isVisible)
        {
            g.parentCoord = wss.parentCoord;
            // set parent's background color
            pushClBg(getWidgetBgColor(g.pWndArray + wss.pWidget->link.parentIdx));
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
