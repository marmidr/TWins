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

struct EditState
{
    const Widget *pWgt = nullptr;
    uint8_t cursorCol;
    String  str;
};

/** Global state object */
struct Glob
{
    Coord   parentCoord;            // current widget left-top position
    String  str;                    // common string buff for widget drawers
    const Widget *pWndArray = {};   // array of Window widgets
    IWindowState *pWndState = {};   //
    const Widget *pFocusedWgt = {}; //
    const Widget *pMouseDownWgt = {}; //
    EditState editState;            // state of Edit being modified
    int listboxHighlightIdx;        // focused Listbox index of highlighted row
};

extern Glob g;

struct WidgetSearchStruct
{
    WID   searchedID = {};      // given
    Coord parentCoord = {};     // expected
    bool  isVisible = true;     // expected
    const Widget *pWidget = {}; // expected
};

// -----------------------------------------------------------------------------

// require g.pWndArray set
bool findWidget(WidgetSearchStruct &wss);
const Widget* findWidget(const WID widgetId);
const Widget* getWidgetAt(uint8_t col, uint8_t row, Rect &wgtRect);
void setCursorAt(const Widget *pWgt);

// do not require g.pWndArray
const Widget* getParent(const Widget *pWgt);

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
