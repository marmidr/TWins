/******************************************************************************
 * @brief   TWins - demo application
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#include "demo_wnd.hpp"
#include "twins.hpp"
#include "twins_ringbuffer.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// -----------------------------------------------------------------------------

class WndMainState : public twins::IWindowState
{
public:
    // --- events ---

    void onButtonClick(const twins::Widget* pWgt) override
    {
        if (pWgt->id == ID_BTN_YES)     TWINS_LOG("BTN_YES");
        if (pWgt->id == ID_BTN_NO)      TWINS_LOG("BTN_NO");
        if (pWgt->id == ID_BTN_CANCEL)  TWINS_LOG("BTN_CANCEL");
    }

    void onEditChange(const twins::Widget* pWgt, twins::String &str) override
    {
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

    void getEditText(const twins::Widget*, twins::String &out) override
    {
        out = "Name:";
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

    void getProgressBarNfo(const twins::Widget*, int &pos, int &max) override
    {
        pos = pgbarPos++;
        max = 20;
        if (pgbarPos >= max) pgbarPos = 0;
    }

    int getPageCtrlPageIndex(const twins::Widget* pWgt) override
    {
        return pgcPage;
    }

    // --- requests ---

    void invalidate(twins::WID id) override
    {
        // state or focus changed - widget must be repainted
        // note: drawing here is lazy solution
        twins::drawWidget(&wndMain, id);
    }

public:
    char lblKeycodeSeq[10];
    const char *lblKeyName = "";

private:
    bool pnlVerEnabled = false;
    bool ledLock = false;
    bool ledBatt = false;
    bool chbxEnabled = false;
    bool chbxLocked = true;
    int  pgbarPos = 0;
    int  pgcPage = 0;
    // focused WID separate for each page
    twins::WID focusedId[3] {};
};

// -----------------------------------------------------------------------------

static WndMainState wndMainState;
twins::IWindowState * getWindMainState() { return &wndMainState; }
twins::RingBuff<char> rbKeybInput;

static const twins::IOs tios =
{
    writeStr : [](const char *s)
    {
        return printf("%s", s);
    },
    writeStrFmt : [](const char *fmt, va_list ap)
    {
        return vprintf(fmt, ap);
    },
    flush : []()
    {
        fflush(stdout);
    },
    malloc : [](uint32_t sz)
    {
        return malloc(sz);
    },
    mfree : [](void *ptr)
    {
        free(ptr);
    },
    getLogsRow : []() -> uint16_t
    {
        return wndMain.coord.row + wndMain.size.height + 1;
    }
};

// -----------------------------------------------------------------------------

#include "twins_input_posix.hpp"

int main()
{
    // printf("Win1 controls: %u" "\n", wndMain.window.childCount);
    // printf("sizeof Widget: %zu" "\n", sizeof(twins::Widget));

    twins::init(&tios);
    twins::clrScreenAll();
    twins::drawWidget(&wndMain);
    twins::inputPosixInit(100);
    rbKeybInput.init(20);
    fflush(stdout);

    for (;;)
    {
        bool quit_req = false;
        const char *posix_inp = twins::inputPosixRead(quit_req);
        if (quit_req) break;
        rbKeybInput.write(posix_inp);

        if (rbKeybInput.size())
        {
            TWINS_LOG("decode");
            // display input buffer
            memset(wndMainState.lblKeycodeSeq, 0, sizeof(wndMainState.lblKeycodeSeq));
            rbKeybInput.copy(wndMainState.lblKeycodeSeq, sizeof(wndMainState.lblKeycodeSeq)-1);
            wndMainState.lblKeycodeSeq[sizeof(wndMainState.lblKeycodeSeq)-1] = '\0';

            twins::KeyCode kc;
            twins::decodeInputSeq(rbKeybInput, kc);
            twins::processKey(&wndMain, kc);

            // display decoded key
            wndMainState.lblKeyName = kc.name;

            if (kc.m_spec && kc.key == twins::Key::F5)
            {
                twins::clrScreenAll();
                twins::drawWidget(&wndMain);
            }
            else
            {
                twins::drawWidgets(&wndMain, {ID_LABEL_KEYSEQ, ID_LABEL_KEYNAME});

                if (kc.mod_all != KEY_MOD_SPECIAL)
                {
                    twins::drawWidgets(&wndMain,
                    {
                        ID_LED_LOCK, ID_LED_BATTERY, ID_LED_PUMP, ID_PRGBAR_1, ID_PANEL_VERSIONS,
                    });
                }
            }
        }

        twins::moveToHome();
        // printf("Key: %s\n", keyseq);
        fflush(stdout);
    }

    twins::moveTo(0, wndMain.coord.row + wndMain.size.height + 1);
    // twins::writeStr();
    twins::inputPosixFree();
}
