/******************************************************************************
 * @brief   TWins - main header file
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#pragma once
#include "twins_esc_codes.hpp"
#include <stdint.h>

// -----------------------------------------------------------------------------

namespace twins
{

/** @brief Template returning length of array of type T */
template<unsigned N, typename T>
unsigned arrSize(const T (&arr)[N]) { return N; }


/** @brief Screen coordinates */
struct Coord
{
    uint8_t col;
    uint8_t row;
};

/** @brief Widget size */
struct Size
{
    uint8_t width;
    uint8_t height;
};

/** @brief Foreground colors */
enum class ColorFG : uint8_t
{
    DEFAULT,
    BLACK,
    BLACK_INTENSE,
    RED,
    RED_INTENSE,
    GREEN,
    GREEN_INTENSE,
    YELLOW,
    YELLOW_INTENSE,
    BLUE,
    BLUE_INTENSE,
    MAGENTA,
    MAGENTA_INTENSE,
    CYAN,
    CYAN_INTENSE,
    WHITE,
    WHITE_INTENSE
};


/** @brief Background colors */
enum class ColorBG : uint8_t
{
    DEFAULT,
    BLACK,
    BLACK_INTENSE,
    RED,
    RED_INTENSE,
    GREEN,
    GREEN_INTENSE,
    YELLOW,
    YELLOW_INTENSE,
    BLUE,
    BLUE_INTENSE,
    MAGENTA,
    MAGENTA_INTENSE,
    CYAN,
    CYAN_INTENSE,
    WHITE,
    WHITE_INTENSE
};

/** @brief Convert color identifier to ASCII ESC code */
const char* clrFg(ColorFG clr);

/** @brief Convert color identifier to ASCII ESC code */
const char* clrBg(ColorBG clr);

/**
 * @brief
 */
enum class FontAttrib : uint8_t
{
    Normal,
    Bold,
    Faint,
    Italics,
    Underline,
    Inverse,
    Invisible,
    StrikeThrough
};

/**
 * @brief
 */
enum class FrameStyle : uint8_t
{
    None,
    Single,
    Double
};

/** @brief Forward declaration */
struct Widget;

/** @brief Each window uses these callbacks while drawing */
struct WindowCallbacks
{
    /** function called each time the widget is beeing draw;
     * @param pWgt - pointer to widget beeing draw
     * @return true if user handled draw, false otherwise
     */
    using OnDraw    = bool (*)(const Widget*);
    using IsEnabled = bool (*)(const Widget*);
    using IsFocused = bool (*)(const Widget*);
    using IsVisible = bool (*)(const Widget*);
    using IsCheckboxChecked = bool (*)(const Widget*);

    OnDraw    onDraw;
    IsEnabled isEnabled;
    IsFocused isFocused;
    IsVisible isVisible;
    IsCheckboxChecked isCheckboxChecked;
};

/**
 * @brief
 */
struct Widget
{
    enum Type
    {
        None,
        Window,
        Panel,
        Label,
        CheckBox,
        Button
    };

    Type        type = {};
    uint8_t     id = 0;
    Coord       coord;
    Size        size;

    /** */
    union
    {
        struct
        {
            FrameStyle              frameStyle;
            ColorBG                 bgColor;
            const char *            caption;
            const WindowCallbacks * pCallbacks;
            const Widget *          pChildrens;
        } window;

        struct
        {
            FrameStyle      frameStyle;
            ColorBG         bgColor;
            const char *    caption;
            const Widget *  pChildrens;
        } panel;

        struct
        {
            ColorBG     bgColor;
            ColorFG     fgColor;
            const char *caption;
        } label;

        struct
        {
            const char *text;
            bool        checked;
        } checkbox;

        struct
        {
            const char *text;
            uint8_t     groupId;
        } button;
    };

};

// -----------------------------------------------------------------------------

/**
 * @brief
 */
uint32_t writeStr(const char *fmt, ...);

/**
 * @brief
 */
void moveTo(uint16_t col, uint16_t row);
void moveToHome();
void moveBy(int16_t cols, int16_t rows);

/**
 * @brief
 */
void cursorSavePos();
void cursorRestorePos();

/**
 * @brief
 */
void clrScreenAbove();
void clrScreenBelow();
void clrScreenAll();

// -----------------------------------------------------------------------------

} // namespace
