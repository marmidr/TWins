/******************************************************************************
 * @brief   TWins - demo window definition
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#include "twins_transform_window.hpp"
#include "demo_wnd.hpp"

// -----------------------------------------------------------------------------

static constexpr twins::Widget pnlStateChilds[] =
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
    // at the end of child's array must lay terminating element;
    // it is needed by window transformation routine and ignored in final definition
    { /* NUL */ }
};

static constexpr twins::Widget page1Childs[] =
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
        }},
        link    : { (const twins::Widget[]) // set first field in union - pChilds
        {
            {
                type    : twins::Widget::Label,
                id      : ID_LABEL_FW_VERSION,
                coord   : { 2, 1 },
                size    : { 12, 1 },
                { label : {
                    fgColor : twins::ColorFG::YellowIntense,
                    text    : "FwVer: 1.1"
                }}
            },
            {
                type    : twins::Widget::Label,
                id      : ID_LABEL_DATE,
                coord   : { 2, 2 },
                size    : { 15, 1 },
                { label : {
                    fgColor : twins::ColorFG::Black,
                    text    : "Date•" __DATE__
                }}
            },
            {
                type    : twins::Widget::Label,
                id      : ID_LABEL_TIME,
                coord   : { 2, 3 },
                size    : { 16, 1 },
                { label : {
                    fgColor : twins::ColorFG::Magenta,
                    text    : "Time≡" __TIME__
                }}
            },
            { /* NUL */ }
        }}
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
        }},
        link    : { pnlStateChilds }
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
        }},
        link    : { (const twins::Widget[])
        {
            {
                type    : twins::Widget::Label,
                id      : ID_LABEL_KEYSEQ,
                coord   : { 2, 1 },
                size    : { 22, 1 },
                { label : {
                    fgColor : twins::ColorFG::White,
                    text    : nullptr // use callback to get text
                }}
            },
            {
                type    : twins::Widget::Label,
                id      : ID_LABEL_KEYNAME,
                coord   : { 2, 2 },
                size    : { 17, 1 },
                { label : {
                    fgColor : twins::ColorFG::White,
                    text    : nullptr // use callback to get text
                }}
            },
            { /* NUL */ }
        }}
    },
    {
        type    : twins::Widget::CheckBox,
        id      : ID_CHBX_ENBL,
        coord   : { 30, 5 },
        size    : {},
        { checkbox : {
            text    : "Enable",
            fgColor : twins::ColorFG::Green
        }}
    },
    {
        type    : twins::Widget::CheckBox,
        id      : ID_CHBX_LOCK,
        coord   : { 45, 5 },
        size    : {},
        { checkbox : {
            text    : "Lock"
        }}
    },
    {
        type    : twins::Widget::Button,
        id      : ID_BTN_YES,
        coord   : { 30, 7 },
        size    : {},
        { button : {
            text    : "YES",
            bgColor : twins::ColorBG::Green,
            fgColor : twins::ColorFG::White,
            style   : twins::ButtonStyle::Solid
        }}
    },
    {
        type    : twins::Widget::Button,
        id      : ID_BTN_NO,
        coord   : { 38, 7 },
        size    : {},
        { button : {
            text    : "NO",
            fgColor : twins::ColorFG::Yellow,
        }}
    },
    {
        type    : twins::Widget::Button,
        id      : ID_BTN_CANCEL,
        coord   : { 45, 7 },
        size    : {},
        { button : {
            text    : "CANCEL",
            bgColor : twins::ColorBG::BlackIntense,
            fgColor : twins::ColorFG::White,
            style   : twins::ButtonStyle::Solid
        }}
    },
    {
        type    : twins::Widget::ProgressBar,
        id      : ID_PRGBAR_1,
        coord   : { 30, 9 },
        size    : { 25, 1 },
        { progressbar : {
            fgColor : twins::ColorFG::Yellow,
            style   : twins::PgBarStyle::Hash
        }}
    },
    {
        type    : twins::Widget::ProgressBar,
        id      : ID_PRGBAR_2,
        coord   : { 30, 10 },
        size    : { 12, 1 },
        { progressbar : {
            fgColor : twins::ColorFG::White,
            style   : twins::PgBarStyle::Shade
        }}
    },
    {
        type    : twins::Widget::ProgressBar,
        id      : ID_PRGBAR_3,
        coord   : { 43, 10 },
        size    : { 12, 1 },
        { progressbar : {
            fgColor : twins::ColorFG::White,
            style   : twins::PgBarStyle::Rectangle
        }}
    },
    { /* NUL */ }
};

