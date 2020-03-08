/******************************************************************************
 * @brief   TWins - demo application
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#include "twins.hpp"
#include "twins_string_buff.hpp"

#include <stdio.h>
#include <stdlib.h>

// -----------------------------------------------------------------------------

enum WndMainIDs
{
    ID_INVALID,
    ID_WND,
        ID_PANEL_VERSIONS,
            ID_LABEL_FW_VERSION,
            ID_LABEL_DATE,
            ID_LABEL_TIME,
        ID_PANEL_CONFIG,
        ID_PANEL_KEYCODE,
            ID_LABEL_KEYCODE
};

static char lbl_keycode_txt[8];
static twins::StringBuff sbuff;

// -----------------------------------------------------------------------------

const twins::WindowCallbacks wndMainClbcks =
{
    .onDraw = [](const twins::Widget* pWgt)
    {
        // render default
        return false;
    },
    .isEnabled = [](const twins::Widget* pWgt)
    {
        static bool pnlEnabled = false;

        if (pWgt->id == ID_PANEL_CONFIG)
        {
            pnlEnabled = !pnlEnabled;
            return pnlEnabled;
        }

        return true;
    },
    .isFocused = [](const twins::Widget* pWgt)
    {
        return false;
    },
    .isVisible = [](const twins::Widget* pWgt)
    {
        return true;
    },
    .getCheckboxChecked = [](const twins::Widget* pWgt)
    {
        return false;
    },
    .getLabelText = [](const twins::Widget* pWgt) -> const char*
    {
        if (pWgt->id == ID_LABEL_KEYCODE)
        {
            sbuff.clear();
            sbuff.cat("KEY: ");

            for (unsigned i = 0; i < sizeof(lbl_keycode_txt); i++)
            {
                char c = lbl_keycode_txt[i];
                if (!c) break;

                if (c < ' ')
                {
                    char b[5];
                    snprintf(b, sizeof(b), "\\x%02x", c);
                    sbuff.cat(b);
                }
                else
                {
                    sbuff.cat(c);
                }
            }

            return sbuff.buff;
        }
        return nullptr;
    }
};

const twins::Widget wndMain =
{
    .type = twins::Widget::Window,
    .id = ID_WND,
    .coord = { 10, 4 },
    .size = { 60, 12 },
    .window =
    {
        .frameStyle = twins::FrameStyle::Double,
        .bgColor = twins::ColorBG::BLUE,
        .fgColor = twins::ColorFG::WHITE,
        .caption = "Service Menu (Ctrl+D quit)",
        .pCallbacks = &wndMainClbcks,
        .pChildrens = (twins::Widget[])
        {
            {
                .type = twins::Widget::Panel,
                .id = ID_PANEL_VERSIONS,
                .coord = { 2, 2 },
                .size = { 21, 5 },
                .panel =
                {
                    .frameStyle = twins::FrameStyle::Single,
                    .bgColor = twins::ColorBG::GREEN,
                    .fgColor = twins::ColorFG::WHITE,
                    .caption = "VER",
                    .pChildrens = (twins::Widget[])
                    {
                        {
                            .type = twins::Widget::Label,
                            .id = ID_LABEL_FW_VERSION,
                            .coord = { 2, 1 },
                            .size = { 14, 1 },
                            .label =
                            {
                                .bgColor = twins::ColorBG::BLACK_INTENSE,
                                .fgColor = twins::ColorFG::WHITE,
                                .text = "FwVer: 1"
                            }
                        },
                        {
                            .type = twins::Widget::Label,
                            .id = ID_LABEL_DATE,
                            .coord = { 2, 2 },
                            .size = { 16, 1 },
                            .label =
                            {
                                .bgColor = twins::ColorBG::WHITE,
                                .fgColor = twins::ColorFG::BLACK,
                                .text = "Date: " __DATE__
                            }
                        },
                        {
                            .type = twins::Widget::Label,
                            .id = ID_LABEL_TIME,
                            .coord = { 2, 3 },
                            .size = { 16, 1 },
                            .label =
                            {
                                .bgColor = twins::ColorBG::WHITE,
                                .fgColor = twins::ColorFG::BLACK,
                                .text = "Time: " __TIME__
                            }
                        },
                    },
                    .childrensCount = 3
                }
            },
            {
                .type = twins::Widget::Panel,
                .id = ID_PANEL_CONFIG,
                .coord = { 30, 2 },
                .size = { 25, 8 },
                .panel =
                {
                    .frameStyle = twins::FrameStyle::Single,
                    .bgColor = twins::ColorBG::GREEN,
                    .fgColor = twins::ColorFG::WHITE,
                    .caption = "CONFIG",
                    .childrensCount = 0
                }
            },
            {
                .type = twins::Widget::Panel,
                .id = ID_PANEL_KEYCODE,
                .coord = { 2, 8 },
                .size = { 21, 3 },
                .panel =
                {
                    .frameStyle = twins::FrameStyle::Single,
                    .bgColor = twins::ColorBG::CYAN,
                    .fgColor = twins::ColorFG::WHITE,
                    .caption = "KEY-CODES",
                    .pChildrens = (twins::Widget[])
                    {
                        {
                            .type = twins::Widget::Label,
                            .id = ID_LABEL_KEYCODE,
                            .coord = { 2, 1 },
                            .size = { 17, 1 },
                            .label =
                            {
                                .bgColor = twins::ColorBG::WHITE,
                                .fgColor = twins::ColorFG::RED,
                                .text = nullptr // use callback to get text
                            }
                        },
                    },
                    .childrensCount = 1
                }
            }
        },
        .childrensCount = 3,
    }
};


static twins::IOs tios =
{
    .writeStr = [](const char *s)
    {
        return printf(s);
    },
    .writeStrFmt = [](const char *fmt, va_list ap)
    {
        return vprintf(fmt, ap);
    },
    .malloc = [](uint32_t sz)
    {
        return malloc(sz);
    },
    .mfree = [](void *ptr)
    {
        free(ptr);
    }
};

// -----------------------------------------------------------------------------

#include "twins_input_posix.hpp"

int main()
{
    // printf("Win1 controls: %u" "\n", wndMain.window.childrensCount);
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
        const char *keyseq = twins::inputPosixCheckInput(quit_req);
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
