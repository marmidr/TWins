/******************************************************************************
 * @brief   TWins - demo application
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#include "demo_wnd.hpp"

#include "twins.hpp"
#include "twins_string.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// -----------------------------------------------------------------------------

static char lbl_keycode_txt[8];

const twins::WindowCallbacks wndMainClbcks =
{
    // C-style:     .onDraw = []
    // C++ style:   onDraw : []

    onDraw : [](const twins::Widget* pWgt)
    {
        // render default
        return false;
    },
    isEnabled : [](const twins::Widget* pWgt)
    {
        static bool pnlEnabled = false;

        if (pWgt->id == ID_PANEL_CONFIG)
        {
            pnlEnabled = !pnlEnabled;
            return pnlEnabled;
        }

        return true;
    },
    isFocused : [](const twins::Widget* pWgt)
    {
        return false;
    },
    isVisible : [](const twins::Widget* pWgt)
    {
        return true;
    },
    getCheckboxChecked : [](const twins::Widget* pWgt)
    {
        return false;
    },
    getLabelText : [](const twins::Widget* pWgt) -> const char*
    {
        static twins::String str;

        if (pWgt->id == ID_LABEL_KEYCODE)
        {
            str.clear();
            str.append("KEY: ");

            for (unsigned i = 0; i < sizeof(lbl_keycode_txt); i++)
            {
                char c = lbl_keycode_txt[i];
                if (!c) break;

                if (c < ' ')
                    str.appendFmt("\\x%02x", c);
                else
                    str.append(c);
            }

            return str.cstr();
        }
        return nullptr;
    }
};

// -----------------------------------------------------------------------------

static twins::IOs tios =
{
    writeStr : [](const char *s)
    {
        return printf(s);
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
    twins::inputPosixInit(100);
    fflush(stdout);

    for (;;)
    {
        bool quit_req = false;
        const char *keyseq = twins::inputPosixCheckKeys(quit_req);
        if (quit_req) break;

        if (keyseq)
        {
            strncpy(lbl_keycode_txt, keyseq, sizeof(lbl_keycode_txt));
            twins::drawWidget(&wndMain, ID_LABEL_KEYCODE);
            // printf("Key: %s\n", keyseq);
            fflush(stdout);
        }
    }

    twins::moveTo(0, wndMain.coord.row + wndMain.size.height + 1);
    twins::writeStr(ESC_RESET);
    twins::inputPosixFree();
}
