/******************************************************************************
 * @brief   TWins - unit tests
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "twins_transform_window.hpp"
#include "twins.hpp"

// -----------------------------------------------------------------------------

class WindowTestState : public twins::IWindowState
{
public:
    twins::WID& getFocusedID() { return wgtId; };

protected:
    twins::WID wgtId = {};
};


static WindowTestState wndTestState;
static twins::IWindowState * getWndTestState() { return &wndTestState; }

enum WndTestIDs
{
    ID_INVALID,
    ID_WND,
        ID_PGCTRL,
            ID_PAGE1,
                ID_LBL1,
                ID_LBL2,
                ID_BTN1,
                ID_BTN2,
                ID_LED,
            ID_PAGE2,
        ID_PANEL,
        ID_EDIT,
        ID_RADIO,
        ID_CHECK,
        ID_PROGRESS,
        ID_LISTBOX,
        ID_TEXTBOX,
};


static constexpr twins::Widget wndTest =
{
    type    : twins::Widget::Window,
    id      : ID_WND,
    coord   : { 1, 1 },
    size    : { 100, 50 },
    { window : {
        title       : {},
        fgColor     : {},
        bgColor     : {},
        isPopup     : false,
        getState    : getWndTestState,
    }},
    link    : { (const twins::Widget[])
    {
        {
            type    : twins::Widget::PageCtrl,
            id      : ID_PGCTRL,
            coord   : { 2, 2 },
            size    : { 80, 40 },
            { pagectrl : {
                tabWidth    : 20,
            }},
            link    : { (const twins::Widget[])
            {
                {
                    type    : twins::Widget::Page,
                    id      : ID_PAGE1,
                    coord   : { 2, 2 },
                    size    : { },
                    { page : {
                        title   : "Page title",
                        fgColor : {},
                    }},
                    link    : { (const twins::Widget[])
                    {
                        {
                            type    : twins::Widget::Label,
                            id      : ID_LBL1,
                            coord   : { 2, 2 },
                            size    : { 30, 4 },
                            { label : {
                                text    : {},
                                fgColor : {},
                                bgColor : {},
                            }}
                        },
                        {
                            type    : twins::Widget::Label,
                            id      : ID_LBL2,
                            coord   : { 2, 2 },
                            size    : { 30, 4 },
                            { label : {
                                text    : "Label: ",
                                fgColor : {},
                                bgColor : {},
                            }}
                        },
                        {
                            type    : twins::Widget::Button,
                            id      : ID_BTN1,
                            coord   : { 5, 7 },
                            size    : {},
                            { button : {
                                text    : "YES",
                                fgColor : {},
                                bgColor : {},
                                style   : twins::ButtonStyle::Solid
                            }}
                        },
                        {
                            type    : twins::Widget::Button,
                            id      : ID_BTN2,
                            coord   : { 5, 8 },
                            size    : {},
                            { button : {
                                text    : "NO",
                                fgColor : {},
                                bgColor : {},
                                style   : twins::ButtonStyle::Simple
                            }}
                        },
                        {
                            type    : twins::Widget::Led,
                            id      : ID_LED,
                            coord   : { 5, 9 },
                            size    : {},
                            { led : {
                                text        : "ENABLED",
                                fgColor     : {},
                                bgColorOff  : {},
                                bgColorOn   : {},
                            }}
                        },
                        { /* NUL */ }
                    }}
                },
                {
                    type    : twins::Widget::Page,
                    id      : ID_PAGE2,
                    coord   : { 2, 2 },
                    size    : { },
                    { page : {
                        title   : "Page 2 title",
                        fgColor : {},
                    }},
                    link    : { (const twins::Widget[])
                    {
                        { /* NUL */ }
                    }}
                },
                { /* NUL */ }
            }}
        },
        {
            type    : twins::Widget::Panel,
            id      : ID_PANEL,
            coord   : { 2, 30 },
            size    : { 30, 4 },
            { panel : {
                title   : "Panel",
                fgColor : {},
                bgColor : {},
                noFrame : {}
            }}
        },
        {
            type    : twins::Widget::Edit,
            id      : ID_EDIT,
            coord   : { 2, 30 },
            size    : { 30, 4 },
            { edit : {
                fgColor : {},
                bgColor : {},
            }}
        },
        {
            type    : twins::Widget::Radio,
            id      : ID_RADIO,
            coord   : { 2, 32 },
            size    : { 10, 1 },
            { radio : {
                text    : "Option 1",
                radioId : 1,
                groupId : 1,
                fgColor : {},
            }}
        },
        {
            type    : twins::Widget::CheckBox,
            id      : ID_CHECK,
            coord   : { 2, 34 },
            size    : { 10, 1 },
            { checkbox : {
                text    : "radio",
                fgColor : {},
            }}
        },
        {
            type    : twins::Widget::ProgressBar,
            id      : ID_PROGRESS,
            coord   : { 2, 34 },
            size    : { 10, 1 },
            { progressbar : {
                fgColor : {},
                style   : twins::PgBarStyle::Hash
            }}
        },
        {
            type    : twins::Widget::ListBox,
            id      : ID_LISTBOX,
            coord   : { 80, 2 },
            size    : { 10, 10 },
            { listbox : {
                fgColor : {},
                bgColor : {},
                noFrame : false
            }}
        },
        {
            type    : twins::Widget::TextBox,
            id      : ID_TEXTBOX,
            coord   : { 80, 20 },
            size    : { 10, 10 },
            { textbox : {
                fgColor : {},
                bgColor : {},
            }}
        },
        { /* NUL */ }
    }}
};

constexpr auto wndTestWidgets = twins::transforWindowDefinition<&wndTest>();
const twins::Widget * pWndTestWidgets = wndTestWidgets.begin();

// -----------------------------------------------------------------------------

class WIDGET : public testing::Test
{
protected:
    void SetUp() override
    {

    }

    void TearDown() override
    {
        twins::flushBuffer();
    }
};

// -----------------------------------------------------------------------------

TEST_F(WIDGET, drawWidget)
{
    twins::drawWidget(pWndTestWidgets, ID_PROGRESS);
    twins::drawWidget(pWndTestWidgets);
}

TEST_F(WIDGET, drawWidgets)
{
    twins::WID wids[] = { ID_CHECK, ID_PANEL };
    twins::drawWidgets(pWndTestWidgets, wids);
}