static constexpr twins::Widget page2Childs[] =
{
    {
        type    : twins::Widget::Label,
        id      : ID_LABEL_LBHELP,
        coord   : { 24, 2 },
        size    : { 35, 4 },
        { label : {
            fgColor : twins::ColorFG::YellowIntense,
            text    : "  ▫▫▫▫▫ " ESC_INVERSE_ON "ListBox" ESC_INVERSE_OFF " ▫▫▫▫▫" "\n"
                      "• " ESC_UNDERLINE_ON "Up/Down" ESC_UNDERLINE_OFF " -> change item" "\n"
                      "• " ESC_UNDERLINE_ON "PgUp/PgDown" ESC_UNDERLINE_OFF " -> scroll page" "\n"
                      "• " ESC_UNDERLINE_ON "Enter" ESC_UNDERLINE_OFF " -> select the item"
        }}
    },
    {
        type    : twins::Widget::ListBox,
        id      : ID_LISTBOX,
        coord   : { 2, 2 },
        size    : { 20, 8 },
        { listbox : {
            bgColor :   twins::ColorBG::White,
            fgColor :   twins::ColorFG::Green
        }}
    },
    {
        type    : twins::Widget::Radio,
        id      : ID_RADIO_1,
        coord   : { 25, 7 },
        size    : {},
        { radio : {
            text    : "YES",
            radioId : 0,
            groupId : 1
        }}
    },
    {
        type    : twins::Widget::Radio,
        id      : ID_RADIO_2,
        coord   : { 35, 7 },
        size    : {},
        { radio : {
            text    : "NO",
            radioId : 1,
            groupId : 1
        }}
    },
    {
        type    : twins::Widget::Radio,
        id      : ID_RADIO_3,
        coord   : { 44, 7 },
        size    : {},
        { radio : {
            text    : "Don't know",
            radioId : 2,
            groupId : 1
        }}
    },
    { /* NUL */ }
};

static constexpr twins::Widget page3Childs[] =
{
    {
        type    : twins::Widget::Panel,
        id      : ID_PANEL_EDT,
        coord   : { 2, 1 },
        size    : { 32, 5 },
        { panel : {
            bgColor     : twins::ColorBG::White,
            fgColor     : twins::ColorFG::White,
            title       : {},
        }},
        link    : { (const twins::Widget[]) // set first field in union - pChilds
        {
            {
                type    : twins::Widget::Edit,
                id      : ID_EDT_1,
                coord   : { 1, 1 },
                size    : { 30, 1 },
                { edit : {
                    bgColor     : twins::ColorBG::Cyan,
                    fgColor     : twins::ColorFG::Black,
                }}
            },
            {
                type    : twins::Widget::Edit,
                id      : ID_EDT_2,
                coord   : { 1, 3 },
                size    : { 30, 1 },
                { edit : {
                    bgColor     : twins::ColorBG::Yellow,
                    fgColor     : twins::ColorFG::Black,
                }}
            },
            { /* NUL */ }
        }}
    },
    {
        type    : twins::Widget::Canvas,
        id      : ID_CANVAS,
        coord   : { 2, 6 },
        size    : { 32, 4 },
    },
    {
        type    : twins::Widget::Panel,
        id      : ID_PANEL_CHBX,
        coord   : { 36, 1 },
        size    : { 22, 10 },
        { panel : {
            bgColor     : twins::ColorBG::Blue,
            fgColor     : twins::ColorFG::White,
            title       : {},
        }},
        link    : { (const twins::Widget[]) // set first field in union - pChilds
        {
            {
                type    : twins::Widget::Label,
                id      : ID_LBL_CHBXTITLE,
                coord   : { 2, 2 },
                size    : { 14, 1 },
                { label : {
                    fgColor : twins::ColorFG::White,
                    text    : "Check list:"
                }}
            },
            {
                type    : twins::Widget::CheckBox,
                id      : ID_CHBX_A,
                coord   : { 2, 4 },
                size    : {},
                { checkbox : {
                    text    : "Check A"
                }}
            },
            {
                type    : twins::Widget::CheckBox,
                id      : ID_CHBX_B,
                coord   : { 2, 5 },
                size    : {},
                { checkbox : {
                    text    : "Check B"
                }}
            },
            {
                type    : twins::Widget::CheckBox,
                id      : ID_CHBX_C,
                coord   : { 2, 6 },
                size    : {},
                { checkbox : {
                    text    : "Check C"
                }}
            },
            {
                type    : twins::Widget::CheckBox,
                id      : ID_CHBX_D,
                coord   : { 2, 7 },
                size    : {},
                { checkbox : {
                    text    : "Check D"
                }}
            },

            { /* NUL */ }
        }}
    },
    { /* NUL */ }
};

