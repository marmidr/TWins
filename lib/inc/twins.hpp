/******************************************************************************
 * @brief   TWins - main header file
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#pragma once
#include "twins_common.hpp"
#include "twins_esc_codes.hpp"
#include "twins_string.hpp"
#include "twins_ringbuffer.hpp"

#include <initializer_list>

// -----------------------------------------------------------------------------

#define TWINS_LOG(...)          twins::log(__FILE__, __FUNCTION__, __LINE__, "" __VA_ARGS__)

#ifndef __TWINS_LINK_SECRET
# define __TWINS_LINK_SECRET    void*_
#endif

#ifdef TWINS_THEMES
# include "twins_theme.hpp"
#else
# define TWINS_THEME_FG
# define TWINS_THEME_BG
#endif

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
    Inherit,
    Default,    // Reset to terminal default
    Black,
    BlackIntense,
    Red,
    RedIntense,
    Green,
    GreenIntense,
    Yellow,
    YellowIntense,
    Blue,
    BlueIntense,
    Magenta,
    MagentaIntense,
    Cyan,
    CyanIntense,
    White,
    WhiteIntense,
    // begin of theme-defined colors
    ThemeBegin,
    TWINS_THEME_FG
    ThemeEnd = 255
};

/** @brief Background colors */
enum class ColorBG : uint8_t
{
    Inherit,
    Default,    // Reset to terminal default
    Black,
    BlackIntense,
    Red,
    RedIntense,
    Green,
    GreenIntense,
    Yellow,
    YellowIntense,
    Blue,
    BlueIntense,
    Magenta,
    MagentaIntense,
    Cyan,
    CyanIntense,
    White,
    WhiteIntense,
    // begin of theme-defined colors
    ThemeBegin,
    TWINS_THEME_BG
    ThemeEnd = 255
};

/** @brief Convert color identifier to ASCII ESC code */
const char* encodeCl(ColorFG cl);
const char* encodeCl(ColorBG cl);
#ifdef TWINS_THEMES
// implemented in user code:
const char* encodeClTheme(ColorFG cl);
const char* encodeClTheme(ColorBG cl);
#endif

/** @brief Color intensification */
ColorFG intensifyCl(ColorFG cl);
ColorBG intensifyCl(ColorBG cl);
#ifdef TWINS_THEMES
// implemented in user code:
ColorFG intensifyClTheme(ColorFG cl);
ColorBG intensifyClTheme(ColorBG cl);
#endif

template<typename CL>
inline void intensifyClIf(bool cond, CL &cl) { if (cond) cl = intensifyCl(cl); }

/**
 * @brief Font attributes; some of them may be combined
 */
enum class FontAttrib : uint8_t
{
    None,           ///< normal style
    Bold,           ///< bold, excludes faint
    Faint,          ///< faint, excludes bold
    Italics,        ///<
    Underline,      ///< single underline
    Blink,          ///< blink
    Inverse,        ///< fg/bg reversed
    Invisible,      ///< text invisible
    StrikeThrough   ///<
};

/**
 * @brief Window, panel and page control frame styles
 */
enum class FrameStyle : uint8_t
{
    None,
    Single,
    Double,
    PgControl,
    ListBox
};

/**
 * @brief ProgressBar style
 */
enum class PgBarStyle : uint8_t
{
    Hash,       // #
    Shade,      //  ▒
    Rectangle,  // □
};

/**
 * @brief Button style
 */
enum class ButtonStyle : uint8_t
{
    Simple,
    Solid,
};

/**
 * @brief Unique Widget-ID
 */
using WID = uint16_t;

// moved to external file so it will not be taken into coverage
#include "twins_window_state.hpp"

