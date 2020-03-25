/******************************************************************************
 * @brief   TWins - demo window definition
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#include "twins.hpp"

// -----------------------------------------------------------------------------

enum WndMainIDs
{
    ID_INVALID, // value 0 is reserved to WIDGET_ID_NONE
    ID_WND,
        ID_PGCONTROL,
            ID_PAGE_1,
                ID_PANEL_VERSIONS,
                    ID_LABEL_FW_VERSION,
                    ID_LABEL_DATE,
                    ID_LABEL_TIME,
                ID_PANEL_STATE,
                    ID_LED_PUMP,
                    ID_LED_LOCK,
                    ID_LED_BATTERY,
                ID_PANEL_KEY,
                    ID_LABEL_KEYSEQ,
                    ID_LABEL_KEYNAME,
                ID_CHBX_ENBL,
                ID_CHBX_LOCK,
                ID_BTN_YES,
                ID_BTN_NO,
                ID_BTN_CANCEL,
                ID_PRGBAR_1,
            ID_PAGE_2,
            ID_PAGE_3,
};

// -----------------------------------------------------------------------------

extern twins::IWindowState * getWindMainState();

const twins::Widget pnlStateChilds[] =
{
    {
        type    : twins::Widget::Led,
        id      : ID_LED_BATTERY,
        coord   : { 2, 1 },
        size    : {},
        { led : {
            bgColorOff  : twins::ColorBG::White,
            bgColorOn   : twins::ColorBG::Magenta,
            fgColor     : twins::ColorFG::Black,
            text        : "(BATT)"
        }}
    },
    {
        type    : twins::Widget::Led,
        id      : ID_LED_LOCK,
        coord   : { 9, 1 },
        size    : {},
        { led : {
            bgColorOff  : twins::ColorBG::White,
            bgColorOn   : twins::ColorBG::Green,
            fgColor     : twins::ColorFG::Black,
            text        : "(LOCK)"
        }}
    },
    {
        type    : twins::Widget::Led,
        id      : ID_LED_PUMP,
        coord   : { 16, 1 },
        size    : {},
        { led : {
            bgColorOff  : twins::ColorBG::White,
            bgColorOn   : twins::ColorBG::Yellow,
            fgColor     : twins::ColorFG::Black,
            text        : "(PUMP)"
        }}
    },
};

const twins::Widget page1Childs[] =
{
    {
        type    : twins::Widget::Panel,
        id      : ID_PANEL_VERSIONS,
        coord   : { 1, 1 },
        size    : { 21, 5 },
        { panel : {
            bgColor     : twins::ColorBG::Green,
            fgColor     : twins::ColorFG::White,
            title       : "VER",
            pChildrens  : (const twins::Widget[])
            {
                {
                    type    : twins::Widget::Label,
                    id      : ID_LABEL_FW_VERSION,
                    coord   : { 2, 1 },
                    size    : { 12, 1 },
                    { label : {
                        bgColor : twins::ColorBG::Black,
                        fgColor : twins::ColorFG::MagentaIntense,
                        text    : "FwVer: 1.1"
                    }}
                },
                {
                    type    : twins::Widget::Label,
                    id      : ID_LABEL_DATE,
                    coord   : { 2, 2 },
                    size    : { 16, 1 },
                    { label : {
                        bgColor : twins::ColorBG::None,
                        fgColor : twins::ColorFG::Yellow,
                        text    : "Date•" __DATE__
                    }}
                },
                {
                    type    : twins::Widget::Label,
                    id      : ID_LABEL_TIME,
                    coord   : { 2, 3 },
                    size    : { 16, 1 },
                    { label : {
                        bgColor : twins::ColorBG::White,
                        fgColor : twins::ColorFG::Magenta,
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
        coord   : { 30, 1 },
        size    : { 25, 3 },
        { panel : {
            bgColor     : twins::ColorBG::White,
            fgColor     : twins::ColorFG::Blue,
            title       : "STATE: Leds",
            pChildrens  : pnlStateChilds,
            childCount  : 3 //twins::arrSize(pnlStateChilds)
        }} // panel
    },
    {
        type    : twins::Widget::Panel,
        id      : ID_PANEL_KEY,
        coord   : { 1, 7 },
        size    : { 26, 4 },
        { panel : {
            bgColor     : twins::ColorBG::Cyan,
            fgColor     : twins::ColorFG::White,
            title       : "KEY-CODES",
            pChildrens  : (const twins::Widget[])
            {
                {
                    type    : twins::Widget::Label,
                    id      : ID_LABEL_KEYSEQ,
                    coord   : { 2, 1 },
                    size    : { 22, 1 },
                    { label : {
                        bgColor : twins::ColorBG::White,
                        fgColor : twins::ColorFG::Red,
                        text    : nullptr // use callback to get text
                    }}
                },
                {
                    type    : twins::Widget::Label,
                    id      : ID_LABEL_KEYNAME,
                    coord   : { 2, 2 },
                    size    : { 17, 1 },
                    { label : {
                        bgColor : twins::ColorBG::White,
                        fgColor : twins::ColorFG::Green,
                        text    : nullptr // use callback to get text
                    }}
                },
            },
            childCount : 2
        }} // panel
    },
    {
        type    : twins::Widget::CheckBox,
        id      : ID_CHBX_ENBL,
        coord   : { 30, 5 },
        size    : { },
        { checkbox : {
            text    : "Enable ",
            fgColor : twins::ColorFG::Green
        }}
    },
    {
        type    : twins::Widget::CheckBox,
        id      : ID_CHBX_LOCK,
        coord   : { 45, 5 },
        size    : { },
        { checkbox : {
            text    : "Lock "
        }}
    },
    {
        type    : twins::Widget::Button,
        id      : ID_BTN_YES,
        coord   : { 30, 7 },
        size    : {},
        { button : {
            text    : "YES",
            groupId : 1
        }}
    },
    {
        type    : twins::Widget::Button,
        id      : ID_BTN_NO,
        coord   : { 38, 7 },
        size    : {},
        { button : {
            text    : "NO",
            groupId : 1,
            fgColor : twins::ColorFG::Red
        }}
    },
    {
        type    : twins::Widget::Button,
        id      : ID_BTN_CANCEL,
        coord   : { 45, 7 },
        size    : {},
        { button : {
            text    : "CANCEL",
            groupId : 1
        }}
    },
    {
        type    : twins::Widget::ProgressBar,
        id      : ID_PRGBAR_1,
        coord   : { 30, 9 },
        size    : { 25, 1 },
        { progressbar : {
            fgColor : twins::ColorFG::Yellow
        }}
    }
};

const twins::Widget wndMain =
{
    // NOTE: all members must be initialized, in order they are declared,
    // otherwise GCC may fail to compile: 'sorry, unimplemented: non-trivial designated initializers not supported'

    // C-style:     .onDraw = []
    // C++ style:   onDraw : []

    type    : twins::Widget::Window,
    id      : ID_WND,
    coord   : { 15,  2 },
    size    : { 80, 15 },
    { window : {
        bgColor     : twins::ColorBG::Blue,
        fgColor     : twins::ColorFG::White,
        title       : "Service Menu " ESC_UNDERLINE_ON "(Ctrl+D quit)" ESC_UNDERLINE_OFF,
        getState    : getWindMainState,
        pChildrens  : (const twins::Widget[])
        {
            {
                type    : twins::Widget::PageCtrl,
                id      : ID_PGCONTROL,
                coord   : { 1,  1 },
                size    : { 75, 12 },
                { pagectrl : {
                    tabWidth    : 14,
                    pChildrens  : (const twins::Widget[])
                    {
                        {
                            type    : twins::Widget::Page,
                            id      : ID_PAGE_1,
                            coord   : {},
                            size    : {},
                            { page : {
                                fgColor     : twins::ColorFG::Yellow,
                                title       : "Version",
                                pChildrens  : page1Childs,
                                childCount  : 9
                            }}
                        },
                        {
                            type    : twins::Widget::Page,
                            id      : ID_PAGE_2,
                            coord   : {},
                            size    : {},
                            { page : {
                                fgColor     : twins::ColorFG::BlackIntense,
                                title       : "Service ∑",
                                pChildrens  : {},
                                childCount  : {}
                            }}
                        },
                        {
                            type    : twins::Widget::Page,
                            id      : ID_PAGE_3,
                            coord   : {},
                            size    : {},
                            { page : {
                                fgColor     : twins::ColorFG::Yellow,
                                title       : "Diagnostics",
                                pChildrens  : {},
                                childCount  : {}
                            }}
                        },
                    },
                    childCount  : 3
                }},
            },
        },
        childCount : 1,
    }} // window
};