static constexpr twins::Widget wndMain =
{
    // NOTE: all members must be initialized, in order they are declared,
    // otherwise GCC may fail to compile: 'sorry, unimplemented: non-trivial designated initializers not supported'

    // C-style:     .id = ID_WND,
    // C++ style:   id : ID_WND,

    type    : twins::Widget::Window,
    id      : ID_WND,
    coord   : { 15, 1 },
    size    : { 80, 15 },
    { window : {
        bgColor     : twins::ColorBG::Blue,
        fgColor     : twins::ColorFG::White,
        title       : "Service Menu " ESC_UNDERLINE_ON "(Ctrl+D quit)" ESC_UNDERLINE_OFF,
        getState    : getWindMainState,
    }},
    link    : { (const twins::Widget[])
    {
        {
            type    : twins::Widget::PageCtrl,
            id      : ID_PGCONTROL,
            coord   : { 1,  1 },
            size    : { 75, 12 },
            { pagectrl : {
                tabWidth    : 14,
            }},
            link    : { (const twins::Widget[])
            {
                {
                    type    : twins::Widget::Page,
                    id      : ID_PAGE_1,
                    coord   : {},
                    size    : {},
                    { page : {
                        fgColor     : twins::ColorFG::Yellow,
                        title       : "Version",
                    }},
                    link    : { page1Childs }
                },
                {
                    type    : twins::Widget::Page,
                    id      : ID_PAGE_2,
                    coord   : {},
                    size    : {},
                    { page : {
                        fgColor     : twins::ColorFG::White,
                        title       : "Service ∑",
                    }},
                    link    : { page2Childs }
                },
                {
                    type    : twins::Widget::Page,
                    id      : ID_PAGE_3,
                    coord   : {},
                    size    : {},
                    { page : {
                        fgColor     : twins::ColorFG::Yellow,
                        title       : "Diagnostics",
                    }},
                    link    : { page3Childs }
                },
                { /* NUL */ }
            }}
        },
        {
            type    : twins::Widget::Label,
            id      : ID_LABEL_FTR,
            coord   : { 2, 13 },
            size    : { 76, 1 },
            { label : {
                fgColor : twins::ColorFG::White,
                text    :  ESC_BOLD "F4 "           ESC_NORMAL ESC_BG_CYAN "Mouse On/Off" ESC_BG_BLUE "  "
                           ESC_BOLD "F5 "           ESC_NORMAL ESC_BG_CYAN "Refresh" ESC_BG_BLUE "  "
                           ESC_BOLD "F6 "           ESC_NORMAL ESC_BG_CYAN "Clr Logs" ESC_BG_BLUE "  "
            }}
        },
        { /* NUL */ }
    }}
};

// -----------------------------------------------------------------------------

constexpr auto wndMainArray = twins::transforWindowDefinition<&wndMain>();
const twins::Widget * pWndMainArray = wndMainArray.begin();
const uint16_t wndMainNumPages = twins::getPagesCount(&wndMain);