/**
 * @brief Widget structure as union of members for different types;
 *        such construction can be const-defined and demands small memory
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
        Radio,
        Button,
        Led,
        PageCtrl,
        Page,
        ProgressBar,
        ListBox,
        ComboBox,
        CustomWgt,
        TextBox,
    };

    Type    type = {};
    WID     id = 0;
    Coord   coord;
    Size    size;

    /** In this union each type of Widget has it's own space */
    union
    {
        struct
        {
            const char *    title;
            ColorFG         fgColor;
            ColorBG         bgColor;
            bool            isPopup;
            IWindowState *  (*getState)();
        } window;

        struct
        {
            const char *title;
            ColorFG     fgColor;
            ColorBG     bgColor;
            bool        noFrame;
        } panel;

        struct
        {
            const char *text;
            ColorFG     fgColor;
            ColorBG     bgColor;
        } label;

        struct
        {
            ColorFG     fgColor;
            ColorBG     bgColor;
        } edit;

        struct
        {
            const char *text;
            ColorFG     fgColor;
        } checkbox;

        struct
        {
            const char *text;
            uint16_t    radioId;
            uint8_t     groupId;
            ColorFG     fgColor;
        } radio;

        struct
        {
            const char *text;
            ColorFG     fgColor;
            ColorBG     bgColor;
            ButtonStyle style;
        } button;

        struct
        {
            const char *text;
            ColorFG     fgColor;
            ColorBG     bgColorOff;
            ColorBG     bgColorOn;
        } led;

        struct
        {
            uint8_t     tabWidth;
        } pagectrl;

        struct
        {
            const char *title;
            ColorFG     fgColor;
        } page;

        struct
        {
            ColorFG     fgColor;
            PgBarStyle  style;
        } progressbar;

        struct
        {
            ColorFG     fgColor;
            ColorBG     bgColor;
            bool        noFrame;
        } listbox;

        struct
        {
            ColorFG     fgColor;
            ColorBG     bgColor;
            uint8_t     dropDownSize;
        } combobox;

        struct
        {
            ColorFG     fgColor;
            ColorBG     bgColor;
        } textbox;
    };

    /** parent <- this -> childs linking */
    union
    {
        __TWINS_LINK_SECRET;
        //
        struct
        {
            /** in constexpr the pointer cannot be calculated, thus,
              * we use flat Widgets array index instead */
            uint16_t ownIdx;     /// set in compile-time
            uint16_t parentIdx;  /// set in compile-time
            uint16_t childsIdx;  /// set in compile-time
            uint8_t  childsCnt;  /// set in compile-time
        };
    } link;
};

static constexpr WID WIDGET_ID_NONE = 0;    // convenient; default value points to nothing
static constexpr WID WIDGET_ID_ALL = -1;

/**
 * @brief Union of trivial-type widget properties;
 *      to be used like this:
 * @code.cpp
 *      twins::Map<twins::WID, twins::WidgetProp> wgtProp;
 * @endcode
 */
struct WidgetProp
{
    // apply to every widget
    bool enabled;

    //
    union
    {
        struct
        {
            bool checked;
        } chbx;

        struct
        {
            bool lit;
        } led;

        struct
        {
            int16_t itemIdx;
            int16_t selIdx;
        } lbx;

        struct
        {
            int16_t itemIdx;
            int16_t selIdx;
            bool    dropDown;
        } cbbx;
    };
};

/** @brief Object remembers terminal font colors and attribute,
 *         to restore them on destruction
 */
struct FontMementoManual
{
    void store();
    void restore();

protected:
    uint8_t szFg;
    uint8_t szBg;
    uint8_t szAttr;
};

/** @brief Helper for automatic restoring terminal font attributes */
struct FontMemento : FontMementoManual
{
    FontMemento()  { store(); }
    ~FontMemento() { restore(); }
};

/** @brief Mouse reporting modes */
enum class MouseMode : uint8_t
{
    Off,
    M1,
    M2
};

// -----------------------------------------------------------------------------

/**
 * @brief Initialize TWins
 */
void init(IPal *pal);

/**
 * @brief Control TWins mutex implemented in IPal
 *        Call unlock() only if lock() returned true
 * @note use twins::Locker instead of calling these functions directly
 */
bool lock(bool wait = true);
void unlock(void);

/** @brief used by TWINS_LOG() */
void log(const char *file, const char *func, unsigned line, const char *fmt, ...);
/** @brief */
void logRawBegin(const char *prologue = "", bool timeStamp = false);
void logRawWrite(const char *msg);
void logRawEnd(const char *epilogue = "");

/**
 * @brief Delay for given number if milliseconds
 */
void sleepMs(uint16_t ms);

/**
 * @brief Write char or string to the output
 */
int writeChar(char c, int16_t repeat = 1);
int writeStr(const char *s, int16_t repeat = 1);
int writeStrLen(const char *s, uint16_t sLen);
int writeStrFmt(const char *fmt, ...);
int writeStrVFmt(const char *fmt, va_list ap);
void flushBuffer(void);

