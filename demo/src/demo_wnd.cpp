/******************************************************************************
 * @brief   TWins - demo window definition
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#include "twins_transform_window.hpp"
#include "demo_wnd.hpp"

// -----------------------------------------------------------------------------

// Theme support enabled - user must provide theme colors
namespace twins
{

// RGB color values: https://en.wikipedia.org/wiki/Web_colors

const char* encodeClTheme(ColorFG cl)
{
    switch (cl)
    {
    // double-state colors
    case ColorFG::Checkbox:         return ESC_FG_YELLOW;
    case ColorFG::CheckboxIntense:  return ESC_FG_YELLOW_INTENSE;
    case ColorFG::Radio:            return ESC_FG_GREEN;
    case ColorFG::RadioIntense:     return ESC_FG_GREEN_INTENSE;
    // one-state colors
    case ColorFG::Window:           return ESC_FG_WHITE;
    case ColorFG::Label:            return ESC_FG_WHITE;
    case ColorFG::Listbox:          return ESC_FG_GREEN;
    case ColorFG::Button:           return ESC_FG_BLACK;
    case ColorFG::ButtonGreen:      return ESC_FG_WHITE;
    case ColorFG::ButtonRed:        return ESC_FG_WHITE;
    case ColorFG::ButtonOrange:     return ESC_FG_RGB(139, 0,   0);
    case ColorFG::PanelChbox:       return ESC_FG_RGB(0,   0, 205);
    default:                        return ESC_FG_DEFAULT;
    }
}

const char* encodeClTheme(ColorBG cl)
{
    switch (cl)
    {
    case ColorBG::Window:       return ESC_BG_BLUE;
    case ColorBG::Listbox:      return ESC_BG_WHITE;
    case ColorBG::Button:       return ESC_BG_BLACK_INTENSE;
    case ColorBG::ButtonGreen:  return ESC_BG_GREEN;
    case ColorBG::ButtonRed:    return ESC_BG_RED;
    case ColorBG::ButtonOrange: return ESC_BG_RGB(255, 165,   0);
    case ColorBG::PanelChbox:   return ESC_BG_RGB(135, 206, 235);
    default:                    return ESC_BG_DEFAULT;
    }
}

ColorFG intenseClTheme(ColorFG cl)
{
    switch (cl)
    {
    case ColorFG::Checkbox: return ColorFG::CheckboxIntense;
    case ColorFG::Radio:    return ColorFG::RadioIntense;
    default:                return cl;
    }
}

ColorBG intenseClTheme(ColorBG cl)
{
    return cl;
}

}

// -----------------------------------------------------------------------------

static constexpr twins::Widget pnlStateChilds[] =
{
    {
        type    : twins::Widget::Led,
        id      : ID_LED_BATTERY,
        coord   : { 2, 1 },
        size    : {},
        { led : {
            text        : "(BATT)",
            fgColor     : twins::ColorFG::Black,
            bgColorOff  : twins::ColorBG::White,
            bgColorOn   : twins::ColorBG::Magenta,
        }}
    },
    {
        type    : twins::Widget::Led,
        id      : ID_LED_LOCK,
        coord   : { 9, 1 },
        size    : {},
        { led : {
            text        : "(LOCK)",
            fgColor     : twins::ColorFG::Black,
            bgColorOff  : twins::ColorBG::White,
            bgColorOn   : twins::ColorBG::Green,
        }}
    },
    {
        type    : twins::Widget::Led,
        id      : ID_LED_PUMP,
        coord   : { 16, 1 },
        size    : {},
        { led : {
            text        : "(PUMP)",
            fgColor     : twins::ColorFG::Black,
            bgColorOff  : twins::ColorBG::White,
            bgColorOn   : twins::ColorBG::Yellow,
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
            title       : "VER",
            fgColor     : twins::ColorFG::White,
            bgColor     : twins::ColorBG::Green,
        }},
        link    : { (const twins::Widget[]) // set first field in union - pChilds
        {
            {
                type    : twins::Widget::Label,
                id      : ID_LABEL_FW_VERSION,
                coord   : { 2, 1 },
                size    : { 12, 1 },
                { label : {
                    text    : "FwVer: 1.1",
                    fgColor : twins::ColorFG::YellowIntense,
                }}
            },
            {
                type    : twins::Widget::Label,
                id      : ID_LABEL_DATE,
                coord   : { 2, 2 },
                size    : { 15, 1 },
                { label : {
                    text    : "Date•" __DATE__,
                    fgColor : twins::ColorFG::Black,
                }}
            },
            {
                type    : twins::Widget::Label,
                id      : ID_LABEL_TIME,
                coord   : { 2, 3 },
                size    : { 16, 1 },
                { label : {
                    text    : "Time≡" __TIME__,
                    fgColor : twins::ColorFG::Magenta,
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
            title       : "STATE: Leds",
            fgColor     : twins::ColorFG::Blue,
            bgColor     : twins::ColorBG::White,
        }},
        link    : { pnlStateChilds }
    },
    {
        type    : twins::Widget::Panel,
        id      : ID_PANEL_KEY,
        coord   : { 1, 7 },
        size    : { 26, 4 },
        { panel : {
            title       : "KEY-CODES",
            fgColor     : twins::ColorFG::White,
            bgColor     : twins::ColorBG::Cyan,
        }},
        link    : { (const twins::Widget[])
        {
            {
                type    : twins::Widget::Label,
                id      : ID_LABEL_KEYSEQ,
                coord   : { 2, 1 },
                size    : { 22, 1 },
                { label : {
                    text    : nullptr, // use callback to get text
                    fgColor : twins::ColorFG::White,
                }}
            },
            {
                type    : twins::Widget::Label,
                id      : ID_LABEL_KEYNAME,
                coord   : { 2, 2 },
                size    : { 17, 1 },
                { label : {
                    text    : nullptr, // use callback to get text
                    fgColor : twins::ColorFG::White,
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
            fgColor : twins::ColorFG::ButtonGreen,
            bgColor : twins::ColorBG::ButtonGreen,
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
            fgColor : twins::ColorFG::ButtonOrange,
            bgColor : twins::ColorBG::ButtonOrange,
            style   : twins::ButtonStyle::Solid
        }}
    },
    {
        type    : twins::Widget::Button,
        id      : ID_BTN_CANCEL,
        coord   : { 45, 7 },
        size    : {},
        { button : {
            text    : "CANCEL",
            fgColor : twins::ColorFG::White,
            bgColor : {},
            style   : twins::ButtonStyle::Simple
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
            text    : "  ▫▫▫▫▫ " ESC_INVERSE_ON "ListBox" ESC_INVERSE_OFF " ▫▫▫▫▫" "\n"
                      "• " ESC_UNDERLINE_ON "Up/Down" ESC_UNDERLINE_OFF " -> change item" "\n"
                      "• " ESC_UNDERLINE_ON "PgUp/PgDown" ESC_UNDERLINE_OFF " -> scroll page" "\n"
                      "• " ESC_UNDERLINE_ON "Enter" ESC_UNDERLINE_OFF " -> select the item",
            fgColor : twins::ColorFG::YellowIntense,
        }}
    },
    {
        type    : twins::Widget::ListBox,
        id      : ID_LISTBOX,
        coord   : { 2, 2 },
        size    : { 20, 8 },
        { listbox : {
            fgColor :   twins::ColorFG::Green,
            bgColor :   twins::ColorBG::White,
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
            groupId : 1,
            fgColor : twins::ColorFG::Radio
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
            groupId : 1,
            fgColor : twins::ColorFG::Yellow
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
            title       : {},
            fgColor     : twins::ColorFG::White,
            bgColor     : twins::ColorBG::White,
            noFrame     : true
        }},
        link    : { (const twins::Widget[]) // set first field in union - pChilds
        {
            {
                type    : twins::Widget::Edit,
                id      : ID_EDT_1,
                coord   : { 1, 1 },
                size    : { 30, 1 },
                { edit : {
                    fgColor     : twins::ColorFG::Black,
                    bgColor     : twins::ColorBG::Cyan,
                }}
            },
            {
                type    : twins::Widget::Edit,
                id      : ID_EDT_2,
                coord   : { 1, 3 },
                size    : { 30, 1 },
                { edit : {
                    fgColor     : twins::ColorFG::Black,
                    bgColor     : twins::ColorBG::Yellow,
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
            title       : {},
            fgColor     : twins::ColorFG::PanelChbox,
            bgColor     : twins::ColorBG::PanelChbox,
        }},
        link    : { (const twins::Widget[]) // set first field in union - pChilds
        {
            {
                type    : twins::Widget::Label,
                id      : ID_LBL_CHBXTITLE,
                coord   : { 2, 2 },
                size    : { 14, 1 },
                { label : {
                    text    : "Check list:",
                    fgColor : twins::ColorFG::Blue,
                }}
            },
            {
                type    : twins::Widget::CheckBox,
                id      : ID_CHBX_A,
                coord   : { 2, 4 },
                size    : {},
                { checkbox : {
                    text    : "Check A",
                    fgColor : twins::ColorFG::Green
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
        fgColor     : twins::ColorFG::White,
        bgColor     : twins::ColorBG::Blue,
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
                        title       : "Version",
                        fgColor     : twins::ColorFG::Yellow,
                    }},
                    link    : { page1Childs }
                },
                {
                    type    : twins::Widget::Page,
                    id      : ID_PAGE_2,
                    coord   : {},
                    size    : {},
                    { page : {
                        title       : "Service ∑",
                        fgColor     : twins::ColorFG::White,
                    }},
                    link    : { page2Childs }
                },
                {
                    type    : twins::Widget::Page,
                    id      : ID_PAGE_3,
                    coord   : {},
                    size    : {},
                    { page : {
                        title       : "Diagnostics",
                        fgColor     : twins::ColorFG::Yellow,
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
                text    :  ESC_BOLD "F4 "           ESC_NORMAL ESC_BG_CYAN "Mouse On/Off" ESC_BG_BLUE "  "
                           ESC_BOLD "F5 "           ESC_NORMAL ESC_BG_CYAN "Refresh" ESC_BG_BLUE "  "
                           ESC_BOLD "F6 "           ESC_NORMAL ESC_BG_CYAN "Clr Logs" ESC_BG_BLUE "  ",
                fgColor : twins::ColorFG::White,
            }}
        },
        { /* NUL */ }
    }}
};

// -----------------------------------------------------------------------------

constexpr auto wndMainArray = twins::transforWindowDefinition<&wndMain>();
const twins::Widget * pWndMainArray = wndMainArray.begin();
const uint16_t wndMainNumPages = twins::getPagesCount(&wndMain);
