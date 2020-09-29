/******************************************************************************
 * @brief   TWins - demo application
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#include "demo_wnd.hpp"

#include "twins.hpp"
#include "twins_ringbuffer.hpp"
#include "twins_pal_defimpl.hpp"
#include "twins_vector.hpp"
#include "twins_map.hpp"
#include "twins_utils.hpp"
#include "twins_wndstack.hpp"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <functional>
#include <mutex>

// -----------------------------------------------------------------------------

void showPopup(twins::String title, twins::String message, std::function<void()> onYes, std::function<void()> onNo);

static twins::WndStack wndStack;


// state of Main window
class WndMainState : public twins::IWindowState
{
public:
    void init()
    {
        focusedId.resize(wndMainNumPages);

        for (auto &wid : focusedId)
            // wid = twins::WIDGET_ID_NONE;
            wid = ID_WND;

        txtBox1Text = ESC_BOLD
                    "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nam arcu magna, placerat sit amet libero at, aliquam fermentum augue.\n"
                    ESC_NORMAL
                    ESC_FG_Gold
                    "Morbi egestas consectetur malesuada. Mauris vehicula, libero eget tempus ullamcorper, nisi lorem efficitur velit, vel bibendum augue eros vel lorem. Duis vestibulum magna a ornare bibendum. Curabitur eleifend dictum odio, eu ultricies nunc eleifend et.\n"
                    "Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis egestas.\n"
                    ESC_FG_GreenYellow
                    "Interdum et malesuada fames ac ante ipsum primis in faucibus. Aenean malesuada lacus leo, a eleifend lorem suscipit sed.\n" "▄";
        txtBox2Text = "Lorem ipsum ▄";
        edt1Text = "00 11 22 33 44 55 66 77 88 99 aa bb cc dd";
        edt2Text = "73+37=100";
        initialized = true;
    }

    ~WndMainState()
    {
        printf("WgtProperty map Distribution=%u%% Buckets:%u Nodes:%u\n",
            wgtProp.distribution(), wgtProp.bucketsCount(), wgtProp.size());
    }

    // --- events ---

    void onButtonDown(const twins::Widget* pWgt) override
    {
        if (pWgt->id == ID_BTN_YES)     TWINS_LOG("BTN_YES");
        if (pWgt->id == ID_BTN_NO)      TWINS_LOG("BTN_NO");
        if (pWgt->id == ID_BTN_POPUP)   TWINS_LOG("BTN_CANCEL");
    }

    void onButtonUp(const twins::Widget* pWgt) override
    {
        if (pWgt->id == ID_BTN_YES)     TWINS_LOG("BTN_YES");
        if (pWgt->id == ID_BTN_NO)      TWINS_LOG("BTN_NO");
        if (pWgt->id == ID_BTN_POPUP)
        {
            TWINS_LOG("BTN_POPUP");
            showPopup("Lorem Titlum",
                "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. "
                "Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.",
                [] { TWINS_LOG(ESC_BG_DarkGreen "Choice: YES" ESC_BG_DEFAULT); },
                [] { TWINS_LOG(ESC_BG_Firebrick "Choice: NO" ESC_BG_DEFAULT); }
            );
        }
    }

    void onEditChange(const twins::Widget* pWgt, twins::String &&str) override
    {
        switch (pWgt->id)
        {
        case ID_EDT_1: edt1Text = std::move(str); break;
        case ID_EDT_2: edt2Text = std::move(str); break;
        default: break;
        }
        TWINS_LOG("value:%s", str.cstr());
    }

    bool onEditInputEvt(const twins::Widget* pWgt, const twins::KeyCode &kc, twins::String &str, int16_t &cursorPos) override
    {
        if (pWgt->id == ID_EDT_2)
            return twins::util::numEditInputEvt(kc, str, cursorPos);

        return false;
    }

    void onCheckboxToggle(const twins::Widget* pWgt) override
    {
        switch (pWgt->id)
        {
        case ID_CHBX_ENBL: TWINS_LOG("CHBX_ENBL"); break;
        case ID_CHBX_LOCK: TWINS_LOG("CHBX_LOCK"); break;
        default: TWINS_LOG("CHBX"); break;
        }

        wgtProp[pWgt->id].chbx.checked = !wgtProp[pWgt->id].chbx.checked;
    }

    void onPageControlPageChange(const twins::Widget* pWgt, uint8_t newPageIdx) override
    {
        if (pWgt->id == ID_PGCONTROL) pgcPage = newPageIdx;
    }

    void onListBoxSelect(const twins::Widget* pWgt, int16_t selIdx) override
    {
        wgtProp[pWgt->id].lbx.selIdx = selIdx;
        TWINS_LOG("LISTBOX_SELECT(%u)", selIdx);
    }

    void onListBoxChange(const twins::Widget* pWgt, int16_t newIdx) override
    {
        if (pWgt->id == ID_LISTBOX)
        {
            wgtProp[pWgt->id].lbx.itemIdx = newIdx;
            TWINS_LOG("LISTBOX_CHANGE(%u)", newIdx);
        }
    }

    void onComboBoxSelect(const twins::Widget* pWgt, int16_t selIdx) override
    {
        wgtProp[pWgt->id].cbbx.selIdx = selIdx;
        TWINS_LOG("COMBOBOX_SELECT(%u)", selIdx);
    }

    void onComboBoxChange(const twins::Widget* pWgt, int16_t newIdx) override
    {
        wgtProp[pWgt->id].cbbx.itemIdx = newIdx;
        TWINS_LOG("COMBOBOX_CHANGE(%u)", newIdx);
    }

    void onComboBoxDrop(const twins::Widget* pWgt, bool dropState) override
    {
        wgtProp[pWgt->id].cbbx.dropDown = dropState;
        if (dropState)
            wgtProp[pWgt->id].cbbx.selIdx = wgtProp[pWgt->id].cbbx.itemIdx;

        TWINS_LOG("COMBOBOX_DROP(%u)", dropState);
    }

    void onRadioSelect(const twins::Widget* pWgt) override
    {
        TWINS_LOG("RADIO_SELECT(%d)", pWgt->radio.radioId);
        if (pWgt->radio.groupId == 1)
            radioId = pWgt->radio.radioId;
    }

    void onCustomWidgetDraw(const twins::Widget* pWgt) override
    {
        auto coord = twins::getScreenCoord(pWgt);
        auto sz = pWgt->size;

        twins::moveTo(coord.col, coord.row);
        twins::writeChar('-', sz.width);
        twins::moveTo(coord.col, coord.row + sz.height);
        twins::writeChar('-', sz.width);
    }

    bool onCustomWidgetInputEvt(const twins::Widget* pWgt, const twins::KeyCode &kc) override
    {
        twins::moveTo(kc.mouse.col, kc.mouse.row);
        twins::writeChar('0' + (int)kc.mouse.btn);
        return true;
    }

    bool onWindowUnhandledInputEvt(const twins::Widget* pWgt, const twins::KeyCode &kc) override
    {
        TWINS_LOG("onWindowUnhandledInputEvt(%s)", kc.name);
        return false;
    }

    // --- widgets state queries ---

    bool isEnabled(const twins::Widget* pWgt) override
    {
        if (pWgt->id == ID_PANEL_VERSIONS)
        {
            auto &prp = wgtProp[pWgt->id];
            prp.pnl.enabled = !prp.pnl.enabled;
            return prp.pnl.enabled;
        }

        if (pWgt->id == ID_CHBX_C)
            return false;

        return true;
    }

    bool isFocused(const twins::Widget* pWgt) override
    {
        return pWgt->id == focusedId[pgcPage];
    }

    bool isVisible(const twins::Widget* pWgt) override
    {
        //if (pWgt->id == ID_WND)    return currWnd.window() == pWgt; // always visible
        if (pWgt->id == ID_PAGE_1) return pgcPage == 0;
        if (pWgt->id == ID_PAGE_2) return pgcPage == 1;
        if (pWgt->id == ID_PAGE_3) return pgcPage == 2;
        if (pWgt->id == ID_PAGE_4) return pgcPage == 3;
        if (pWgt->id == ID_PAGE_5) return pgcPage == 4;
        if (pWgt->id == ID_PAGE_6) return pgcPage == 5;

        return true;
    }

    twins::WID& getFocusedID() override
    {
        return focusedId[pgcPage];
    }

    bool getCheckboxChecked(const twins::Widget* pWgt) override
    {
        return wgtProp[pWgt->id].chbx.checked;
    }

    void getLabelText(const twins::Widget* pWgt, twins::String &out) override
    {
        out.clear();

        if (pWgt->id == ID_LABEL_KEYSEQ)
        {
            out.appendFmt("SEQ[%zu]:", lblKeycodeSeq.size());

            for (unsigned i = 0; i < lblKeycodeSeq.size(); i++)
            {
                uint8_t c = lblKeycodeSeq.cstr()[i];
                if (!c) break;

                if (c < ' ')
                    out.appendFmt("\\x%02x", c);
                else if (c != (char)twins::Ansi::DEL)
                    out.append(c);
            }
        }

        if (pWgt->id == ID_LABEL_KEYNAME)
        {
            out.appendFmt("KEY[%zu]:%s", lblKeyName.size(), lblKeyName.cstr());
        }

        if (pWgt->id == ID_LBL_EMPTY_1)
        {
            const char *s =
                ESC_BOLD "Name:\n" ESC_NORMAL
                "  20 Hits on 2\n"
                ESC_BOLD "Description:\n" ESC_NORMAL
                "  Latest, most lo♡ed radio hits. ❤"
                ;
                // note: the ❤ is double-width glyph (U+1F90D)
            out = twins::util::wordWrap(s, pWgt->size.width, " \n", "\n  ");
        }
    }

    void getEditText(const twins::Widget* pWgt, twins::String &out) override
    {
        switch (pWgt->id)
        {
        case ID_EDT_1: out = edt1Text; break;
        case ID_EDT_2: out = edt2Text; break;
        default: break;
        }
    }

    bool getLedLit(const twins::Widget* pWgt) override
    {
        auto &prp = wgtProp[pWgt->id];
        prp.led.lit = !prp.led.lit;
        return prp.led.lit;
    }

    void getProgressBarState(const twins::Widget*, int &pos, int &max) override
    {
        pos = pgbarPos++;
        max = 20;
        if (pgbarPos > max) pgbarPos = 0;
    }

    int getPageCtrlPageIndex(const twins::Widget* pWgt) override
    {
        return pgcPage;
    }

    void getListBoxState(const twins::Widget* pWgt, int16_t &itemIdx, int16_t &selIdx, int16_t &itemsCount) override
    {
        itemIdx = wgtProp[pWgt->id].lbx.itemIdx;
        selIdx = wgtProp[pWgt->id].lbx.selIdx;
        itemsCount = listBoxItemsCount;
    }

    void getListBoxItem(const twins::Widget*, int itemIdx, twins::String &out) override
    {
        if (itemIdx == 3)
            out.appendFmt(ESC_BOLD "Item" ESC_NORMAL " 0034567890123456789*");
        else
            out.appendFmt(ESC_FG_BLACK "Item" ESC_FG_BLUE " %03d", itemIdx);
    }

    void getComboBoxState(const twins::Widget* pWgt, int16_t &itemIdx, int16_t &selIdx, int16_t &itemsCount, bool &dropDown) override
    {
        itemIdx = wgtProp[pWgt->id].cbbx.itemIdx;
        selIdx = wgtProp[pWgt->id].cbbx.selIdx;
        itemsCount = 6;
        dropDown = wgtProp[pWgt->id].cbbx.dropDown;
    }

    void getComboBoxItem(const twins::Widget*, int itemIdx, twins::String &out) override
    {
        out.appendFmt("Option %03d", itemIdx);
    }

    int getRadioIndex(const twins::Widget* pWgt) override
    {
        return radioId;
    }

    void getTextBoxLines(const twins::Widget* pWgt, const twins::Vector<twins::StringRange> **ppLines, bool &changed) override
    {
        if (pWgt->id == ID_TBX_LOREMIPSUM)
        {
            changed = txtBox1Text.isDirty();
            txtBox1Text.config(pWgt->size.width-2, " \n");
            *ppLines = &txtBox1Text.getLines();
        }
        else
        {
            changed = txtBox2Text.isDirty();
            txtBox2Text.config(pWgt->size.width-2, " \n");
            *ppLines = &txtBox2Text.getLines();
        }
    }

    // --- requests ---

    void invalidate(twins::WID id, bool instantly) override
    {
        if (id == twins::WIDGET_ID_NONE)
        {
            invalidatedWgts.clear();
            return;
        }

        // state or focus changed - widget must be repainted
        if (instantly)
        {
            twins::drawWidget(pWndMainWidgets, id);
            twins::flushBuffer();
        }
        else
        {
            if (!invalidatedWgts.contains(id))
                invalidatedWgts.append(id);
        }
    }

public:
    twins::String lblKeycodeSeq;
    twins::String lblKeyName;
    bool initialized = false;
    twins::Vector<twins::WID> invalidatedWgts;

private:
    union WgtProp
    {
        struct
        {
            bool checked;
        } chbx;

        struct
        {
            bool lit;
        } led;

        struct
        {
            bool enabled;
        } pnl;

        struct
        {
            int16_t itemIdx;
            int16_t selIdx;
        } lbx;

        struct
        {
            int16_t itemIdx;
            int16_t selIdx;
            bool    dropDown;
        } cbbx;
    };

    int  pgbarPos = 0;
    int  pgcPage = 0;
    int  radioId = 0;
    int16_t listBoxItemsCount = 20;
    twins::String edt1Text;
    twins::String edt2Text;
    twins::Map<twins::WID, WgtProp> wgtProp;
    twins::util::WrappedString txtBox1Text;
    twins::util::WrappedString txtBox2Text;

    // focused WID separate for each page
    using wids_t = twins::Vector<twins::WID>;
    wids_t focusedId;
};

// state of Popup window
class WndYesNoState : public twins::IWindowState
{
public:
    void init()
    {
        focusedId = IDYN_WND;
        initialized = true;
    }

    // --- events ---

    void onButtonUp(const twins::Widget* pWgt) override
    {
        if (pWgt->id == IDYN_BTN_YES && onYES)
            onYES();

        if (pWgt->id == IDYN_BTN_NO && onNO)
            onNO();

        wndStack.popWnd();
    }

    bool onWindowUnhandledInputEvt(const twins::Widget* pWgt, const twins::KeyCode &kc) override
    {
        if (kc.key == twins::Key::Esc)
        {
            wndStack.popWnd();
            return true;
        }
        return false;
    }

    // --- widgets state queries ---

    void getWindowCoord(const twins::Widget* pWgt, twins::Coord &coord) override
    {
        // calc location on the main window center
        coord.col = (pWndMainWidgets[0].size.width - pWgt->size.width) / 2;
        coord.col += pWndMainWidgets[0].coord.col;
        coord.row = (pWndMainWidgets[0].size.height - pWgt->size.height) / 2;
        coord.row += pWndMainWidgets[0].coord.row;
    }

    void getWindowTitle(const twins::Widget* pWgt, twins::String &title) override
    {
        title = wndTitle;
    }

    twins::WID& getFocusedID() override
    {
        return focusedId;
    }

    bool isFocused(const twins::Widget* pWgt) override
    {
        return pWgt->id == focusedId;
    }

    bool isVisible(const twins::Widget* pWgt) override
    {
        if (pWgt->id == IDYN_WND)
            return wndStack.window() == pWgt;

        return true;
    }

    void getLabelText(const twins::Widget* pWgt, twins::String &out) override
    {
        if (pWgt->id == IDYN_LBL_MSG)
        {
            out = twins::util::wordWrap(wndMessage.cstr(), pWgt->size.width);
        }
    }

    // --- requests ---

    void invalidate(twins::WID id, bool /* instantly */) override
    {
        // state or focus changed - widget must be repainted
        twins::drawWidget(pWndYesNoWidgets, id);
        twins::flushBuffer();
    }