/**
 * @brief Foreground color stack
 */
void pushClFg(ColorFG cl);
void popClFg(int n = 1);
void resetClFg(void);

/**
 * @brief Background color stack
 */
void pushClBg(ColorBG cl);
void popClBg(int n = 1);
void resetClBg(void);

/**
 * @brief Font attributes stack
 */
void pushAttr(FontAttrib attr);
void popAttr(int n = 1);
void resetAttr(void);

/**
 * @brief Cursor manipulation
 */
void moveTo(uint16_t col, uint16_t row);
void moveToCol(uint16_t col);
void moveBy(int16_t cols, int16_t rows);
inline void moveToHome(void)        { writeStr(ESC_CURSOR_HOME); }

inline void cursorSavePos(void)     { writeStr(ESC_CURSOR_POS_SAVE); }
inline void cursorRestorePos(void)  { writeStr(ESC_CURSOR_POS_RESTORE); }
inline void cursorHide(void)        { writeStr(ESC_CURSOR_HIDE); }
inline void cursorShow(void)        { writeStr(ESC_CURSOR_SHOW); }

/**
 * @brief Lines manipulation
 */
inline void insertLines(uint16_t count) { writeStrFmt(ESC_LINE_INSERT_FMT, count); }

/**
 * @brief Screen manipulation
 */
inline void screenClrAbove(void)    { writeStr(ESC_SCREEN_ERASE_ABOVE); }
inline void screenClrBelow(void)    { writeStr(ESC_SCREEN_ERASE_BELOW); }
inline void screenClrAll(void)      { writeStr(ESC_SCREEN_ERASE_ALL); }

inline void screenSave(void)        { writeStr(ESC_SCREEN_SAVE); }
inline void screenRestore(void)     { writeStr(ESC_SCREEN_RESTORE); }

/**
 * @brief Mouse reporting
 */
void mouseMode(MouseMode mode);

// -----------------------------------------------------------------------------

/**
 * @brief Decode ANSI keyboard/mouse sequence from \p input and produce readable Key Code \p output
 */
void decodeInputSeq(RingBuff<char> &input, KeyCode &output);

// -----------------------------------------------------------------------------

/**
 * @brief Draw single widget or entire window
 */
void drawWidget(const Widget *pWindowWidgets, WID widgetId = WIDGET_ID_ALL);

/**
 * @brief Draw selected widgets
 */
void drawWidgets(const Widget *pWindowWidgets, const WID *pWidgetIds, uint16_t count);

template<int N>
inline void drawWidgets(const Widget *pWindowWidgets, const WID (&widgetIds)[N])
{
    drawWidgets(pWindowWidgets, widgetIds, N);
}

inline void drawWidgets(const Widget *pWindowWidgets, const std::initializer_list<WID> &ids)
{
    drawWidgets(pWindowWidgets, ids.begin(), ids.size());
}

/**
 * @brief Return widget type as string
 */
const char * toString(Widget::Type type);

/**
 * @brief Return widget terminal screen based coordinates
 */
Coord getScreenCoord(const Widget *pWgt);

/**
 * @brief Return page id from \p pPageControl and \p pageIdx or \b WIDGET_ID_NONE
 */
WID getPageID(const Widget *pPageControl, int8_t pageIdx);

/**
 * @brief Return widget from its ID or \b nullptr
 */
const Widget* getWidget(const Widget *pWindowWidgets, WID widgetId);

/**
 * @brief Process keyboard/mouse signal received by console
 */
bool processKey(const Widget *pWindow, const KeyCode &kc);

/**
 * @brief As the PgUp/PgDn are often used by consoles, let the user decide
 *        when to change page
 */
void mainPgControlChangePage(const Widget *pWindowWidgets, bool next);

// -----------------------------------------------------------------------------

/** @brief RAII style locker */
struct Locker
{
    Locker(bool wait = true)
    {
        m_isLocked = twins::lock(wait);
    }

    ~Locker()
    {
        if (m_isLocked)
            twins::unlock();
    }

    bool isLocked() const { return m_isLocked; }

private:
    bool m_isLocked;
};

// -----------------------------------------------------------------------------

} // namespace
