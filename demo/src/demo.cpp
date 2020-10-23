/******************************************************************************
 * @brief   TWins - demo application
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#include "twins.hpp"
#include "twins_ringbuffer.hpp"
#include "twins_vector.hpp"
#include "twins_map.hpp"
#include "twins_utils.hpp"
#include "twins_input_posix.hpp"
#include "twins_pal_defimpl.hpp"
#include "twins_glob.hpp"
#include "twins_window_state_base.hpp"

#include "demo_wnd.hpp"

#include <stdio.h>
#include <functional>
#include <mutex>

// -----------------------------------------------------------------------------

struct DemoPAL : twins::DefaultPAL
{
    DemoPAL()
    {
        twins::init(this);
    }

    ~DemoPAL()
    {
        printf("~DemoPAL() lineBuffMaxSize: %u\n", lineBuffMaxSize);
        deinit();
        twins::deinit();
    }

    uint16_t getLogsRow() override
    {
        const auto *p_wnd = twins::glob::pMainWindowWgts;
        return p_wnd->coord.row + p_wnd->size.height + 1;
    }

    bool lock(bool wait) override
    {
        if (wait)
        {
            mMtx.lock();
            return true;
        }

        return mMtx.try_lock();
    }

    void unlock() override
    {
        mMtx.unlock();
    }

private:
    std::recursive_mutex mMtx;
};


// local definition of twins::glob namespace for Demo needs
namespace twins
{
namespace glob
{
DemoPAL demoPAL; // must be first due to construction-destruction order
twins::WndManager wndMngr;

twins::IPal& pal = demoPAL;
twins::WndManager& wMngr = wndMngr;
const twins::Widget* pMainWindowWgts = pWndMainWidgets;
}
}


void showPopup(twins::String title, twins::String message, std::function<void(twins::WID btnID)> onButton, const char *buttons = "");

// -----------------------------------------------------------------------------

// state of Main window
class WndMainState : public twins::WindowStateBase
{
public:
    void init(const twins::Widget *pWindowWgts) override
    {
        WindowStateBase::init(pWindowWgts);
        mFocusedIds.resize(wndMainNumPages);

        for (auto &wid : mFocusedIds)
            // wid = twins::WIDGET_ID_NONE;
            wid = ID_WND;

        mTxtBox1Text = ESC_BOLD
                    "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nam arcu magna, placerat sit amet libero at, aliquam fermentum augue.\n"
                    ESC_NORMAL
                    ESC_FG_Gold
                    "Morbi egestas consectetur malesuada. Mauris vehicula, libero eget tempus ullamcorper, nisi lorem efficitur velit, vel bibendum augue eros vel lorem. Duis vestibulum magna a ornare bibendum. Curabitur eleifend dictum odio, eu ultricies nunc eleifend et.\n"
                    "Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis egestas.\n"
                    ESC_FG_GreenYellow
                    "Interdum et malesuada fames ac ante ipsum primis in faucibus. Aenean malesuada lacus leo, a eleifend lorem suscipit sed.\n" "▄";
        mTxtBox2Text = "Lorem ipsum ▄";
        mEdt1Text = "00 11 22 33 44 55 66 77 88 99 aa bb cc dd";
        mEdt2Text = "73+37=100";
    }

    ~WndMainState()
    {
        printf("~WndMainState()  WgtProperty map Distribution=%u%% Buckets:%u Nodes:%u\n",
            mWgtProp.distribution(), mWgtProp.bucketsCount(), mWgtProp.size());
    }

    // --- events ---

    void onButtonDown(const twins::Widget* pWgt, const twins::KeyCode &kc) override
    {
        if (pWgt->id == ID_BTN_YES)     TWINS_LOG("▼ BTN_YES");
        if (pWgt->id == ID_BTN_NO)      TWINS_LOG("▼ BTN_NO");
        if (pWgt->id == ID_BTN_POPUP)   TWINS_LOG("▼ BTN_POPUP");
    }

    void onButtonUp(const twins::Widget* pWgt, const twins::KeyCode &kc) override
    {
        if (pWgt->id == ID_BTN_YES)     TWINS_LOG("▲ BTN_YES");
        if (pWgt->id == ID_BTN_NO)      TWINS_LOG("▲ BTN_NO");
        if (pWgt->id == ID_BTN_POPUP)   TWINS_LOG("▲ BTN_POPUP");

        if (pWgt->id == ID_BTN_SAYNO)
        {
            auto &prp = mWgtProp[ID_BTN_SAYYES];
            prp.enabled = !prp.enabled;
            invalidate(ID_BTN_SAYYES);
        }
    }

    void onButtonClick(const twins::Widget* pWgt, const twins::KeyCode &kc) override
    {
        TWINS_LOG("BTN_CLICK");

        if (pWgt->id == ID_BTN_POPUP)
        {
            showPopup("Lorem Titlum",
                "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. "
                "Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.",
                [](twins::WID btnID) { TWINS_LOG(ESC_BG_DarkGreen "Choice: %d" ESC_BG_DEFAULT, btnID); },
                "ync"
            );
        }
    }

    void onEditChange(const twins::Widget* pWgt, twins::String &&str) override
    {
        switch (pWgt->id)
        {
        case ID_EDT_1: mEdt1Text = std::move(str); break;
        case ID_EDT_2: mEdt2Text = std::move(str); break;
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

        mWgtProp[pWgt->id].chbx.checked = !mWgtProp[pWgt->id].chbx.checked;
    }

    void onPageControlPageChange(const twins::Widget* pWgt, uint8_t newPageIdx) override
    {
        if (pWgt->id == ID_PGCONTROL) mPgcPage = newPageIdx;
    }

    void onListBoxSelect(const twins::Widget* pWgt, int16_t selIdx) override
    {
        mWgtProp[pWgt->id].lbx.selIdx = selIdx;
        TWINS_LOG("LISTBOX_SELECT(%u)", selIdx);
    }

    void onListBoxChange(const twins::Widget* pWgt, int16_t newIdx) override
    {
        if (pWgt->id == ID_LISTBOX)
        {
            mWgtProp[pWgt->id].lbx.itemIdx = newIdx;
            mWgtProp[pWgt->id].lbx.selIdx = newIdx;
            TWINS_LOG("LISTBOX_CHANGE(%u)", newIdx);
        }
    }

    void onComboBoxSelect(const twins::Widget* pWgt, int16_t selIdx) override
    {
        mWgtProp[pWgt->id].cbbx.selIdx = selIdx;
        TWINS_LOG("COMBOBOX_SELECT(%u)", selIdx);
    }

    void onComboBoxChange(const twins::Widget* pWgt, int16_t newIdx) override
    {
        mWgtProp[pWgt->id].cbbx.itemIdx = newIdx;
        TWINS_LOG("COMBOBOX_CHANGE(%u)", newIdx);
    }

    void onComboBoxDrop(const twins::Widget* pWgt, bool dropState) override
    {
        mWgtProp[pWgt->id].cbbx.dropDown = dropState;
        if (dropState)
            mWgtProp[pWgt->id].cbbx.selIdx = mWgtProp[pWgt->id].cbbx.itemIdx;

        TWINS_LOG("COMBOBOX_DROP(%u)", dropState);
    }

    void onRadioSelect(const twins::Widget* pWgt) override
    {
        TWINS_LOG("RADIO_SELECT(%d)", pWgt->radio.radioId);
        if (pWgt->radio.groupId == 1)
            mRadioId = pWgt->radio.radioId;
    }

    void onTextBoxScroll(const twins::Widget* pWgt, int16_t topLine) override
    {
        mWgtProp[pWgt->id].txtbx.topLine = topLine;
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
            auto &prp = mWgtProp[pWgt->id];
            prp.enabled = !prp.enabled;
            return prp.enabled;
        }

        if (pWgt->id == ID_CHBX_C)
            return false;

        if (pWgt->id == ID_BTN_SAYYES)
            return mWgtProp[pWgt->id].enabled;

        return true;
    }

    bool isFocused(const twins::Widget* pWgt) override
    {
        return pWgt->id == mFocusedIds[mPgcPage];
    }

    bool isVisible(const twins::Widget* pWgt) override
    {
        //if (pWgt->id == ID_WND)    return bb.wndMngr().topWnd() == pWgt; // always visible
        if (pWgt->id == ID_PAGE_1) return mPgcPage == 0;
        if (pWgt->id == ID_PAGE_2) return mPgcPage == 1;
        if (pWgt->id == ID_PAGE_3) return mPgcPage == 2;
        if (pWgt->id == ID_PAGE_4) return mPgcPage == 3;
        if (pWgt->id == ID_PAGE_5) return mPgcPage == 4;
        if (pWgt->id == ID_PAGE_6) return mPgcPage == 5;

        return true;
    }

    twins::WID& getFocusedID() override
    {
        return mFocusedIds[mPgcPage];
    }

    bool getCheckboxChecked(const twins::Widget* pWgt) override
    {
        return mWgtProp[pWgt->id].chbx.checked;
    }

    void getLabelText(const twins::Widget* pWgt, twins::String &out) override
    {
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

        if (pWgt->id == ID_LBL_WORDWRAP)
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
        case ID_EDT_1: out = mEdt1Text; break;
        case ID_EDT_2: out = mEdt2Text; break;
        default: break;
        }
    }

    bool getLedLit(const twins::Widget* pWgt) override
    {
        auto &prp = mWgtProp[pWgt->id];
        prp.led.lit = !prp.led.lit;
        return prp.led.lit;
    }

    void getProgressBarState(const twins::Widget*, int32_t &pos, int32_t &max) override
    {
        pos = mPgbarPos++;
        max = 20;
        if (mPgbarPos > max) mPgbarPos = 0;
    }

    int getPageCtrlPageIndex(const twins::Widget* pWgt) override
    {
        return mPgcPage;
    }

    void getListBoxState(const twins::Widget* pWgt, int16_t &itemIdx, int16_t &selIdx, int16_t &itemsCount) override
    {
        if (pWgt->id == ID_LISTBOX)
        {
            itemIdx = mWgtProp[pWgt->id].lbx.itemIdx;
            selIdx = mWgtProp[pWgt->id].lbx.selIdx;
            itemsCount = mListBoxItemsCount;
        }
    }

    void getListBoxItem(const twins::Widget* pWgt, int itemIdx, twins::String &out) override
    {
        if (pWgt->id == ID_LISTBOX)
        {
            if (itemIdx == 3)
                out.appendFmt(ESC_BOLD "Item" ESC_NORMAL " 0034567890123456789*");
            else
                out.appendFmt(ESC_FG_BLACK "Item" ESC_FG_BLUE " %03d", itemIdx);
        }
    }

    void getComboBoxState(const twins::Widget* pWgt, int16_t &itemIdx, int16_t &selIdx, int16_t &itemsCount, bool &dropDown) override
    {
        itemIdx = mWgtProp[pWgt->id].cbbx.itemIdx;
        selIdx = mWgtProp[pWgt->id].cbbx.selIdx;
        itemsCount = 6;
        dropDown = mWgtProp[pWgt->id].cbbx.dropDown;
    }

    void getComboBoxItem(const twins::Widget*, int itemIdx, twins::String &out) override
    {
        out.appendFmt("Option %03d", itemIdx);
    }

    int getRadioIndex(const twins::Widget* pWgt) override
    {
        return mRadioId;
    }

    void getTextBoxState(const twins::Widget* pWgt, const twins::Vector<twins::StringRange> **ppLines, int16_t &topLine) override
    {
        if (pWgt->id == ID_TBX_LOREMIPSUM)
        {
            if (mTxtBox1Text.isDirty()) mWgtProp[pWgt->id].txtbx.topLine = 0;
            topLine = mWgtProp[pWgt->id].txtbx.topLine;

            if (ppLines)
            {
                mTxtBox1Text.config(pWgt->size.width-2, " \n");
                *ppLines = &mTxtBox1Text.getLines();
            }
        }
        else
        {
            if (mTxtBox2Text.isDirty()) mWgtProp[pWgt->id].txtbx.topLine = 0;
            topLine = mWgtProp[pWgt->id].txtbx.topLine;

            if (ppLines)
            {
                mTxtBox2Text.config(pWgt->size.width-2, " \n");
                *ppLines = &mTxtBox2Text.getLines();
            }
        }
    }

    // --- requests ---

    void invalidate(twins::WID id, bool instantly = false) override
    {
        if (id == twins::WIDGET_ID_NONE)
        {
            invalidatedWgts.clear();
            return;
        }

        // state or focus changed - widget must be repainted
        if (instantly)
        {
            WindowStateBase::invalidate(id, instantly);
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
    twins::Vector<twins::WID> invalidatedWgts;

private:
    int16_t mPgbarPos = 0;
    int16_t mPgcPage = 0;
    int16_t mRadioId = 0;
    int16_t mListBoxItemsCount = 20;
    twins::String mEdt1Text;
    twins::String mEdt2Text;
    twins::Map<twins::WID, twins::WidgetProp> mWgtProp;
    twins::util::WrappedString mTxtBox1Text;
    twins::util::WrappedString mTxtBox2Text;

    // focused WID separate for each page
    using wids_t = twins::Vector<twins::WID>;
    wids_t mFocusedIds;
};

// state of Popup window
class WndPopupState : public twins::WindowStateBase
{
public:
    ~WndPopupState() { printf("~WndPopupState()\n"); }

    // --- events ---

    void onButtonClick(const twins::Widget* pWgt, const twins::KeyCode &kc) override
    {
        if (onButton)
            onButton(pWgt->id);

        twins::glob::wMngr.popWnd();
    }

    bool onWindowUnhandledInputEvt(const twins::Widget* pWgt, const twins::KeyCode &kc) override
    {
        if (kc.key == twins::Key::Esc)
        {
            twins::glob::wMngr.popWnd();
            return true;
        }
        return false;
    }

    // --- widgets state queries ---

    void getWindowCoord(const twins::Widget* pWgt, twins::Coord &coord) override
    {
        const auto *p_wnd = twins::glob::pMainWindowWgts;
        // calc location on the main window center
        coord.col = (p_wnd->size.width - pWgt->size.width) / 2;
        coord.col += p_wnd->coord.col;
        coord.row = (p_wnd->size.height - pWgt->size.height) / 2;
        coord.row += p_wnd->coord.row;
    }

    void getWindowTitle(const twins::Widget* pWgt, twins::String &title) override
    {
        title = wndTitle;
    }

    bool isFocused(const twins::Widget* pWgt) override
    {
        return pWgt->id == mFocusedId;
    }

    bool isVisible(const twins::Widget* pWgt) override
    {
        switch (pWgt->id)
        {
        case IDPP_WND:          return twins::glob::wMngr.topWndWidgets() == pWgt;
        case IDPP_BTN_YES:      return strstr(buttons.cstr(), "y") != nullptr;
        case IDPP_BTN_NO:       return strstr(buttons.cstr(), "n") != nullptr;
        case IDPP_BTN_CANCEL:   return strstr(buttons.cstr(), "c") != nullptr;
        case IDPP_BTN_OK:       return strstr(buttons.cstr(), "o") != nullptr;
        default:                return true;
        }
        return true;
    }

    void getLabelText(const twins::Widget* pWgt, twins::String &out) override
    {
        if (pWgt->id == IDPP_LBL_MSG)
        {
            out = twins::util::wordWrap(wndMessage.cstr(), pWgt->size.width);
        }
    }

public:
    std::function<void(twins::WID id)> onButton;
    twins::String wndTitle;
    twins::String wndMessage;
    twins::String buttons;
};

// -----------------------------------------------------------------------------

static WndMainState wndMain;
static WndPopupState wndPopup;

twins::IWindowState * getWndMain()
{
    // the only place where pWndMainWidgets is used
    if (!wndMain.getWidgets())
        wndMain.init(pWndMainWidgets);
    return &wndMain;
}

twins::IWindowState * getWndPopup()
{
    // the only place where pWndPopupWidgets is used
    if (!wndPopup.getWidgets())
        wndPopup.init(pWndPopupWidgets);
    return &wndPopup;
}

// -----------------------------------------------------------------------------

void showPopup(twins::String title, twins::String message, std::function<void(twins::WID btnID)> onButton, const char *buttons)
{
    wndPopup.wndTitle = std::move(title);
    wndPopup.wndMessage = std::move(message);
    wndPopup.onButton = onButton;
    wndPopup.buttons = buttons;

    twins::resetInternalState();
    twins::glob::wMngr.pushWnd(getWndPopup());
}

// -----------------------------------------------------------------------------

int main()
{
    // printf("Win1 controls: %u" "\n", wndMain.topWnd.childCount);
    // printf("sizeof Widget: %zu" "\n", sizeof(twins::Widget));
    twins::screenClrAll();
    twins::glob::wMngr.pushWnd(getWndMain());
    twins::inputPosixInit(100);
    twins::mouseMode(twins::MouseMode::M2);
    twins::flushBuffer();

    // after twins::init():
    twins::RingBuff<char> rbKeybInput;
    rbKeybInput.init(20);

    // assert(!"just-a-test");

    for (;;)
    {
        bool quit_req = false;
        const char *posix_inp = twins::inputPosixRead(quit_req);
        if (quit_req) break;
        rbKeybInput.write(posix_inp);

        if (rbKeybInput.size() && twins::glob::wMngr.size())
        {
            twins::Locker lck;
            twins::KeyCode kc = {};

            // display input buffer
            {
                char seq[10];
                auto n = rbKeybInput.copy(seq, sizeof(seq)-1);
                seq[n] = '\0';
                wndMain.lblKeycodeSeq = seq;
            }

            twins::decodeInputSeq(rbKeybInput, kc);
            // pass key to top-window
            bool key_handled = twins::processKey(twins::glob::wMngr.topWnd()->getWidgets(), kc);
            wndMain.lblKeyName = kc.name;

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
                twins::glob::wMngr.redrawAll();
            }
            else if (kc.m_spec && kc.key == twins::Key::F6)
            {
                twins::cursorSavePos();
                twins::moveTo(0, twins::glob::pal.getLogsRow());
                twins::screenClrBelow();
                twins::cursorRestorePos();
            }
            else if (kc.m_spec && kc.m_ctrl && (kc.key == twins::Key::PgUp || kc.key == twins::Key::PgDown))
            {
                if (twins::glob::wMngr.topWnd() == &wndMain)
                    twins::mainPgControlChangePage(wndMain.getWidgets(), kc.key == twins::Key::PgDown);
            }
            else if (kc.m_spec && (kc.key == twins::Key::F9 || kc.key == twins::Key::F10))
            {
                if (twins::glob::wMngr.topWnd() == &wndMain)
                    twins::mainPgControlChangePage(wndMain.getWidgets(), kc.key == twins::Key::F10);
            }


            if (twins::glob::wMngr.topWnd() == &wndMain)
            {
                // keyboard code
                twins::cursorSavePos();
                twins::drawWidgets(wndMain.getWidgets(), {ID_LABEL_KEYSEQ, ID_LABEL_KEYNAME});

                if (kc.mod_all != KEY_MOD_SPECIAL)
                {
                    twins::drawWidgets(wndMain.getWidgets(),
                    {
                        ID_LED_LOCK, ID_LED_BATTERY, ID_LED_PUMP,
                        ID_PRGBAR_1, ID_PRGBAR_2, ID_PRGBAR_3,
                        ID_PANEL_VERSIONS,
                    });
                }
                twins::cursorRestorePos();
            }


            if (twins::glob::wMngr.topWnd() == &wndMain)
            {
                twins::drawWidgets(wndMain.getWidgets(), wndMain.invalidatedWgts.data(), wndMain.invalidatedWgts.size());
                wndMain.invalidatedWgts.clear();
            }
        }

        twins::flushBuffer();
    }

    twins::moveTo(0, twins::glob::pal.getLogsRow());
    twins::screenClrBelow();
    twins::mouseMode(twins::MouseMode::Off);
    twins::flushBuffer();
    twins::inputPosixFree();

    printf(ESC_BOLD "Memory stats: max chunks: %d, max allocated: %d B\n" ESC_NORMAL,
        ((DemoPAL&)twins::glob::pal).stats.memChunksMax,
        ((DemoPAL&)twins::glob::pal).stats.memAllocatedMax
    );
}
