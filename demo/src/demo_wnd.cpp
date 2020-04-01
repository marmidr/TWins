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
        link    : {},
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
        link    : {},
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
        link    : {},
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
        link    : { (const twins::Widget[]) // set first field in union - pChilds
        {
            {
                type    : twins::Widget::Label,
                id      : ID_LABEL_FW_VERSION,
                coord   : { 2, 1 },
                size    : { 12, 1 },
                link    : {},
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
                link    : {},
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
                link    : {},
                { label : {
                    bgColor : twins::ColorBG::White,
                    fgColor : twins::ColorFG::Magenta,
                    text    : "Time≡" __TIME__
                }}
            },
            { /* NUL */ }
        }},
        { panel : {
            bgColor     : twins::ColorBG::Green,
            fgColor     : twins::ColorFG::White,
            title       : "VER",
        }} // panel
    },
    {
        type    : twins::Widget::Panel,
        id      : ID_PANEL_STATE,
        coord   : { 30, 1 },
        size    : { 25, 3 },
        link    : { pnlStateChilds },
        { panel : {
            bgColor     : twins::ColorBG::White,
            fgColor     : twins::ColorFG::Blue,
            title       : "STATE: Leds",
        }} // panel
    },
    {
        type    : twins::Widget::Panel,
        id      : ID_PANEL_KEY,
        coord   : { 1, 7 },
        size    : { 26, 4 },
        link    : { (const twins::Widget[])
        {
            {
                type    : twins::Widget::Label,
                id      : ID_LABEL_KEYSEQ,
                coord   : { 2, 1 },
                size    : { 22, 1 },
                link    : {},
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
                link    : {},
                { label : {
                    bgColor : twins::ColorBG::White,
                    fgColor : twins::ColorFG::Green,
                    text    : nullptr // use callback to get text
                }}
            },
            { /* NUL */ }
        }},
        { panel : {
            bgColor     : twins::ColorBG::Cyan,
            fgColor     : twins::ColorFG::White,
            title       : "KEY-CODES",
        }} // panel
    },
    {
        type    : twins::Widget::CheckBox,
        id      : ID_CHBX_ENBL,
        coord   : { 30, 5 },
        size    : {},
        link    : {},
        { checkbox : {
            text    : "Enable ",
            fgColor : twins::ColorFG::Green
        }}
    },
    {
        type    : twins::Widget::CheckBox,
        id      : ID_CHBX_LOCK,
        coord   : { 45, 5 },
        size    : {},
        link    : {},
        { checkbox : {
            text    : "Lock "
        }}
    },
    {
        type    : twins::Widget::Button,
        id      : ID_BTN_YES,
        coord   : { 30, 7 },
        size    : {},
        link    : {},
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
        link    : {},
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
        link    : {},
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
        link    : {},
        { progressbar : {
            fgColor : twins::ColorFG::Yellow
        }}
    },
    { /* NUL */ }
};

static constexpr twins::Widget page2Childs[] =
{
    {
        type    : twins::Widget::ListBox,
        id      : ID_LISTBOX,
        coord   : { 2, 2 },
        size    : { 20, 6 },
        link    : {},
        { listbox : {
            //
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
    coord   : { 15,  2 },
    size    : { 80, 15 },
    link    : { (const twins::Widget[])
    {
        {
            type    : twins::Widget::PageCtrl,
            id      : ID_PGCONTROL,
            coord   : { 1,  1 },
            size    : { 75, 12 },
            link    : { (const twins::Widget[])
            {
                {
                    type    : twins::Widget::Page,
                    id      : ID_PAGE_1,
                    coord   : {},
                    size    : {},
                    link    : { page1Childs },
                    { page : {
                        fgColor     : twins::ColorFG::Yellow,
                        title       : "Version",
                    }}
                },
                {
                    type    : twins::Widget::Page,
                    id      : ID_PAGE_2,
                    coord   : {},
                    size    : {},
                    link    : { page2Childs },
                    { page : {
                        fgColor     : twins::ColorFG::BlackIntense,
                        title       : "Service ∑",
                    }}
                },
                {
                    type    : twins::Widget::Page,
                    id      : ID_PAGE_3,
                    coord   : {},
                    size    : {},
                    link    : { },
                    { page : {
                        fgColor     : twins::ColorFG::Yellow,
                        title       : "Diagnostics",
                    }}
                },
                { /* NUL */ }
            }},
            { pagectrl : {
                tabWidth    : 14,
            }},
        },
        { /* NUL */ }
    }},
    { window : {
        bgColor     : twins::ColorBG::Blue,
        fgColor     : twins::ColorFG::White,
        title       : "Service Menu " ESC_UNDERLINE_ON "(Ctrl+D quit)" ESC_UNDERLINE_OFF,
        getState    : getWindMainState,
    }} // window
};

// -----------------------------------------------------------------------------

constexpr auto wndMainArray = twins::transforWindowDefinition<&wndMain>();
const twins::Widget * pWndMainArray = wndMainArray.begin();
