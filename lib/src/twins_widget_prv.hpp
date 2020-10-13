/******************************************************************************
 * @brief   TWins - main header file
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#pragma once
#include "twins.hpp"
#include "twins_string.hpp"
#include "twins_utf8str.hpp"

// -----------------------------------------------------------------------------

namespace twins
{

struct Rect
{
    Coord coord;
    Size  size;

    void setMax()
    {
        coord.col = 1;
        coord.row = 1;
        size.width = 0xff;
        size.height = 0xff;
    }
};

struct WidgetSearchStruct
{
    WID   searchedID = {};      // given
    Coord parentCoord = {};     // expected
    bool  isVisible = true;     // expected
    const Widget *pWidget = {}; // expected
};

/** Widget drawing state object */
struct WidgetState
{
    Coord   parentCoord;                // current widget's parent left-top position
    String  str;                        // common string buff for widget drawers
    const Widget *pWndWidgets = {};     // array of Window widgets
    IWindowState *pWndState = {};       //
    const Widget *pFocusedWgt = {};     //
    const Widget *pMouseDownWgt = {};   //
    const Widget *pDropDownCombo = {};
    struct                              // state of Edit being modified
    {
        const Widget *pWgt = nullptr;
        int16_t cursorPos = 0;
        String  str;
    } editState;
};

extern WidgetState& g_ws;

// -----------------------------------------------------------------------------

// require g_ws.pWindowWidgets set
const Widget* getWidgetByWID(const WID widgetId);
const Widget* getWidgetAt(uint8_t col, uint8_t row, Rect &wgtRect);
// does not require g_ws.pWindowWidgets
const Widget* getParent(const Widget *pWgt);

bool getWidgetWSS(WidgetSearchStruct &wss);
void setCursorAt(const Widget *pWgt);

// -----------------------------------------------------------------------------

inline void operator += (Coord &cord, const Coord &offs)
{
    cord.col += offs.col;
    cord.row += offs.row;
}

inline void operator -= (Coord &cord, const Coord &offs)
{
    cord.col -= offs.col;
    cord.row -= offs.row;
}

inline Coord operator + (const Coord &cord1, const Coord &cord2)
{
    Coord ret = {
        uint8_t(cord1.col + cord2.col),
        uint8_t(cord1.row + cord2.row)
    };
    return ret;
}

inline Coord operator + (const Coord &cord, const Size offs)
{
    Coord ret = {
        uint8_t(cord.col + offs.width),
        uint8_t(cord.row + offs.height)
    };
    return ret;
}

// inline Size operator + (const Size &sz1, const Size &sz2)
// {
//     Size ret = {
//         uint8_t(sz1.width + sz2.width),
//         uint8_t(sz1.height + sz2.height)
//     };
//     return ret;
// }

inline Size operator - (const Size &sz1, const Size &sz2)
{
    Size ret = {
        uint8_t(sz1.width - sz2.width),
        uint8_t(sz1.height - sz2.height)
    };
    return ret;
}

// -----------------------------------------------------------------------------

} // namespace
