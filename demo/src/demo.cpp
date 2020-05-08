/******************************************************************************
 * @brief   TWins - demo application
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#include "demo_wnd.hpp"
#include "twins.hpp"
#include "twins_ringbuffer.hpp"
#include "twins_ios_defimpl.hpp"
#include "twins_vector.hpp"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

// -----------------------------------------------------------------------------

class WndMainState : public twins::IWindowState
{
public:
    void init()
    {
        focusedId.resize(wndMainNumPages);

        for (auto &wid : focusedId)
            wid = twins::WIDGET_ID_NONE;

        initialized = true;
    }

    // --- events ---

    void onButtonDown(const twins::Widget* pWgt) override
    {
        if (pWgt->id == ID_BTN_YES)     TWINS_LOG("BTN_YES");
        if (pWgt->id == ID_BTN_NO)      TWINS_LOG("BTN_NO");
        if (pWgt->id == ID_BTN_CANCEL)  TWINS_LOG("BTN_CANCEL");
    }

    void onButtonUp(const twins::Widget* pWgt) override
    {
        if (pWgt->id == ID_BTN_YES)     TWINS_LOG("BTN_YES");
        if (pWgt->id == ID_BTN_NO)      TWINS_LOG("BTN_NO");
        if (pWgt->id == ID_BTN_CANCEL)  TWINS_LOG("BTN_CANCEL");
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

    void onCheckboxToggle(const twins::Widget* pWgt) override
    {
        if (pWgt->id == ID_CHBX_ENBL) TWINS_LOG("CHBX_ENBL"), chbxEnabled = !chbxEnabled;
        if (pWgt->id == ID_CHBX_LOCK) TWINS_LOG("CHBX_LOCK"), chbxLocked = !chbxLocked;
    }

    void onPageControlPageChange(const twins::Widget* pWgt, uint8_t newPageIdx) override
    {
        if (pWgt->id == ID_PGCONTROL) pgcPage = newPageIdx;
    }

    void onListBoxSelect(const twins::Widget* pWgt, uint16_t highlightIdx) override
    {
        TWINS_LOG("LISTBOX_SELECT(%u)", highlightIdx);
    }

    void onListBoxChange(const twins::Widget* pWgt, uint16_t newIdx) override
    {
        if (pWgt->id == ID_LISTBOX)
        {
            listBoxItemIdx = newIdx;
            TWINS_LOG("LISTBOX_CHANGE(%u)", newIdx);
        }
    }

    void onRadioSelect(const twins::Widget* pWgt) override
    {
        TWINS_LOG("RADIO_SELECT(%d)", pWgt->radio.radioId);
        if (pWgt->radio.groupId == 1)
            radioId = pWgt->radio.radioId;
    }

    void onCanvasDraw(const twins::Widget* pWgt) override
    {
        auto coord = twins::getScreenCoord(pWgt);
        auto sz = pWgt->size;

        twins::moveTo(coord.col, coord.row);
        twins::writeChar('-', sz.width);
        twins::moveTo(coord.col, coord.row + sz.height);
        twins::writeChar('-', sz.width);
    }

    void onCanvasMouseEvt(const twins::Widget* pWgt, const twins::KeyCode &kc) override
    {
        twins::moveTo(kc.mouse.col, kc.mouse.row);
        twins::writeChar('0' + (int)kc.mouse.btn);
    }

    // --- widgets state queries ---

    bool isEnabled(const twins::Widget* pWgt) override
    {
        if (pWgt->id == ID_PANEL_VERSIONS)
        {
            pnlVerEnabled = !pnlVerEnabled;
            return pnlVerEnabled;
        }

        return true;
    }

    bool isFocused(const twins::Widget* pWgt) override
    {
        return pWgt->id == focusedId[pgcPage];
    }

    bool isVisible(const twins::Widget* pWgt) override
    {
        if (pWgt->id == ID_PAGE_1) return pgcPage == 0;
        if (pWgt->id == ID_PAGE_2) return pgcPage == 1;
        if (pWgt->id == ID_PAGE_3) return pgcPage == 2;

        return true;
    }

    twins::WID& getFocusedID() override
    {
        return focusedId[pgcPage];
    }

    bool getCheckboxChecked(const twins::Widget* pWgt) override
    {
        if (pWgt->id == ID_CHBX_ENBL) return chbxEnabled;
        if (pWgt->id == ID_CHBX_LOCK) return chbxLocked;
        return false;
    }

    void getLabelText(const twins::Widget* pWgt, twins::String &out) override
    {
        out.clear();

        if (pWgt->id == ID_LABEL_KEYSEQ)
        {
            out.appendFmt("SEQ[%zu]:", strlen(lblKeycodeSeq));

            for (unsigned i = 0; i < sizeof(lblKeycodeSeq); i++)
            {
                uint8_t c = lblKeycodeSeq[i];
                if (!c) break;

                if (c < ' ')
                    out.appendFmt("\\x%02x", c);
                else if (c != (char)twins::Ansi::DEL)
                    out.append(c);
            }
        }

        if (pWgt->id == ID_LABEL_KEYNAME)
        {
            out.appendFmt("KEY[%zu]:%s", strlen(lblKeyName), lblKeyName);
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
        if (pWgt->id == ID_LED_LOCK)
        {
            ledLock = !ledLock;
            return ledLock;
        }
        if (pWgt->id == ID_LED_PUMP)
        {
            ledBatt = !ledBatt;
            return lblKeycodeSeq[0] == ' ';
        }
        return ledBatt;
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

    void getListBoxState(const twins::Widget*, int &itemIdx, int &itemsCount) override
    {
        itemIdx = listBoxItemIdx;
        itemsCount = listBoxItemsCount;
    }

    void getListBoxItem(const twins::Widget*, int itemIdx, twins::String &out) override
    {
        if (itemIdx == 3)
            out.appendFmt(ESC_BOLD "Item" ESC_NORMAL " 0034567890123456789*");
        else
            out.appendFmt(ESC_FG_GREEN_INTENSE "Item" ESC_FG_WHITE " %03d", itemIdx);
    }

    int getRadioIndex(const twins::Widget* pWgt) override
    {
        return radioId;
    }

    // --- requests ---

    void invalidate(twins::WID id) override
    {
        // state or focus changed - widget must be repainted
        // note: drawing here is lazy solution
        twins::drawWidget(pWndMainArray, id);
    }


public:
    char lblKeycodeSeq[10];
    const char *lblKeyName = "";
    bool initialized = false;

private:
    bool pnlVerEnabled = false;
    bool ledLock = false;
    bool ledBatt = false;
    bool chbxEnabled = false;
    bool chbxLocked = true;
    int  pgbarPos = 0;
    int  pgcPage = 0;
    int  listBoxItemIdx = 0;
    int  listBoxItemsCount = 20;
    int  radioId = 0;

    // focused WID separate for each page
    using wids_t = twins::Vector<twins::WID>;
    wids_t focusedId;
    twins::String edt1Text;
    twins::String edt2Text;
};

// -----------------------------------------------------------------------------

static WndMainState wndMainState;

twins::IWindowState * getWindMainState()
{
    if (!wndMainState.initialized) wndMainState.init();
    return &wndMainState;
}

twins::RingBuff<char> rbKeybInput;

struct DemoIOs : twins::DefaultIOs
{
    DemoIOs()
    {
    }

    ~DemoIOs()
    {
    }

    uint16_t getLogsRow() override
    {
        return pWndMainArray[0].coord.row + pWndMainArray[0].size.height + 1;
    }

    void sleep(uint16_t ms) override
    {
        usleep(ms * 1000);
    }
};

static DemoIOs demo_ios;

// -----------------------------------------------------------------------------

#include "twins_input_posix.hpp"

int main()
{
    // printf("Win1 controls: %u" "\n", wndMain.window.childCount);
    // printf("sizeof Widget: %zu" "\n", sizeof(twins::Widget));

    twins::init(&demo_ios);
    twins::screenClrAll();
    twins::drawWidget(pWndMainArray);
    twins::inputPosixInit(100);
    twins::writeStr(ESC_MOUSE_REPORTING_M2_ON);
    rbKeybInput.init(20);
    fflush(stdout);

    for (;;)
    {
        bool quit_req = false;
        twins::KeyCode kc = {};
        const char *posix_inp = twins::inputPosixRead(quit_req);
        if (quit_req) break;
        rbKeybInput.write(posix_inp);

        if (rbKeybInput.size())
        {
            // display input buffer
            memset(wndMainState.lblKeycodeSeq, 0, sizeof(wndMainState.lblKeycodeSeq));
            rbKeybInput.copy(wndMainState.lblKeycodeSeq, sizeof(wndMainState.lblKeycodeSeq)-1);
            wndMainState.lblKeycodeSeq[sizeof(wndMainState.lblKeycodeSeq)-1] = '\0';

            twins::decodeInputSeq(rbKeybInput, kc);
            bool key_handled = twins::processKey(pWndMainArray, kc);
            wndMainState.lblKeyName = kc.name;

            // display decoded key
            if (kc.key == twins::Key::MouseEvent)
            {
                // TWINS_LOG("B%c %c%c%c %03d:%03d",
                //     '0' + (char)kc.mouse.btn,
                //     kc.m_ctrl ? 'C' : ' ',
                //     kc.m_alt ? 'A' : ' ',
                //     kc.m_shift ? 'S' : ' ',
                //     kc.mouse.col, kc.mouse.row);
            }
            else if (kc.key != twins::Key::None)
            {
                TWINS_LOG("Key: '%s' %s", kc.name, key_handled ? "(handled)" : "");
            }

            if (kc.m_spec && kc.key == twins::Key::F5)
            {
                twins::screenClrAll();
                twins::drawWidget(pWndMainArray);
            }
            else
            {
                twins::cursorSavePos();
                twins::drawWidgets(pWndMainArray, {ID_LABEL_KEYSEQ, ID_LABEL_KEYNAME});

                if (kc.mod_all != KEY_MOD_SPECIAL)
                {
                    twins::drawWidgets(pWndMainArray,
                    {
                        ID_LED_LOCK, ID_LED_BATTERY, ID_LED_PUMP,
                        ID_PRGBAR_1, ID_PRGBAR_2, ID_PRGBAR_3,
                        ID_PANEL_VERSIONS,
                    });
                }
                twins::cursorRestorePos();
            }
        }

        fflush(stdout);
    }

    // Window is always at [0]
    twins::moveTo(0, pWndMainArray[0].coord.row + pWndMainArray[0].size.height + 1);
    twins::screenClrBelow();
    twins::writeStr(ESC_MOUSE_REPORTING_M2_OFF);
    twins::inputPosixFree();

    printf("Memory stats: max chunks: %d, max memory: %d B \n",
        demo_ios.stats.memChunksMax, demo_ios.stats.memAllocatedMax
    );
}
