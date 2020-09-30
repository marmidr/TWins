/******************************************************************************
 * @brief   TWins - widget drawing
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#include "twins_widget_prv.hpp"
#include "twins_utils.hpp"

#include <string.h>
#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <utility> //std::swap

// -----------------------------------------------------------------------------

namespace twins
{

Glob g;

// forward decl
static bool isPointWithin(uint8_t col, uint8_t row, const Rect& e);
static bool isRectWithin(const Rect& i, const Rect& e);
static bool isVisible(const Widget *pWgt);

// -----------------------------------------------------------------------------
// ---- TWINS INTERNAL FUNCTIONS -----------------------------------------------
// -----------------------------------------------------------------------------

bool getWidgetWSS(WidgetSearchStruct &wss)
{
    if (wss.searchedID == WIDGET_ID_NONE)
        return false;

    const Widget *p_wgt = g.pWndWidgets;

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
        const auto *p_parent = g.pWndWidgets + parent_idx;
        wss.isVisible &= g.pWndState->isVisible(p_parent);

        Coord coord = p_parent->coord;
        if (p_parent->type == Widget::Type::Window)
            g.pWndState->getWindowCoord(p_parent, coord);
        wss.parentCoord += coord;

        if (p_parent->type == Widget::Type::PageCtrl)
            wss.parentCoord.col += p_parent->pagectrl.tabWidth;

        if (parent_idx == 0)
            break;

        parent_idx = p_parent->link.parentIdx;
    }

    return true;
}

const Widget* getWidgetByWID(const WID widgetId)
{
    for (unsigned i = 0; g.pWndWidgets[i].type != Widget::None; i++)
        if (g.pWndWidgets[i].id == widgetId)
            return &g.pWndWidgets[i];

    return nullptr;
}

const Widget* getParent(const Widget *pWgt)
{
    assert(pWgt->link.parentIdx <= pWgt->link.ownIdx);

    const Widget *p_parent = pWgt;
    p_parent -= pWgt->link.ownIdx - pWgt->link.parentIdx;
    return p_parent;
}

const Widget* getWidgetAt(uint8_t col, uint8_t row, Rect &wgtRect)
{
    const Widget *p_wgt_at = nullptr;
    Rect best_rect;
    best_rect.setMax();

    for (unsigned i = 0; g.pWndWidgets[i].type != Widget::None; i++)
    {
        bool stop_searching = true;
        Rect r;
        const auto *p_wgt = g.pWndWidgets + i;
        r.coord = getScreenCoord(p_wgt);
        r.size = p_wgt->size;

        // correct the widget size
        switch (p_wgt->type)
        {
        case Widget::Edit:
            break;
        case Widget::CheckBox:
            r.size.height = 1;
            r.size.width = 4 + utf8len(p_wgt->checkbox.text);
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
        case Widget::ComboBox:
            break;
        default:
            stop_searching = false;
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

                // visible and clickable widget found?
                if (stop_searching)
                    break;
            }
        }
    }

    return p_wgt_at;
}

void setCursorAt(const Widget *pWgt)
{
    if (!pWgt)
        return;

    Coord coord = getScreenCoord(pWgt);

    switch (pWgt->type)
    {
    case Widget::Edit:
        if (g.editState.pWgt == pWgt)
        {
            const int16_t max_w = pWgt->size.width-3;
            coord.col += g.editState.cursorPos;
            auto cursor_pos = g.editState.cursorPos;
            auto delta = (max_w/2);
            while (cursor_pos >= max_w-1)
            {
                coord.col -= delta;
                cursor_pos -= delta;
            }
        }
        else
        {
            coord.col += pWgt->size.width-2;
        }
        break;
    case Widget::CheckBox:
        coord.col += 1;
        break;
    case Widget::Radio:
        coord.col += 1;
        break;
    case Widget::Button:
        coord.col += pWgt->button.style == ButtonStyle::Simple ? 2 : 1;
        break;
    case Widget::PageCtrl:
        coord.row += 1;
        coord.row += g.pWndState->getPageCtrlPageIndex(pWgt);
        break;
    case Widget::ListBox:
    {
        int16_t idx = 0, selidx = 0, cnt = 0;
        const uint8_t frame_size = !pWgt->listbox.noFrame;
        g.pWndState->getListBoxState(pWgt, idx, selidx, cnt);

        int page_size = pWgt->size.height - (frame_size * 2);
        int row = selidx % page_size;

        coord.col += frame_size;
        coord.row += frame_size + row;
        break;
    }
    case Widget::ComboBox:
        break;
    case Widget::TextBox:
        break;
    default:
        break;
    }

    moveTo(coord.col, coord.row);
}

// -----------------------------------------------------------------------------
// ---- TWINS PRIVATE FUNCTIONS ------------------------------------------------
// -----------------------------------------------------------------------------

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

static void invalidateRadioGroup(const Widget *pRadio)
{
    const Widget *p_parent = g.pWndWidgets + pRadio->link.parentIdx;
    const auto group_id = pRadio->radio.groupId;

    for (unsigned i = 0; i < p_parent->link.childsCnt; i++)
    {
        const auto *p_wgt = g.pWndWidgets + p_parent->link.childsIdx + i;
        if (p_wgt->type == Widget::Type::Radio && p_wgt->radio.groupId == group_id)
            g.pWndState->invalidate(p_wgt->id);
    }
}

static bool isVisible(const Widget *pWgt)
{
    bool vis = g.pWndState->isVisible(pWgt);
    int parent_idx = pWgt->link.parentIdx;

    for (; vis;)
    {
        const auto *p_parent = g.pWndWidgets + parent_idx;
        vis &= g.pWndState->isVisible(p_parent);

        if (parent_idx == 0)
            break;

        parent_idx = p_parent->link.parentIdx;
    }

    return vis;
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
    case Widget::ComboBox:
    case Widget::TextBox:
        return g.pWndState->isEnabled(pWgt);
    default:
        return false;
    }
}

static bool isFocusable(const WID widgetId)
{
    if (const auto *p_wgt = getWidgetByWID(widgetId))
        return isFocusable(p_wgt);
    return false;
}

static const Widget* getNextFocusable(const Widget *pParent, WID focusedID, bool forward, const Widget *pFirstParent = nullptr)
{
    if (!pParent)
        return nullptr;

    if (pParent == pFirstParent)
    {
        // TWINS_LOG("full loop detected");
        return nullptr;
    }

    if (pParent->id == focusedID)
        return nullptr;

    const Widget *p_childs = {};
    uint16_t child_cnt = 0;

    // get childrens and their number
    switch (pParent->type)
    {
    case Widget::Window:
    case Widget::Panel:
    case Widget::Page:
    {
        p_childs  = g.pWndWidgets + pParent->link.childsIdx;
        child_cnt = pParent->link.childsCnt;
        break;
    }
    case Widget::PageCtrl:
    {
        // get selected page childrens
        int idx = g.pWndState->getPageCtrlPageIndex(pParent);
        if (idx >= 0 && idx < pParent->link.childsCnt)
        {
            pParent   = g.pWndWidgets + pParent->link.childsIdx + idx;
            p_childs  = g.pWndWidgets + pParent->link.childsIdx;
            child_cnt = pParent->link.childsCnt;
        }
        else
        {
            return nullptr;
        }
        break;
    }
    default:
        TWINS_LOG("-E- no-parent widget");
        return nullptr;
    }

    if (child_cnt == 0)
        return nullptr;

    if (!pFirstParent && (pParent->type == Widget::Panel || pParent->type == Widget::Page))
    {
        // it must be Panel or Page because while traversing we never step below Page level
        // TWINS_LOG("1st parent[id:%u, %s]", pParent->id, toString(pParent->type));
        pFirstParent = pParent;
    }

    assert(p_childs);
    const Widget *p_wgt = nullptr;

    // TWINS_LOG("pParent[id:%u, %s] cur= %d", pParent->id, toString(pParent->type), focusedID); twins::sleepMs(200);

    if (focusedID == WIDGET_ID_NONE)
    {
        // get first/last of the childs ID
        p_wgt = forward ? &p_childs[0] : &p_childs[child_cnt-1];
        focusedID = p_wgt->id;

        if (isFocusable(p_wgt))
            return p_wgt;

        if (isParent(p_wgt))
            if (const auto *p = getNextFocusable(p_wgt, WIDGET_ID_NONE, forward, pFirstParent))
                return p;
    }
    else
    {
        // get pointer to focusedID
        p_wgt = p_childs;

        while (p_wgt->id != focusedID && p_wgt < p_childs + child_cnt)
            p_wgt++;

        // expect that childs have focusedID
        //assert(p_wgt < p_childs + child_cnt); // occures rarely
        if (p_wgt >= p_childs + child_cnt)
        {
            TWINS_LOG("-W- focused ID=%d not found on parent ID=%d", focusedID, pParent->id);
            return nullptr;
        }
    }


    // TWINS_LOG("search in %s childs[%d]", toString(pParent->type), child_cnt);
    // iterate until focusable found or childs border reached
    assert(p_wgt);

    for (uint16_t i = 0; i < child_cnt; i++)
    {
        p_wgt += forward ? 1 : -1;

        if (p_wgt == p_childs + child_cnt || p_wgt == p_childs - 1)
        {
            // border reached: if we are on Panel, jump to panel's parent next child
            if (pParent->type == Widget::Panel)
                return getNextFocusable(getParent(pParent), pParent->id, forward, pFirstParent);

            if (p_wgt > p_childs) p_wgt = p_childs;
            else                  p_wgt = p_childs + child_cnt - 1;
        }

        if (isFocusable(p_wgt))
            return p_wgt;

        if (isParent(p_wgt))
            if (const auto *p = getNextFocusable(p_wgt, WIDGET_ID_NONE, forward, pFirstParent))
                return p;
    }

    return nullptr;
}

static WID getNextToFocus(const WID focusedID, bool forward)
{
    WidgetSearchStruct wss { searchedID : focusedID };

    if (!getWidgetWSS(wss))
    {
        // here, find may fail only if invalid focusedID was given
        wss.pWidget = g.pWndWidgets;
    }

    // use the parent to get next widget
    if (auto *p_next = getNextFocusable(g.pWndWidgets + wss.pWidget->link.parentIdx, focusedID, forward))
    {
        return p_next->id;
    }

    return WIDGET_ID_NONE;
}

static WID getParentToFocus(WID focusedID)
{
    if (focusedID == WIDGET_ID_NONE)
        return g.pWndWidgets[0].id;

    WidgetSearchStruct wss { searchedID : focusedID };

    if (getWidgetWSS(wss))
    {
        const auto *p_wgt = &g.pWndWidgets[wss.pWidget->link.parentIdx];
        // g.parentCoord -= wss.pWidget->coord;
        return p_wgt->id;
    }

    return g.pWndWidgets[0].id;
}

static bool changeFocusTo(WID newID)
{
    auto &curr_id = g.pWndState->getFocusedID();
    // TWINS_LOG("curr_id=%d, newID=%d", curr_id, newID);

    if (newID != curr_id)
    {
        auto prev_id = curr_id;
        curr_id = newID;
        WidgetSearchStruct wss { searchedID : newID };

        if (getWidgetWSS(wss))
        {
            if (wss.pWidget->type == Widget::ListBox)
            {
                int16_t idx = 0, selidx = 0, cnt = 0;
                g.pWndState->getListBoxState(wss.pWidget, idx, selidx, cnt);

                if (idx < 0 && cnt > 0)
                {
                    g.pWndState->onListBoxSelect(wss.pWidget, selidx);
                }
            }

            if (wss.pWidget->type == Widget::TextBox)
            {
                g.textboxTopLine = 0;
            }
        }

        if (isFocusable(prev_id))
            g.pWndState->invalidate(prev_id);
        if (isFocusable(newID))
            g.pWndState->invalidate(newID);

        setCursorAt(wss.pWidget);
        g.pFocusedWgt = wss.pWidget;
        return true;
    }

    return false;
}

static const Widget *findMainPgControl()
{
    assert(g.pWndWidgets);
    const auto *p_wnd = &g.pWndWidgets[0];

    for (unsigned i = 0; i < p_wnd->link.childsCnt; i++)
    {
        const auto *p_wgt = g.pWndWidgets + p_wnd->link.childsIdx + i;

        if (p_wgt->type == Widget::PageCtrl)
            return p_wgt;
    }

    return nullptr;
}

static void pgControlChangePage(const Widget *pWgt, bool next)
{
    assert(pWgt);
    assert(pWgt->type == Widget::PageCtrl);

    int idx = g.pWndState->getPageCtrlPageIndex(pWgt);
    idx += next ? 1 : -1;
    if (idx < 0)                     idx = pWgt->link.childsCnt -1;
    if (idx >= pWgt->link.childsCnt) idx = 0;

    // changeFocusTo(pWgt->id); // DON'T or separate focus for each Tab will not work
    g.pWndState->onPageControlPageChange(pWgt, idx);
    g.pWndState->invalidate(pWgt->id);

    // cancel EDIT mode
    g.editState.pWgt = nullptr;

    if (const auto *p_wgt = getWidgetByWID(g.pWndState->getFocusedID()))
    {
        //TWINS_LOG("focused id=%d (%s)", p_wgt->id, toString(p_wgt->type));
        g.pFocusedWgt = p_wgt;
        setCursorAt(p_wgt);
    }
    else
    {
        g.pFocusedWgt = p_wgt;
        moveToHome();
    }
}

static void comboBoxHideList(const Widget *pWgt)
{
    assert(pWgt);
    assert(pWgt->type == Widget::ComboBox);

    g.pWndState->onComboBoxDrop(pWgt, false);
    // redraw parent to hide list
    const auto *p_parent = getParent(pWgt);
    if (p_parent->type == Widget::Page)
        p_parent = getParent(p_parent);
    g.pWndState->invalidate(p_parent->id);
    g.pDropDownCombo = nullptr;
}

// -----------------------------------------------------------------------------

static bool processKey_Edit(const Widget *pWgt, const KeyCode &kc)
{
    if (pWgt == g.editState.pWgt)
    {
        // if in edit state, allow user to handle key
        if (g.pWndState->onEditInputEvt(pWgt, kc, g.editState.str, g.editState.cursorPos))
        {
            g.pWndState->invalidate(pWgt->id);
            return true;
        }
        // user let us continue checking the key
    }

    bool key_handled = false;

    if (g.editState.pWgt)
    {
        auto cursor_pos = g.editState.cursorPos;

        if (kc.m_spec)
        {
            switch (kc.key)
            {
            case Key::Esc:
                // cancel editing
                g.editState.pWgt = nullptr;
                g.pWndState->invalidate(pWgt->id);
                key_handled = true;
                break;
            case Key::Tab:
                // real TAB may have different widths and require extra processing
                g.editState.str.insert(cursor_pos, "    ");
                cursor_pos += 4;
                g.pWndState->invalidate(pWgt->id);
                key_handled = true;
                break;
            case Key::Enter:
                // finish editing
                g.pWndState->onEditChange(pWgt, std::move(g.editState.str));
                g.editState.pWgt = nullptr;
                g.pWndState->invalidate(pWgt->id);
                key_handled = true;
                break;
            case Key::Backspace:
                if (cursor_pos > 0)
                {
                    if (kc.m_ctrl)
                    {
                        g.editState.str.erase(0, cursor_pos);
                        cursor_pos = 0;
                    }
                    else
                    {
                        g.editState.str.erase(cursor_pos-1);
                        cursor_pos--;
                    }
                    g.pWndState->invalidate(pWgt->id);
                }
                key_handled = true;
                break;
            case Key::Delete:
                if (kc.m_ctrl)
                    g.editState.str.trim(cursor_pos);
                else
                    g.editState.str.erase(cursor_pos);

                key_handled = true;
                g.pWndState->invalidate(pWgt->id);
                break;
            case Key::Up:
            case Key::Down:
                break;
            case Key::Left:
                if (cursor_pos > 0)
                {
                    cursor_pos --;
                    g.pWndState->invalidate(pWgt->id);
                }
                key_handled = true;
                break;
            case Key::Right:
                if (cursor_pos < (signed)g.editState.str.u8len())
                {
                    cursor_pos++;
                    g.pWndState->invalidate(pWgt->id);
                }
                key_handled = true;
                break;
            case Key::Home:
                cursor_pos = 0;
                g.pWndState->invalidate(pWgt->id);
                key_handled = true;
                break;
            case Key::End:
                cursor_pos = g.editState.str.u8len();
                g.pWndState->invalidate(pWgt->id);
                key_handled = true;
                break;
            default:
                break;
            }
        }
        else
        {
            g.editState.str.insert(cursor_pos, kc.utf8);
            cursor_pos++;
            g.pWndState->invalidate(pWgt->id);
            key_handled = true;
        }

        g.editState.cursorPos = cursor_pos;
    }
    else if (kc.key == Key::Enter)
    {
        // enter edit mode
        g.editState.pWgt = pWgt;
        g.pWndState->getEditText(pWgt, g.editState.str);
        g.editState.cursorPos = g.editState.str.u8len();
        g.pWndState->invalidate(pWgt->id);
        key_handled = true;
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
        g.pMouseDownWgt = pWgt;
        g.pWndState->onButtonDown(pWgt);
        g.pWndState->invalidate(pWgt->id, true);
        sleepMs(50);
        g.pMouseDownWgt = nullptr;
        g.pWndState->onButtonUp(pWgt);
        g.pWndState->invalidate(pWgt->id);
        return true;
    }

    return false;
}

static bool processKey_PageCtrl(const Widget *pWgt, const KeyCode &kc)
{
    if (kc.key == Key::PgDown || kc.key == Key::PgUp ||
        kc.key == Key::F11 || kc.key == Key::F12)
    {
        pgControlChangePage(pWgt, kc.key == Key::PgDown || kc.key == Key::F12);
        return true;
    }

    return false;
}

static bool processKey_ListBox(const Widget *pWgt, const KeyCode &kc)
{
    int delta = 0;
    const uint16_t items_visible = pWgt->size.height-2;

    switch (kc.key)
    {
    case Key::Enter:
    {
        int16_t idx = 0, selidx = 0, cnt = 0;
        g.pWndState->getListBoxState(pWgt, idx, selidx, cnt);
        if (cnt > 0)
        {
            if (selidx >= 0 && selidx != idx)
                g.pWndState->onListBoxChange(pWgt, selidx);
            g.pWndState->invalidate(pWgt->id);
        }
        return true;
    }
    case Key::Up:
        delta = kc.mod_all == KEY_MOD_SPECIAL ? -1 : 0;
        break;
    case Key::Down:
        delta = kc.mod_all == KEY_MOD_SPECIAL ? 1 : 0;
        break;
    case Key::PgUp:
        delta = kc.mod_all == KEY_MOD_SPECIAL ? -items_visible : 0;
        break;
    case Key::PgDown:
        delta = kc.mod_all == KEY_MOD_SPECIAL ? items_visible : 0;
        break;
    default:
        break;
    }

    if (delta != 0)
    {
        int16_t idx = 0, selidx = 0, cnt = 0;
        g.pWndState->getListBoxState(pWgt, idx, selidx, cnt);

        if (cnt > 0)
        {
            selidx += delta;

            if (selidx < 0)
                selidx = cnt - 1;

            if (selidx >= cnt)
                selidx = 0;

            g.pWndState->onListBoxSelect(pWgt, selidx);
            g.pWndState->invalidate(pWgt->id);
        }
        return true;
    }

    return false;
}

static bool processKey_ComboBox(const Widget *pWgt, const KeyCode &kc)
{
    int16_t idx = 0, selidx = 0, cnt = 0; bool drop_down = false;
    g.pWndState->getComboBoxState(pWgt, idx, selidx, cnt, drop_down);

    if (kc.utf8[0] == ' ')
    {
        if (cnt > 0)
        {
            drop_down = !drop_down;

            if (drop_down)
            {
                g.pWndState->onComboBoxDrop(pWgt, true);
                g.pDropDownCombo = pWgt;
            }
            else
            {
                comboBoxHideList(pWgt);
            }
        }
    }
    else if (kc.key == Key::Esc)
    {
        comboBoxHideList(pWgt);
    }
    else if (drop_down)
    {
        if (kc.key == Key::Up)
        {
            if (--selidx < 0) selidx = cnt-1;
            g.pWndState->onComboBoxSelect(pWgt, selidx);
        }
        else if (kc.key == Key::Down)
        {
            if (++selidx >= cnt) selidx = 0;
            g.pWndState->onComboBoxSelect(pWgt, selidx);
        }
        else if (kc.key == Key::PgUp && kc.mod_all == KEY_MOD_SPECIAL)
        {
            selidx -= pWgt->combobox.dropDownSize;
            if (selidx < 0) selidx = cnt-1;
            g.pWndState->onComboBoxSelect(pWgt, selidx);
        }
        else if (kc.key == Key::PgDown && kc.mod_all == KEY_MOD_SPECIAL)
        {
            selidx += pWgt->combobox.dropDownSize;
            if (selidx >= cnt) selidx = 0;
            g.pWndState->onComboBoxSelect(pWgt, selidx);
        }
        else if (kc.key == Key::Enter)
        {
            g.pWndState->onComboBoxChange(pWgt, selidx);
            comboBoxHideList(pWgt);
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }

    g.pWndState->invalidate(pWgt->id);
    return true;
}

static bool processKey_TextBox(const Widget *pWgt, const KeyCode &kc)
{
    int delta = 0;
    const uint16_t lines_visible = pWgt->size.height - 2;

    switch (kc.key)
    {
    case Key::Up:
        delta = kc.mod_all == KEY_MOD_SPECIAL ? -1 : 0;
        break;
    case Key::Down:
        delta = kc.mod_all == KEY_MOD_SPECIAL ? 1 : 0;
        break;
    case Key::PgUp:
        delta = kc.mod_all == KEY_MOD_SPECIAL ? -lines_visible : 0;
        break;
    case Key::PgDown:
        delta = kc.mod_all == KEY_MOD_SPECIAL ? lines_visible : 0;
        break;
    default:
        break;
    }

    if (delta != 0)
    {
        const twins::Vector<twins::StringRange> *p_lines = nullptr;
        bool changed = false;
        g.pWndState->getTextBoxLines(pWgt, &p_lines, changed);
        if (changed) g.textboxTopLine = 0;

        if (p_lines)
        {
            g.textboxTopLine += delta;

            if (g.textboxTopLine > (int)p_lines->size() - lines_visible)
                g.textboxTopLine = p_lines->size() - lines_visible;

            if (g.textboxTopLine < 0)
                g.textboxTopLine = 0;

            g.pWndState->invalidate(pWgt->id);
        }
        return true;
    }

    return false;
}

static bool processKey(const KeyCode &kc)
{
    auto focused_id = g.pWndState->getFocusedID();
    const Widget* p_wgt = getWidgetByWID(focused_id);
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
    case Widget::ComboBox:
        key_handled = processKey_ComboBox(p_wgt, kc);
        break;
    case Widget::TextBox:
        key_handled = processKey_TextBox(p_wgt, kc);
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
    if (kc.mouse.btn == MouseBtn::ButtonLeft)
    {
        changeFocusTo(pWgt->id);
    }
}

static void processMouse_CheckBox(const Widget *pWgt, const Rect &wgtRect, const KeyCode &kc)
{
    if (kc.mouse.btn == MouseBtn::ButtonLeft)
    {
        changeFocusTo(pWgt->id);
        g.pWndState->onCheckboxToggle(pWgt);
        g.pWndState->invalidate(pWgt->id);
    }
}

static void processMouse_Radio(const Widget *pWgt, const Rect &wgtRect, const KeyCode &kc)
{
    if (kc.mouse.btn == MouseBtn::ButtonLeft)
    {
        changeFocusTo(pWgt->id);
        g.pWndState->onRadioSelect(pWgt);
        invalidateRadioGroup(pWgt);
    }
}

static void processMouse_Button(const Widget *pWgt, const Rect &wgtRect, const KeyCode &kc)
{
    if (kc.mouse.btn == MouseBtn::ButtonLeft)
    {
        changeFocusTo(pWgt->id);
        g.pWndState->onButtonDown(pWgt);
        g.pWndState->invalidate(pWgt->id);
    }
    else if (kc.mouse.btn == MouseBtn::ButtonReleased && g.pMouseDownWgt == pWgt)
    {
        g.pWndState->onButtonUp(pWgt);
        g.pMouseDownWgt = nullptr;
        g.pWndState->invalidate(pWgt->id);
    }
    else
    {
        g.pMouseDownWgt = nullptr;
    }
}

static void processMouse_PageCtrl(const Widget *pWgt, const Rect &wgtRect, const KeyCode &kc)
{
    if (kc.mouse.btn == MouseBtn::ButtonLeft)
    {
        changeFocusTo(pWgt->id);
        int idx = g.pWndState->getPageCtrlPageIndex(pWgt);
        int new_idx = kc.mouse.row - wgtRect.coord.row - 1;

        if (new_idx != idx && new_idx >= 0 && new_idx < pWgt->link.childsCnt)
        {
            g.pWndState->onPageControlPageChange(pWgt, new_idx);
            g.pWndState->invalidate(pWgt->id);
        }
    }
    else if (kc.mouse.btn == MouseBtn::WheelUp || kc.mouse.btn == MouseBtn::WheelDown)
    {
        pgControlChangePage(pWgt, kc.mouse.btn == MouseBtn::WheelDown);
    }
}

static void processMouse_ListBox(const Widget *pWgt, const Rect &wgtRect, const KeyCode &kc)
{
    const uint16_t items_visible = pWgt->size.height-2;

    if (kc.mouse.btn == MouseBtn::ButtonLeft || kc.mouse.btn == MouseBtn::ButtonMid)
    {
        bool focus_changed = changeFocusTo(pWgt->id);

        int16_t idx = 0, selidx = 0, cnt = 0;
        g.pWndState->getListBoxState(pWgt, idx, selidx, cnt);

        if (cnt <= 0)
            return;

        int page = selidx / items_visible;
        unsigned new_selidx = page * items_visible;
        new_selidx += (int)kc.mouse.row - wgtRect.coord.row - 1;

        if (kc.mouse.btn == MouseBtn::ButtonLeft)
        {
            if (new_selidx < (unsigned)cnt && (((signed)new_selidx != selidx) || focus_changed))
            {
                selidx = new_selidx;
                g.pWndState->onListBoxSelect(pWgt, selidx);
            }
        }
        else
        {
            if (new_selidx < (unsigned)cnt && new_selidx != (unsigned)idx)
            {
                selidx = new_selidx;
                g.pWndState->onListBoxSelect(pWgt, selidx);
                g.pWndState->onListBoxChange(pWgt, selidx);
            }
        }

        g.pWndState->invalidate(pWgt->id);
    }
    else if (kc.mouse.btn == MouseBtn::WheelUp || kc.mouse.btn == MouseBtn::WheelDown)
    {
        changeFocusTo(pWgt->id);

        int16_t idx = 0, selidx = 0, cnt = 0;
        g.pWndState->getListBoxState(pWgt, idx, selidx, cnt);

        if (cnt <= 0)
            return;

        int delta = kc.mouse.btn == MouseBtn::WheelUp ? -1 : 1;
        if (kc.m_ctrl) delta *= items_visible;
        selidx += delta;

        if (selidx < 0)
            selidx = cnt - 1;

        if (selidx >= cnt)
            selidx = 0;

        g.pWndState->onListBoxSelect(pWgt, selidx);
        g.pWndState->invalidate(pWgt->id);
    }
}

static void processMouse_ComboBox(const Widget *pWgt, const Rect &wgtRect, const KeyCode &kc)
{
    if (kc.mouse.btn == MouseBtn::ButtonLeft)
    {
        changeFocusTo(pWgt->id);
        auto col = kc.mouse.col - wgtRect.coord.col;
        auto row = kc.mouse.row - wgtRect.coord.row - 1;

        if (row >= 0 && row < pWgt->combobox.dropDownSize)
        {
            int16_t idx = 0, selidx = 0, cnt = 0; bool drop_down = false;
            g.pWndState->getComboBoxState(pWgt, idx, selidx, cnt, drop_down);
            selidx = (selidx / pWgt->combobox.dropDownSize) * pWgt->combobox.dropDownSize; // top item
            selidx += row;
            if (selidx < cnt)
            {
                g.pWndState->onComboBoxSelect(pWgt, selidx);
                g.pWndState->invalidate(pWgt->id);
            }
        }
        else if (col >= wgtRect.size.width - 3 && col <= wgtRect.size.width - 1)
        {
            // drop down arrow clicked
            int16_t _, cnt = 0; bool drop_down = false;
            g.pWndState->getComboBoxState(pWgt, _, _, cnt, drop_down);

            if (cnt <= 0)
                return;

            drop_down = !drop_down;

            if (drop_down)
            {
                g.pWndState->onComboBoxDrop(pWgt, true);
                g.pWndState->invalidate(pWgt->id);
                g.pDropDownCombo = pWgt;
            }
            else
            {
                comboBoxHideList(pWgt);
            }
        }
    }
    else if (kc.mouse.btn == MouseBtn::WheelUp || kc.mouse.btn == MouseBtn::WheelDown)
    {
        changeFocusTo(pWgt->id);

        int16_t idx = 0, selidx = 0, cnt = 0; bool drop_down = false;
        g.pWndState->getComboBoxState(pWgt, idx, selidx, cnt, drop_down);

        if (!drop_down || cnt <= 0)
            return;

        int delta = kc.mouse.btn == MouseBtn::WheelUp ? -1 : 1;
        if (kc.m_ctrl) delta *= pWgt->combobox.dropDownSize;
        selidx += delta;

        if (selidx < 0)
            selidx = cnt - 1;

        if (selidx >= cnt)
            selidx = 0;

        g.pWndState->onComboBoxSelect(pWgt, selidx);
        g.pWndState->invalidate(pWgt->id);
    }
    else if (kc.mouse.btn == MouseBtn::ButtonMid)
    {
        changeFocusTo(pWgt->id);

        int16_t _, selidx = 0; bool drop_down = false;
        g.pWndState->getComboBoxState(pWgt, _, selidx, _, drop_down);

        if (!drop_down)
            return;

        g.pWndState->onComboBoxChange(pWgt, selidx);
        comboBoxHideList(pWgt);
    }
}

static void processMouse_CustomWgt(const Widget *pWgt, const Rect &wgtRect, const KeyCode &kc)
{
    g.pWndState->onCustomWidgetInputEvt(pWgt, kc);
}

static void processMouse_TextBox(const Widget *pWgt, const Rect &wgtRect, const KeyCode &kc)
{
    changeFocusTo(pWgt->id);

    if (kc.mouse.btn == MouseBtn::WheelUp || kc.mouse.btn == MouseBtn::WheelDown)
    {
        const twins::Vector<twins::StringRange> *p_lines = nullptr;
        bool changed = false;
        g.pWndState->getTextBoxLines(pWgt, &p_lines, changed);
        if (changed) g.textboxTopLine = 0;

        if (p_lines && p_lines->size())
        {
            int delta = kc.mouse.btn == MouseBtn::WheelUp ? -1 : 1;
            const uint16_t lines_visible = pWgt->size.height - 2;
            if (kc.m_ctrl) delta *= lines_visible;

            g.textboxTopLine += delta;

            if (g.textboxTopLine > (int)p_lines->size() - lines_visible)
                g.textboxTopLine = p_lines->size() - lines_visible;

            if (g.textboxTopLine < 0)
                g.textboxTopLine = 0;

            changeFocusTo(pWgt->id);
            g.pWndState->invalidate(pWgt->id);
        }
    }
}


static bool processMouse(const KeyCode &kc)
{
    if (kc.mouse.btn == MouseBtn::ButtonGoBack || kc.mouse.btn == MouseBtn::ButtonGoForward)
    {
        if (const auto *p_wgt = findMainPgControl())
        {
            pgControlChangePage(p_wgt, kc.mouse.btn == MouseBtn::ButtonGoForward);
            return true;
        }
    }

    Rect rct;
    const Widget *p_wgt = getWidgetAt(kc.mouse.col, kc.mouse.row, rct);

    if (g.pMouseDownWgt)
    {
        // apply only for Button widget
        if (g.pMouseDownWgt->type == Widget::Button)
        {
            // mouse button released over another widget - generate event for previously clicked button
            if (kc.mouse.btn == MouseBtn::ButtonReleased && g.pMouseDownWgt != p_wgt)
                p_wgt = g.pMouseDownWgt;
        }
    }
    else if (p_wgt)
    {
        // remember clicked widget
        if (kc.mouse.btn >= MouseBtn::ButtonLeft && kc.mouse.btn < MouseBtn::ButtonReleased)
            g.pMouseDownWgt = p_wgt;
    }

    if (!p_wgt)
        return false;

    //TWINS_LOG("WidgetAt(%2d:%2d)=%s ID:%u", kc.mouse.col, kc.mouse.row, toString(p_wgt->type), p_wgt->id);

    if (g.pDropDownCombo && (p_wgt->type != Widget::ComboBox))
    {
        // check if drop-down list clicked
        Rect dropdownlist_rct;
        dropdownlist_rct.coord = getScreenCoord(g.pDropDownCombo);
        dropdownlist_rct.coord.row++;
        dropdownlist_rct.size.width = g.pDropDownCombo->size.width;
        dropdownlist_rct.size.height = g.pDropDownCombo->combobox.dropDownSize;

        if (isPointWithin(kc.mouse.col, kc.mouse.row, dropdownlist_rct))
        {
            // yes -> replace data for processing with g.pDropDownCombo
            p_wgt = g.pDropDownCombo;
            rct.coord = getScreenCoord(g.pDropDownCombo);
            rct.size = g.pDropDownCombo->size;
        }
        else
        {
            comboBoxHideList(g.pDropDownCombo);
        }
    }

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
    case Widget::ComboBox:
        processMouse_ComboBox(p_wgt, rct, kc);
        break;
    case Widget::CustomWgt:
        processMouse_CustomWgt(p_wgt, rct, kc);
        break;
    case Widget::TextBox:
        processMouse_TextBox(p_wgt, rct, kc);
        break;
    default:
        moveToHome();
        g.pMouseDownWgt = nullptr;
        return false;
    }

    if (kc.mouse.btn == MouseBtn::ButtonReleased)
        g.pMouseDownWgt = nullptr;

    return true;
}

// -----------------------------------------------------------------------------
// ---- TWINS  P U B L I C  FUNCTIONS ------------------------------------------
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
    CASE_WGT_STR(ComboBox)
    CASE_WGT_STR(CustomWgt)
    CASE_WGT_STR(TextBox)
    default: return "?";
    }
}

Coord getScreenCoord(const Widget *pWgt)
{
    Coord coord = pWgt->coord;
    if (pWgt->type == Widget::Type::Window)
        g.pWndState->getWindowCoord(pWgt, coord);

    if (pWgt->link.ownIdx > 0)
    {
        // go up the widgets hierarchy
        const auto *p_parent = getParent(pWgt);

        for (;;)
        {
            if (p_parent->type == Widget::Type::Window)
            {
                Coord wnd_coord = p_parent->coord;
                // getWindowCoord is optional
                g.pWndState->getWindowCoord(p_parent, wnd_coord);
                coord += wnd_coord;
            }
            else
            {
                coord += p_parent->coord;
            }

            if (p_parent->type == Widget::Type::PageCtrl)
            {
                coord.col += p_parent->pagectrl.tabWidth;
            }

            // top-level parent reached
            if (p_parent->link.ownIdx == 0)
                break;

            p_parent = getParent(p_parent);
        }
    }

    return coord;
}

WID getPageID(const Widget *pPageControl, int8_t pageIdx)
{
    assert(pPageControl);
    assert(pPageControl->type == Widget::PageCtrl);

    if (pageIdx < 0 || pageIdx >= pPageControl->link.childsCnt)
        return WIDGET_ID_NONE;

    const Widget *p_page = pPageControl;
    p_page += (pPageControl->link.childsIdx - pPageControl->link.ownIdx) + pageIdx;
    return p_page->id;
}

const Widget* getWidget(const Widget *pWindowWidgets, WID widgetId)
{
    assert(pWindowWidgets);
    assert(pWindowWidgets->type == Widget::Window);

    const Widget *p_arr_bkp = g.pWndWidgets;
    g.pWndWidgets = pWindowWidgets;
    const auto *p_wgt = getWidgetByWID(widgetId);
    g.pWndWidgets = p_arr_bkp;
    return p_wgt;
}

bool processKey(const Widget *pWindowWidgets, const KeyCode &kc)
{
    assert(pWindowWidgets);
    assert(pWindowWidgets->type == Widget::Window);
    g.pWndWidgets = pWindowWidgets;
    g.pWndState = pWindowWidgets->window.getState();
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
            if (g.pDropDownCombo)
            {
                comboBoxHideList(g.pDropDownCombo);
            }

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
            default:
                break;
            }
        }

        if (!key_processed)
            key_processed = g.pWndState->onWindowUnhandledInputEvt(g.pWndWidgets, kc);
    }

    g.pWndWidgets = nullptr; g.pWndState = nullptr;
    return key_processed;
}

void mainPgControlChangePage(const Widget *pWindowWidgets, bool next)
{
    assert(pWindowWidgets);
    g.pWndWidgets = pWindowWidgets;
    g.pWndState = pWindowWidgets->window.getState();
    assert(g.pWndState);

    if (const auto *p_wgt = findMainPgControl())
        pgControlChangePage(p_wgt, next);

    g.pWndWidgets = nullptr; g.pWndState = nullptr;
}

// -----------------------------------------------------------------------------

}
