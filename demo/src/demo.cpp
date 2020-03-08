/******************************************************************************
 * @brief   TWins - demo application
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#include "twins.hpp"
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
        ID_PANEL_CONFIG
};

const twins::WindowCallbacks wndMainClbcks =
{
    .onDraw =    [](const twins::Widget* pWdgt) { return false; },
    .isEnabled = [](const twins::Widget* pWdgt)
        {
            static bool pnlEnabled = false;

            if (pWdgt->id == ID_PANEL_CONFIG)
                return pnlEnabled;

            return true;
        },
    .isFocused = [](const twins::Widget* pWdgt) { return false; },
    .isVisible = [](const twins::Widget* pWdgt) { return true; },
    .getCheckboxChecked = [](const twins::Widget* pWdgt) { return false; },
    .getLabelText = [](const twins::Widget* pWdgt) { return ""; }
};

const twins::Widget wndMain =
{
    .type = twins::Widget::Window,
    .id = ID_WND,
    .coord = { 10, 2 },
    .size = { 60, 12 },
    .window =
    {
        .frameStyle = twins::FrameStyle::Double,
        .bgColor = twins::ColorBG::BLUE,
        .fgColor = twins::ColorFG::WHITE,
        .caption = "Service Menu",
        .pCallbacks = &wndMainClbcks,
        .pChildrens = (twins::Widget[])
        {
            {
                .type = twins::Widget::Panel,
                .id = ID_PANEL_VERSIONS,
                .coord = { 4, 2 },
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
            }
        },
        .childrensCount = 2
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
    .onKey = [](twins::Widget *pActWidget, twins::Key k, twins::Mod m)
    {

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

int main()
{
    printf("Win1 controls: %u" "\n", wndMain.window.childrensCount);
    printf("sizeof Widget: %zu" "\n", sizeof(twins::Widget));

    puts(ESC_CURSOR_HOME);
    puts(ESC_ERASE_DISPLAY_ALL);

    twins::init(&tios);
    twins::drawWidget(&wndMain);

    // for (;;)
    // {
    //     // int x = getchar();
    //     int x = fgetc(stdin);
    //     putchar(x);
    //     puts(ESC_ERASE_LINE_RIGHT);
    //     puts(ESC_CURSOR_HOME);
    //     fflush(stdout);
    // }
}
