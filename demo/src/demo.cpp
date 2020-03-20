/******************************************************************************
 * @brief   TWins - demo application
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#include "demo_wnd.hpp"
#include "twins.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// -----------------------------------------------------------------------------

class WndMainState : public twins::IWindowState
{
public:
    // --- events ---

    bool onDraw(const twins::Widget* pWgt) override
    {
        // render default
        return false;
    }

    void onButtonClick(const twins::Widget* pWgt) override
    {
    }

    void onEditChange(const twins::Widget* pWgt, twins::String &str) override
    {
    }

    void onCheckboxToggle(const twins::Widget* pWgt) override
    {
    }

    void onPageControlPageChange(const twins::Widget* pWgt, uint8_t newPageIdx) override
    {
        // change page control page id
        if (pWgt->id == ID_PGCONTROL)
        {
            pgcPage = newPageIdx;

            //onInvalidated() is the next call
            //twins::drawWidget(&wndMain, ID_PGCONTROL);
        }
    }

    void onInvalidate(twins::WID id) override
    {
        // state or focus changed - widget must be repainted
        twins::drawWidget(&wndMain, id);
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
        return ledBatt;
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

public:
    char lblKeycodeSeq[8];
    const char *lblKeyName = "";

private:
    bool pnlVerEnabled = false;
    bool ledLock = false;
    bool ledBatt = false;
    int  pgbarPos = 0;
    int  pgcPage = 0;
    // focused WID separate for each page
    twins::WID focusedId[3] {};
};

// -----------------------------------------------------------------------------

static WndMainState wndMainState;
twins::IWindowState * getWindMainState() { return &wndMainState; }

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
    malloc : [](uint32_t sz)
    {
        return malloc(sz);
    },
    mfree : [](void *ptr)
    {
        free(ptr);
    }
};

// -----------------------------------------------------------------------------

#include "twins_input_posix.hpp"

int main()
{
    // printf("Win1 controls: %u" "\n", wndMain.window.childCount);
    // printf("sizeof Widget: %zu" "\n", sizeof(twins::Widget));

    puts(ESC_CURSOR_HOME);
    puts(ESC_SCREEN_ERASE_ALL);

    twins::init(&tios);
    twins::drawWidget(&wndMain);
    twins::inputPosixInit(500);
    fflush(stdout);

    for (;;)
    {
        bool quit_req = false;
        twins::AnsiSequence ansi_seq;
        twins::inputPosixRead(ansi_seq, quit_req);
        if (quit_req) break;

        if (ansi_seq.len)
        {
            twins::KeyCode key_decoded;
            twins::decodeInputSeq(ansi_seq, key_decoded);
            twins::processKey(&wndMain, key_decoded);

            // display key code
            strncpy(wndMainState.lblKeycodeSeq, ansi_seq.data, sizeof(wndMainState.lblKeycodeSeq));
            wndMainState.lblKeyName = key_decoded.name;
            twins::drawWidgets(&wndMain, {ID_LABEL_KEYSEQ, ID_LABEL_KEYNAME});

            if (key_decoded.mod_all != KEY_MOD_SPECIAL)
            {
                twins::drawWidgets(&wndMain,
                {
                    ID_LED_LOCK, ID_LED_BATTERY, ID_LED_PUMP, ID_CHBX_ENBL, ID_PRGBAR_1,
                    ID_PANEL_VERSIONS, ID_BTN_YES
                });
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