public:
    bool initialized = false;
    std::function<void()> onYES;
    std::function<void()> onNO;
    twins::String wndTitle;
    twins::String wndMessage;

private:
    twins::WID focusedId;
};

// -----------------------------------------------------------------------------

static WndMainState wndMainState;
static WndYesNoState wndYesNoState;

twins::IWindowState * getWndMainState()
{
    if (!wndMainState.initialized)
        wndMainState.init();
    return &wndMainState;
}

twins::IWindowState * getWndYesNoState()
{
    if (!wndYesNoState.initialized)
        wndYesNoState.init();
    return &wndYesNoState;
}

void showPopup(twins::String title, twins::String message, std::function<void()> onYes, std::function<void()> onNo)
{
    wndYesNoState.wndTitle = std::move(title);
    wndYesNoState.wndMessage = std::move(message);
    wndYesNoState.onYES = onYes;
    wndYesNoState.onNO = onNo;

    wndStack.pushWnd(pWndYesNoWidgets);
}

// -----------------------------------------------------------------------------

twins::RingBuff<char> rbKeybInput;

struct DemoPAL : twins::DefaultPAL
{
    DemoPAL() { }

    ~DemoPAL()
    {
        printf(ESC_BOLD "lineBuffMaxSize: %u\n" ESC_NORMAL, lineBuffMaxSize);
    }

