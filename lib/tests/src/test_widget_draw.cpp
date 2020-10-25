/******************************************************************************
 * @brief   TWins - unit tests
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "twins_transform_window.hpp"
#include "twins.hpp"
#include "twins_utils.hpp"
#include "twins_window_mngr.hpp"
#include "../../lib/src/twins_widget_prv.hpp"

// -----------------------------------------------------------------------------

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
        ID_TEXTBOX_EMPTY,
        ID_COMBOBOX,
};

class WindowTestState : public twins::IWindowState
{
public:
    void init(const twins::Widget *pWindowWgts) override
    {
        mpWgts = pWindowWgts;
    }

    const twins::Widget *getWidgets() const override { return mpWgts; }

    twins::WID& getFocusedID() { return wgtId; };

    void getLabelText(const twins::Widget*, twins::String &out) override
    {
        out = "Label 1" "\n" "..but Line 2";
    }

    void getListBoxState(const twins::Widget*, int16_t &itemIdx, int16_t &selIdx, int16_t &itemsCount) override
    {
        itemIdx = 1;
        selIdx = 0;
        itemsCount = 3;
    }

    void getListBoxItem(const twins::Widget*, int itemIdx, twins::String &out) override
    {
        out.appendFmt("item: %d", itemIdx);
    }

    void getComboBoxState(const twins::Widget* pWgt, int16_t &itemIdx, int16_t &selIdx, int16_t &itemsCount, bool &dropDown) override
    {
        itemIdx = 1;
        selIdx = 0;
        itemsCount = 8;
        dropDown = true;
    }

    void getComboBoxItem(const twins::Widget* pWgt, int itemIdx, twins::String &out) override
    {
        out.appendFmt("item: %d", itemIdx);
    }

    void getTextBoxState(const twins::Widget* pWgt, const twins::Vector<twins::StringRange> **ppLines, int16_t &topLine) override
    {
        if (pWgt->id == ID_TEXTBOX_EMPTY)
        {
            wrapString.updateLines();
            *ppLines = &wrapString.getLines();
            topLine = 5;
            return;
        }

        wrapString = "Lorem ipsum \e[1m dolor \e[0m sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. "
                    "Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.";

        wrapString.config(15);
        *ppLines = &wrapString.getLines();
        // force scanning of invisible lines:
        topLine = 2;
    }

protected:
    const twins::Widget *mpWgts = nullptr;
    twins::WID wgtId = {};
    twins::util::WrappedString wrapString;
};


static WindowTestState wndTest;
twins::IWindowState * getWndTest();


static constexpr twins::Widget wndTestDef =
{
    type    : twins::Widget::Window,
    id      : ID_WND,
    coord   : { 5, 5 },
    size    : { 100, 50 },
    { window : {
        title       : "**Test**Window**",
        fgColor     : {},
        bgColor     : {},
        isPopup     : true, // draw shadow
        getState    : getWndTest,
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
                fgColor : {},
                groupId : 1,
                radioId : 1,
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
        {
            type    : twins::Widget::TextBox,
            id      : ID_TEXTBOX_EMPTY,
            coord   : { 80, 20 },
            size    : { 10, 10 },
            { textbox : {
                fgColor : {},
                bgColor : {},
            }}
        },
        {
            type    : twins::Widget::ComboBox,
            id      : ID_COMBOBOX,
            coord   : { 10, 10 },
            size    : { 10, 1 },
            { combobox : {
                fgColor : {},
                bgColor : {},
                dropDownSize : 5
            }}
        },
        { /* NUL */ }
    }}
};

constexpr auto wndTestWidgets = twins::transforWindowDefinition<&wndTestDef>();
const twins::Widget * pWndTestWidgets = wndTestWidgets.begin();

twins::IWindowState * getWndTest()
{
    wndTest.init(pWndTestWidgets);
    return &wndTest;
}

// -----------------------------------------------------------------------------

class WIDGETDRW : public testing::Test
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

TEST_F(WIDGETDRW, drawWidget)
{
    twins::drawWidget(pWndTestWidgets, ID_TEXTBOX);

    // draw pressed button
    twins::g_ws.pMouseDownWgt = twins::getWidget(pWndTestWidgets, ID_BTN1);
    twins::drawWidget(pWndTestWidgets, ID_BTN1);
    twins::g_ws.pMouseDownWgt = {};

    // draw all
    auto t = twins::pPAL->getTimeStamp();
    twins::drawWidget(pWndTestWidgets);
    twins::writeChar('\n', 3);

    t = twins::pPAL->getTimeDiff(t);
    TWINS_LOG("Drawn in %u ms", t);
}

TEST_F(WIDGETDRW, drawWidgets)
{
    twins::WID wids[] = { ID_CHECK, ID_PANEL };
    twins::drawWidgets(pWndTestWidgets, wids);
}

TEST_F(WIDGETDRW, wndManager)
{
    twins::WndManager wmngr;

    EXPECT_EQ(0, wmngr.size());
    EXPECT_EQ(nullptr, wmngr.topWndWidgets());
    wmngr.pushWnd(getWndTest());
    wmngr.redrawAll();
    EXPECT_EQ(1, wmngr.size());
    EXPECT_EQ(getWndTest(), wmngr.topWnd());
    wmngr.popWnd();
    EXPECT_EQ(0, wmngr.size());
}
