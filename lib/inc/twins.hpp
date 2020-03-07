/******************************************************************************
 * @brief   TWins - main header file
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#pragma once
#include "twins_esc_codes.hpp"
#include <stdint.h>
#include <stdio.h>

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

struct Theme
{
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
        Button,
        PageCtrl
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
            uint16_t                childrensCount;
        } window;

        struct
        {
            FrameStyle      frameStyle;
            ColorBG         bgColor;
            const char *    caption;
            const Widget *  pChildrens;
            uint16_t        childrensCount;
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

        struct
        {

        } pagectrl;
    };

};

enum class Key : uint8_t
{
    None,
    Esc,
    Return,
    Tab,
    Up,
    Down,
    Left,
    Right,
    Insert,
    Delete,
    Home,
    End,
    PgUp,
    PgDown,
};

enum class Mod : uint8_t
{
    None,
    Ctrl = 1,
    Alt = 2,
    Shift = 4,
};

union KeyCode
{
    uint16_t code;
    struct
    {
        Key k;
        Mod m;
    };
};

struct IOs
{
    void (*writeStr)(const char *s);
    void (*writeStrFmt)(const char *fmt, va_list ap);
    void (*onKey)(Widget *pActWidget, Key k, Mod m);
    void *(*malloc)(uint32_t sz);
    void (*mfree)(void *ptr);
};

// -----------------------------------------------------------------------------

/**
 * @brief
 */
void init(IOs *ios);

/**
 * @brief
 */
void writeStr(const char *s);
void writeStrFmt(const char *fmt, ...);
void drawWidget(const Widget *pWdgt);

/**
 * @brief Cursor manipulation
 */
void moveTo(uint16_t col, uint16_t row);
void moveToCol(uint16_t col);
inline void moveToHome()           { writeStr(ESC_CURSOR_HOME); }
void moveBy(int16_t cols, int16_t rows);

inline void cursorSavePos()        { writeStr(ESC_CURSOR_POS_SAVE); }
inline void cursorRestorePos()     { writeStr(ESC_CURSOR_POS_RESTORE); }
inline void cursorHide()           { writeStr(ESC_CURSOR_HIDE); }
inline void cursorShow()           { writeStr(ESC_CURSOR_SHOW); }

/**
 * @brief Screen manipulation
 */
inline void clrScreenAbove()       { writeStr(ESC_ERASE_DISPLAY_ABOVE); }
inline void clrScreenBelow()       { writeStr(ESC_ERASE_DISPLAY_BELOW); }
inline void clrScreenAll()         { writeStr(ESC_ERASE_DISPLAY_ALL); }

inline void clrScreenSave()        { writeStr(ESC_SCREEN_SAVE); }
inline void clrScreenRestore()     { writeStr(ESC_SCREEN_RESTORE); }

/**
 * @brief
 */
 void quit();

// -----------------------------------------------------------------------------

} // namespace