    uint16_t getLogsRow() override
    {
        return pWndMainWidgets[0].coord.row + pWndMainWidgets[0].size.height + 1;
    }

    bool lock(bool wait) override
    {
        if (wait)
        {
            mtx.lock();
            return true;
        }

        return mtx.try_lock();
    }

    void unlock() override
    {
        mtx.unlock();
    }

private:
    std::recursive_mutex mtx;
};

static DemoPAL demo_pal;

// -----------------------------------------------------------------------------

#include "twins_input_posix.hpp"

int main()
{
    // printf("Win1 controls: %u" "\n", wndMain.window.childCount);
    // printf("sizeof Widget: %zu" "\n", sizeof(twins::Widget));

    twins::init(&demo_pal);
    twins::screenClrAll();
    wndStack.pushWnd(pWndMainWidgets);

    twins::inputPosixInit(100);
    twins::mouseMode(twins::MouseMode::M2);
    rbKeybInput.init(20);
    twins::flushBuffer();

    for (;;)
    {
        bool quit_req = false;
        twins::KeyCode kc = {};
        const char *posix_inp = twins::inputPosixRead(quit_req);
        if (quit_req) break;
        rbKeybInput.write(posix_inp);

        if (rbKeybInput.size())
        {
            twins::Locker lck;

            // display input buffer
            {
                char seq[10];
                rbKeybInput.copy(seq, sizeof(seq)-1);
                seq[sizeof(seq)-1] = '\0';
                wndMainState.lblKeycodeSeq = seq;
            }

            twins::decodeInputSeq(rbKeybInput, kc);
            // pass key to top-window
            bool key_handled = twins::processKey(wndStack.window(), kc);
            wndMainState.lblKeyName = kc.name;

            // display decoded key
            if (kc.key == twins::Key::MouseEvent)
            {
                TWINS_LOG("B%c %c%c%c %03d:%03d",
                    '0' + (char)kc.mouse.btn,
                    kc.m_ctrl ? 'C' : ' ',
                    kc.m_alt ? 'A' : ' ',
                    kc.m_shift ? 'S' : ' ',
                    kc.mouse.col, kc.mouse.row);
            }
            else if (kc.key != twins::Key::None)
            {
                TWINS_LOG("Key: '%s' %s", kc.name, key_handled ? "(handled)" : "");
            }



            if (kc.m_spec && kc.key == twins::Key::F4)
            {
                static bool mouse_on = true;
                mouse_on = !mouse_on;
                TWINS_LOG("Mouse %s", mouse_on ? "ON" : "OFF");
                twins::mouseMode(mouse_on ? twins::MouseMode::M2 : twins::MouseMode::Off);
                twins::flushBuffer();
            }
            else if (kc.m_spec && kc.key == twins::Key::F5)
            {
                twins::screenClrAll();
                twins::flushBuffer();

                // draw windows from bottom to top
                wndStack.redraw();
            }
            else if (kc.m_spec && kc.key == twins::Key::F6)
            {
                twins::cursorSavePos();
                twins::moveTo(0, demo_pal.getLogsRow());
                twins::screenClrBelow();
                twins::cursorRestorePos();
            }
            else if (kc.m_spec && kc.m_ctrl && (kc.key == twins::Key::PgUp || kc.key == twins::Key::PgDown))
            {
                if (wndStack.window() == pWndMainWidgets)
                    twins::mainPgControlChangePage(pWndMainWidgets, kc.key == twins::Key::PgDown);
            }
            else if (kc.m_spec && (kc.key == twins::Key::F9 || kc.key == twins::Key::F10))
            {
                if (wndStack.window() == pWndMainWidgets)
                    twins::mainPgControlChangePage(pWndMainWidgets, kc.key == twins::Key::F10);
            }


            if (wndStack.window() == pWndMainWidgets)
            {
                // keyboard code
                twins::cursorSavePos();
                twins::drawWidgets(pWndMainWidgets, {ID_LABEL_KEYSEQ, ID_LABEL_KEYNAME});

                if (kc.mod_all != KEY_MOD_SPECIAL)
                {
                    twins::drawWidgets(pWndMainWidgets,
                    {
                        ID_LED_LOCK, ID_LED_BATTERY, ID_LED_PUMP,
                        ID_PRGBAR_1, ID_PRGBAR_2, ID_PRGBAR_3,
                        ID_PANEL_VERSIONS,
                    });
                }
                twins::cursorRestorePos();
            }

            if (wndStack.window() == pWndMainWidgets)
            {
                twins::drawWidgets(pWndMainWidgets, wndMainState.invalidatedWgts.data(), wndMainState.invalidatedWgts.size());
                wndMainState.invalidatedWgts.clear();
            }
        }

        twins::flushBuffer();
    }

    twins::moveTo(0, demo_pal.getLogsRow());
    twins::screenClrBelow();
    twins::mouseMode(twins::MouseMode::Off);
    twins::flushBuffer();
    twins::inputPosixFree();

    printf(ESC_BOLD "Memory stats: max chunks: %d, max allocated: %d B\n" ESC_NORMAL,
        demo_pal.stats.memChunksMax, demo_pal.stats.memAllocatedMax
    );
}
