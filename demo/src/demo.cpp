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
            // wid = twins::WIDGET_ID_NONE;
            wid = ID_WND;

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
        if (pWgt->id == ID_PAGE_1) return pgcPage == 0;
        if (pWgt->id == ID_PAGE_2) return pgcPage == 1;
        if (pWgt->id == ID_PAGE_3) return pgcPage == 2;
        if (pWgt->id == ID_PAGE_4) return pgcPage == 3;

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
            out.appendFmt(ESC_FG_BLACK "Item" ESC_FG_BLUE " %03d", itemIdx);
    }

    int getRadioIndex(const twins::Widget* pWgt) override
    {
        return radioId;
    }

    // --- requests ---

    void invalidate(twins::WID id, bool instantly) override
    {
        // state or focus changed - widget must be repainted

        if (instantly)
        {
            twins::drawWidget(pWndMainArray, id);
            twins::flushBuffer();
        }
        else
        {
            if (!invalidatedWgts.contains(id))
                invalidatedWgts.append(id);
        }
    }

public:
    char lblKeycodeSeq[10];
    const char *lblKeyName = "";
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
    };

    int  pgbarPos = 0;
    int  pgcPage = 0;
    int  listBoxItemIdx = 0;
    int  listBoxItemsCount = 20;
    int  radioId = 0;
    twins::String edt1Text;
    twins::String edt2Text;
    twins::Map<twins::WID, WgtProp> wgtProp;

    // focused WID separate for each page
    using wids_t = twins::Vector<twins::WID>;
    wids_t focusedId;
};

// -----------------------------------------------------------------------------

static WndMainState wndMainState;

twins::IWindowState * getWindMainState()
{
    if (!wndMainState.initialized)
        wndMainState.init();
    return &wndMainState;
}

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
        return pWndMainArray[0].coord.row + pWndMainArray[0].size.height + 1;
    }
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
    twins::drawWidget(pWndMainArray);
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
                twins::drawWidget(pWndMainArray);
            }
            else if (kc.m_spec && kc.key == twins::Key::F6)
            {
                twins::cursorSavePos();
                twins::moveTo(0, pWndMainArray[0].coord.row + pWndMainArray[0].size.height + 1);
                twins::screenClrBelow();
                twins::cursorRestorePos();
            }
            else if (kc.m_spec && kc.m_ctrl && (kc.key == twins::Key::PgUp || kc.key == twins::Key::PgDown))
            {
                twins::mainPgControlChangePage(pWndMainArray, kc.key == twins::Key::PgDown);
            }
            else if (kc.m_spec && (kc.key == twins::Key::F9 || kc.key == twins::Key::F10))
            {
                twins::mainPgControlChangePage(pWndMainArray, kc.key == twins::Key::F10);
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

            twins::drawWidgets(pWndMainArray, wndMainState.invalidatedWgts.data(), wndMainState.invalidatedWgts.size());
            wndMainState.invalidatedWgts.clear();
        }

        twins::flushBuffer();
    }

    twins::moveTo(0, demo_pal.getLogsRow());
    twins::screenClrBelow();
    twins::mouseMode(twins::MouseMode::Off);
    twins::flushBuffer();
    twins::inputPosixFree();

    printf(ESC_BOLD "Memory stats: max chunks: %d, max memory: %d B\n" ESC_NORMAL,
        demo_pal.stats.memChunksMax, demo_pal.stats.memAllocatedMax
    );
}
