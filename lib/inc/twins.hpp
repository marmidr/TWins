/******************************************************************************
 * @brief   TWins - main header file
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#pragma once
#include "twins_esc_codes.hpp"
#include "twins_string.hpp"
#include <stdint.h>
#include <stdarg.h>

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
class IWindowState
{
public:
    virtual bool onDraw(const Widget*) = 0;
    virtual bool isEnabled(const Widget*) = 0;
    virtual bool isFocused(const Widget*) = 0;
    virtual bool isVisible(const Widget*) = 0;
    virtual bool getCheckboxChecked(const Widget*) = 0;
    virtual void getLabelText(const Widget*, String &out) = 0;
    virtual bool getLedLit(const Widget*) = 0;
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
        Led,
        PageCtrl
    };

    Type        type = {};
    uint16_t    id = 0;
    Coord       coord;
    Size        size;

    /** */
    union
    {
        struct
        {
            FrameStyle      frameStyle;
            ColorBG         bgColor;
            ColorFG         fgColor;
            const char *    caption;
            IWindowState *  (*getState)();
            const Widget *  pChildrens;
            uint16_t        childCount;
        } window;

        struct
        {
            FrameStyle      frameStyle;
            ColorBG         bgColor;
            ColorFG         fgColor;
            const char *    caption;
            const Widget *  pChildrens;
            uint16_t        childCount;
        } panel;

        struct
        {
            ColorBG     bgColor;
            ColorFG     fgColor;
            const char *text;
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
            ColorBG     bgColorOff;
            ColorBG     bgColorOn;
            ColorFG     fgColor;
            const char *text;
        } led;

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
    int     (*writeStr)(const char *s);
    int     (*writeStrFmt)(const char *fmt, va_list ap);
    void *  (*malloc)(uint32_t sz);
    void    (*mfree)(void *ptr);
};

static constexpr uint16_t WIDGET_ID_ALL = (-1);

// -----------------------------------------------------------------------------

/**
 * @brief
 */
void init(const IOs *ios);

/**
 * @brief
 */
int writeChar(char c, int16_t count);
int writeStr(const char *s);
int writeStrFmt(const char *fmt, ...);
void drawWidget(const Widget *pWindow, uint16_t widgetId = WIDGET_ID_ALL);

/**
 * @brief Cursor manipulation
 */
void moveTo(uint16_t col, uint16_t row);
void moveToCol(uint16_t col);
void moveBy(int16_t cols, int16_t rows);
inline void moveToHome()           { writeStr(ESC_CURSOR_HOME); }

inline void cursorSavePos()        { writeStr(ESC_CURSOR_POS_SAVE); }
inline void cursorRestorePos()     { writeStr(ESC_CURSOR_POS_RESTORE); }
inline void cursorHide()           { writeStr(ESC_CURSOR_HIDE); }
inline void cursorShow()           { writeStr(ESC_CURSOR_SHOW); }

/**
 * @brief Screen manipulation
 */
inline void clrScreenAbove()       { writeStr(ESC_SCREEN_ERASE_ABOVE); }
inline void clrScreenBelow()       { writeStr(ESC_SCREEN_ERASE_BELOW); }
inline void clrScreenAll()         { writeStr(ESC_SCREEN_ERASE_ALL); }

inline void clrScreenSave()        { writeStr(ESC_SCREEN_SAVE); }
inline void clrScreenRestore()     { writeStr(ESC_SCREEN_RESTORE); }

/**
 * @brief
 */
 void quit();

// -----------------------------------------------------------------------------

} // namespace
