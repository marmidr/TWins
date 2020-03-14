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

class Wnd1State : public twins::IWindowState
{
public:
    bool onDraw(const twins::Widget* pWgt) override
    {
        // render default
        return false;
    }
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
        if (pWgt->id == ID_BTN_YES) return true;
        return false;
    }
    bool isVisible(const twins::Widget* pWgt) override
    {
        return true;
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
            // out.appendFmt("SEQ[%zu]:", strlen(lblKeycodeSeq));
            out.append("SEQ:");

            for (unsigned i = 0; i < sizeof(lblKeycodeSeq); i++)
            {
                uint8_t c = lblKeycodeSeq[i];
                if (!c) break;

                if (c < ' ')
                    out.appendFmt("\\x%02x", c);
                else
                    out.append(c);
            }
        }

        if (pWgt->id == ID_LABEL_KEYNAME)
        {
            // out.appendFmt("KEY[%zu]:", strlen(lblKeyName));
            out.append("KEY:");
            out.append(lblKeyName);
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

public:
    char lblKeycodeSeq[8];
    const char *lblKeyName = "";
private:
    bool pnlVerEnabled = false;
    bool ledLock = false;
    bool ledBatt = false;
    int  pgbarPos = 0;
};

// -----------------------------------------------------------------------------

static Wnd1State wnd1State;
twins::IWindowState * getWind1State() { return &wnd1State; }

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

            strncpy(wnd1State.lblKeycodeSeq, ansi_seq.data, sizeof(wnd1State.lblKeycodeSeq));
            wnd1State.lblKeyName = key_decoded.name;

            twins::drawWidgets(&wndMain, {ID_LABEL_KEYSEQ, ID_LABEL_KEYNAME});
        }

        twins::drawWidgets(&wndMain,
        {
            ID_LED_LOCK, ID_LED_BATTERY, ID_LED_PUMP, ID_CHBX_ENBL, ID_PRGBAR_1, ID_PANEL_VERSIONS
        });

        twins::moveToHome();
        // printf("Key: %s\n", keyseq);
        fflush(stdout);
    }

    twins::moveTo(0, wndMain.coord.row + wndMain.size.height + 1);
    // twins::writeStr();
    twins::inputPosixFree();
}
