/******************************************************************************
 * @brief   TWins - main header file
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#pragma once
#include "twins_common.hpp"
#include "twins_esc_codes.hpp"
#include "twins_string.hpp"

// -----------------------------------------------------------------------------

namespace twins
{

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
    NONE,       // means 'no change'
    DEFAULT,    // reset do terminal default
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
    NONE,       // means 'no change'
    DEFAULT,    // reset do terminal default
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
const char* encodeCl(ColorFG cl);

/** @brief Convert color identifier to ASCII ESC code */
const char* encodeCl(ColorBG cl);

/**
 * @brief
 */
enum class FontAttrib : uint8_t
{
    None,
    Bold,
    Faint,
    Italics,
    Underline,
    BlinkSlow,
    BlinkFast,
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
    virtual ~IWindowState() = default;
    virtual bool onDraw(const Widget*) = 0;
    virtual bool isEnabled(const Widget*) = 0;
    virtual bool isFocused(const Widget*) = 0;
    virtual bool isVisible(const Widget*) = 0;
    virtual bool getCheckboxChecked(const Widget*) = 0;
    virtual void getLabelText(const Widget*, String &out) = 0;
    virtual void getEditText(const Widget*, String &out) = 0;
    virtual bool getLedLit(const Widget*) = 0;
    virtual void getProgressBarNfo(const Widget*, int &pos, int &max) = 0;
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
        Edit,
        CheckBox,
        Button,
        Led,
        PageCtrl,
        Page,
        ProgressBar
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
            ColorBG         bgColor;
            ColorFG         fgColor;
            const char *    title;
            IWindowState *  (*getState)();
            const Widget *  pChildrens;
            uint16_t        childCount;
        } window;

        struct
        {
            ColorBG         bgColor;
            ColorFG         fgColor;
            const char *    title;
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
            ColorBG     bgColor;
            ColorFG     fgColor;
        } edit;

        struct
        {
            const char *text;
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
            const Widget *  pPages;
            uint16_t        pageCount;
        } pagectrl;

        struct
        {
            ColorBG         bgColor;
            ColorFG         fgColor;
            const char *    title;
            const Widget *  pChildrens;
            uint16_t        childCount;
        } page;

        struct
        {

        } progressbar;
    };

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

/**
 * @brief Draw all, single or set of widgets
 */
void drawWidget(const Widget *pWindow, uint16_t widgetId = WIDGET_ID_ALL);

void drawWidgets(const Widget *pWindow, uint16_t *pWidgetIds, uint16_t count);

template<int N>
void drawWidgets(const Widget *pWindow, uint16_t (&pWidgetIds)[N])
{
    drawWidgets(pWindow, pWidgetIds, N);
}

/**
 * @brief Foreground color stack
 */
void pushClrFg(ColorFG cl);
void popClrFg();
void resetClrFg();

/**
 * @brief Background color stack
 */
void pushClrBg(ColorBG cl);
void popClrBg();
void resetClrBg();

/**
 * @brief Font attributes stack
 */
void pushAttr(FontAttrib attr);
void popAttr();
void resetAttr();

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
void decodeInputSeq(KeySequence &input, KeyCode &output);

//void quit();

// -----------------------------------------------------------------------------

} // namespace
