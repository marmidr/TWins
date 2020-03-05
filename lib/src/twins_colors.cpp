/******************************************************************************
 * @brief   TWins - colors
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#include "twins.hpp"

// -----------------------------------------------------------------------------

namespace twins
{

const char*  CL_FG_DEFAULT = ESC_FG_DEFAULT;
const char*  CL_FG_BLACK = ESC_FG_BLACK;
const char*  CL_FG_BLACK_INTENSE = ESC_FG_BLACK_INTENSE;
const char*  CL_FG_RED = ESC_FG_RED;
const char*  CL_FG_RED_INTENSE = ESC_FG_RED_INTENSE;
const char*  CL_FG_GREEN = ESC_FG_GREEN;
const char*  CL_FG_GREEN_INTENSE = ESC_FG_GREEN_INTENSE;
const char*  CL_FG_YELLOW = ESC_FG_YELLOW;
const char*  CL_FG_YELLOW_INTENSE = ESC_FG_YELLOW_INTENSE;
const char*  CL_FG_BLUE = ESC_FG_BLUE;
const char*  CL_FG_BLUE_INTENSE = ESC_FG_BLUE_INTENSE;
const char*  CL_FG_MAGENTA = ESC_FG_MAGENTA;
const char*  CL_FG_MAGENTA_INTENSE = ESC_FG_MAGENTA_INTENSE;
const char*  CL_FG_CYAN = ESC_FG_CYAN;
const char*  CL_FG_CYAN_INTENSE = ESC_FG_CYAN_INTENSE;
const char*  CL_FG_WHITE = ESC_FG_WHITE;
const char*  CL_FG_WHITE_INTENSE = ESC_FG_WHITE_INTENSE;


const char*  CL_BG_DEFAULT = ESC_BG_DEFAULT;
const char*  CL_BG_BLACK = ESC_BG_BLACK;
const char*  CL_BG_BLACK_INTENSE = ESC_BG_BLACK_INTENSE;
const char*  CL_BG_RED = ESC_BG_RED;
const char*  CL_BG_RED_INTENSE = ESC_BG_RED_INTENSE;
const char*  CL_BG_GREEN = ESC_BG_GREEN;
const char*  CL_BG_GREEN_INTENSE = ESC_BG_GREEN_INTENSE;
const char*  CL_BG_YELLOW = ESC_BG_YELLOW;
const char*  CL_BG_YELLOW_INTENSE = ESC_BG_YELLOW_INTENSE;
const char*  CL_BG_BLUE = ESC_BG_BLUE;
const char*  CL_BG_BLUE_INTENSE = ESC_BG_BLUE_INTENSE;
const char*  CL_BG_MAGENTA = ESC_BG_MAGENTA;
const char*  CL_BG_MAGENTA_INTENSE = ESC_BG_MAGENTA_INTENSE;
const char*  CL_BG_CYAN = ESC_BG_CYAN;
const char*  CL_BG_CYAN_INTENSE = ESC_BG_CYAN_INTENSE;
const char*  CL_BG_WHITE = ESC_BG_WHITE;
const char*  CL_BG_WHITE_INTENSE = ESC_BG_WHITE_INTENSE;


const char * clMapFg[] =
{
    CL_FG_DEFAULT,
    CL_FG_BLACK,
    CL_FG_BLACK_INTENSE,
    CL_FG_RED,
    CL_FG_RED_INTENSE,
    CL_FG_GREEN,
    CL_FG_GREEN_INTENSE,
    CL_FG_YELLOW,
    CL_FG_YELLOW_INTENSE,
    CL_FG_BLUE,
    CL_FG_BLUE_INTENSE,
    CL_FG_MAGENTA,
    CL_FG_MAGENTA_INTENSE,
    CL_FG_CYAN,
    CL_FG_CYAN_INTENSE,
    CL_FG_WHITE,
    CL_FG_WHITE_INTENSE
};

const char * clMapBg[] =
{
    CL_BG_DEFAULT,
    CL_BG_BLACK,
    CL_BG_BLACK_INTENSE,
    CL_BG_RED,
    CL_BG_RED_INTENSE,
    CL_BG_GREEN,
    CL_BG_GREEN_INTENSE,
    CL_BG_YELLOW,
    CL_BG_YELLOW_INTENSE,
    CL_BG_BLUE,
    CL_BG_BLUE_INTENSE,
    CL_BG_MAGENTA,
    CL_BG_MAGENTA_INTENSE,
    CL_BG_CYAN,
    CL_BG_CYAN_INTENSE,
    CL_BG_WHITE,
    CL_BG_WHITE_INTENSE
};

const char* clrFg(ColorFG clr)
{
    if ((int)clr < arrSize(clMapFg))
        return clMapFg[(int)clr];
    return "";
}

const char* clrBg(ColorBG clr)
{
    if ((int)clr < arrSize(clMapBg))
        return clMapBg[(int)clr];
    return "";
}

}