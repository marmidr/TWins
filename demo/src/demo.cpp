/******************************************************************************
 * @brief   TWins - demo application
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#include "twins.hpp"
#include <stdio.h>

// -----------------------------------------------------------------------------


enum Wnd1Cid
{
    CID_INVALID,
    CID_WND,
    CID_PANEL_VERSIONS,
    CID_LABEL_FW_VERSION,
    CID_LABEL_DATE_TIME,
};

const twins::WindowCallbacks wnd1cbks =
{
    .onDraw =    [](const twins::Widget* pWdgt) { return false; },
    .isEnabled = [](const twins::Widget* pWdgt)
        {
            static bool pnlEnabled = true;

            if (pWdgt->id == CID_PANEL_VERSIONS)
                return pnlEnabled;
            return false;
        },
    .isFocused = [](const twins::Widget* pWdgt) { return false; },
    .isVisible = [](const twins::Widget* pWdgt) { return false; },
    .isCheckboxChecked = [](const twins::Widget* pWdgt) { return false; },
};

const twins::Widget wnd1[] =
{
    {
        .type = twins::Widget::Window,
        .id = CID_WND,
        .coord = { 0, 0 },
        .size = { 50, 25 },
        .window =
        {
            .frameStyle = twins::FrameStyle::Double,
            .bgColor = twins::ColorBG::BLUE_INTENSE,
            .caption = "Service Menu",
            .pCallbacks = &wnd1cbks,
            .pChildrens = wnd1 + 1,
        }
    },
    {
        .type = twins::Widget::Panel,
        .id = CID_PANEL_VERSIONS,
        .coord = { 3, 3 },
        .size = { 44, 20 },
        .panel =
        {
            .frameStyle = twins::FrameStyle::Single,
            .bgColor = twins::ColorBG::BLUE,
            .pChildrens = (twins::Widget[])
            {
                {
                    .type = twins::Widget::Label,
                    .id = CID_LABEL_FW_VERSION,
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
                    .id = CID_LABEL_DATE_TIME,
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
        }
    },
};

#define CTR __COUNTER__

int main()
{
    printf("Win1 controls: %u" "\n", twins::arrSize(wnd1));
    printf("sizeof Widget: %zu" "\n", sizeof(twins::Widget));
}
