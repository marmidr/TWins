/******************************************************************************
 * @brief   TWins - demo window definition
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#include "twins.hpp"

// -----------------------------------------------------------------------------

enum WndMainIDs
{
    ID_INVALID,
    ID_WND,
        ID_PANEL_VERSIONS,
            ID_LABEL_FW_VERSION,
            ID_LABEL_DATE,
            ID_LABEL_TIME,
        ID_PANEL_STATE,
            ID_LED_PUMP,
            ID_LED_LOCK,
            ID_LED_BATTERY,
        ID_PANEL_KEYCODE,
            ID_LABEL_KEYCODE,
        ID_CHKBX1,
        ID_BTN_YES,
        ID_BTN_NO,
        ID_PRGBAR1,
};

// -----------------------------------------------------------------------------

extern twins::IWindowState * getWind1State();

const twins::Widget pnlStateChilds[] =
{
    {
        type    : twins::Widget::Led,
        id      : ID_LED_BATTERY,
        coord   : { 2, 1 },
        size    : { 6, 1 },
        { led : {
            bgColorOff  : twins::ColorBG::WHITE,
            bgColorOn   : twins::ColorBG::MAGENTA,
            fgColor     : twins::ColorFG::BLACK,
            text        : "(BATT)"
        }}
    },
    {
        type    : twins::Widget::Led,
        id      : ID_LED_LOCK,
        coord   : { 9, 1 },
        size    : { 6, 1 },
        { led : {
            bgColorOff  : twins::ColorBG::WHITE,
            bgColorOn   : twins::ColorBG::GREEN,
            fgColor     : twins::ColorFG::BLACK,
            text        : "(LOCK)"
        }}
    },
    {
        type    : twins::Widget::Led,
        id      : ID_LED_PUMP,
        coord   : { 16, 1 },
        size    : { 6, 1 },
        { led : {
            bgColorOff  : twins::ColorBG::WHITE,
            bgColorOn   : twins::ColorBG::YELLOW,
            fgColor     : twins::ColorFG::BLACK,
            text        : "(PUMP)"
        }}
    },
};

const twins::Widget wndMain =
{
    // NOTE: all members must be initialized, in order they are declared,
    // otherwise GCC may fail to compile: 'sorry, unimplemented: non-trivial designated initializers not supported'

    // C-style:     .onDraw = []
    // C++ style:   onDraw : []

    type    : twins::Widget::Window,
    id      : ID_WND,
    coord   : { 10, 4 },
    size    : { 60, 12 },
    { window : {
        bgColor     : twins::ColorBG::BLUE,
        fgColor     : twins::ColorFG::WHITE,
        caption     : "Service Menu (Ctrl+D quit)",
        getState    : getWind1State,
        pChildrens  : (const twins::Widget[])
        {
            {
                type    : twins::Widget::Panel,
                id      : ID_PANEL_VERSIONS,
                coord   : { 2, 2 },
                size    : { 21, 5 },
                { panel : {
                    bgColor     : twins::ColorBG::GREEN,
                    fgColor     : twins::ColorFG::WHITE,
                    caption     : "VER",
                    pChildrens  : (const twins::Widget[])
                    {
                        {
                            type    : twins::Widget::Label,
                            id      : ID_LABEL_FW_VERSION,
                            coord   : { 2, 1 },
                            size    : { 14, 1 },
                            { label : {
                                bgColor : twins::ColorBG::BLACK_INTENSE,
                                fgColor : twins::ColorFG::WHITE,
                                text    : "FwVer: 1"
                            }}
                        },
                        {
                            type    : twins::Widget::Label,
                            id      : ID_LABEL_DATE,
                            coord   : { 2, 2 },
                            size    : { 16, 1 },
                            { label : {
                                bgColor : twins::ColorBG::WHITE,
                                fgColor : twins::ColorFG::BLACK,
                                text    : "Date•" __DATE__
                            }}
                        },
                        {
                            type    : twins::Widget::Label,
                            id      : ID_LABEL_TIME,
                            coord   : { 2, 3 },
                            size    : { 16, 1 },
                            { label : {
                                bgColor : twins::ColorBG::WHITE,
                                fgColor : twins::ColorFG::BLACK,
                                text    : "Time≡" __TIME__
                            }}
                        },
                    },
                    childCount : 3
                }} // panel
            },
            {
                type    : twins::Widget::Panel,
                id      : ID_PANEL_STATE,
                coord   : { 30, 2 },
                size    : { 25, 3 },
                { panel : {
                    bgColor     : twins::ColorBG::WHITE,
                    fgColor     : twins::ColorFG::BLUE,
                    caption     : "STATE: Leds",
                    pChildrens  : pnlStateChilds,
                    childCount  : 3 //twins::arrSize(pnlStateChilds)
                }} // panel
            },
            {
                type    : twins::Widget::Panel,
                id      : ID_PANEL_KEYCODE,
                coord   : { 2, 8 },
                size    : { 21, 3 },
                { panel : {
                    bgColor     : twins::ColorBG::CYAN,
                    fgColor     : twins::ColorFG::WHITE,
                    caption     : "KEY-CODES",
                    pChildrens  : (const twins::Widget[])
                    {
                        {
                            type    : twins::Widget::Label,
                            id      : ID_LABEL_KEYCODE,
                            coord   : { 2, 1 },
                            size    : { 17, 1 },
                            { label : {
                                bgColor : twins::ColorBG::WHITE,
                                fgColor : twins::ColorFG::RED,
                                text    : nullptr // use callback to get text
                            }}
                        },
                    },
                    childCount : 1
                }} // panel
            },
            {
                type    : twins::Widget::CheckBox,
                id      : ID_CHKBX1,
                coord   : { 30, 6 },
                size    : { 12, 1 },
                { checkbox : {
                    text    : "Enable "
                }}
            },
            {
                type    : twins::Widget::Button,
                id      : ID_BTN_YES,
                coord   : { 30, 8 },
                size    : { 8, 1 },
                { button : {
                    text    : "YES",
                    groupId : 1
                }}
            },
            {
                type    : twins::Widget::Button,
                id      : ID_BTN_NO,
                coord   : { 38, 8 },
                size    : { 8, 1 },
                { button : {
                    text    : "NO!",
                    groupId : 1
                }}
            },
            {
                type    : twins::Widget::ProgressBar,
                id      : ID_PRGBAR1,
                coord   : { 30, 10 },
                size    : { 20, 1 },
                { progressbar : {
                    //
                }}
            },
        },
        childCount : 7,
    }} // window
};

