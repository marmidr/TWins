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
            ID_LABEL_DATE_TIME,
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
    .isVisible = [](const twins::Widget* pWdgt) { return false; },
    .isCheckboxChecked = [](const twins::Widget* pWdgt) { return false; },
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
        .caption = "Service Menu",
        .pCallbacks = &wndMainClbcks,
        .pChildrens = (twins::Widget[])
        {
            {
                .type = twins::Widget::Panel,
                .id = ID_PANEL_VERSIONS,
                .coord = { 5, 2 },
                .size = { 20, 6 },
                .panel =
                {
                    .frameStyle = twins::FrameStyle::Single,
                    .bgColor = twins::ColorBG::GREEN,
                    .caption = "VER",
                    .pChildrens = (twins::Widget[])
                    {
                        {
                            .type = twins::Widget::Label,
                            .id = ID_LABEL_FW_VERSION,
                            .coord = { 2, 2 },
                            .size = { 12, 1 },
                            .label =
                            {
                                .bgColor = twins::ColorBG::BLACK_INTENSE,
                                .fgColor = twins::ColorFG::WHITE,
                                .caption = "FwVer: %6s"
                            }
                        },
                        {
                            .type = twins::Widget::Label,
                            .id = ID_LABEL_DATE_TIME,
                            .coord = { 2, 3 },
                            .size = { 12, 1 },
                            .label =
                            {
                                .bgColor = twins::ColorBG::BLACK_INTENSE,
                                .fgColor = twins::ColorFG::YELLOW,
                                .caption = "Date: %s"
                            }
                        },
                    },
                    .childrensCount = 2
                }
            },
            {
                .type = twins::Widget::Panel,
                .id = ID_PANEL_CONFIG,
                .coord = { 30, 2 },
                .size = { 25, 6 },
                .panel =
                {
                    .frameStyle = twins::FrameStyle::Single,
                    .bgColor = twins::ColorBG::GREEN,
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
        printf(s);
    },
    .writeStrFmt = [](const char *fmt, va_list ap)
    {
        vprintf(fmt, ap);
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
