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

#define TWINS_LOG(...)   twins::log(__FILE__, __FUNCTION__, __LINE__, "" __VA_ARGS__)

#ifndef __TWINS_LINK_SECRET
# define __TWINS_LINK_SECRET void*_
#endif

#ifndef THEME_FG_DEFS
# define THEME_FG_DEFS
#endif

#ifndef THEME_BG_DEFS
# define THEME_BG_DEFS
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
    None,       // Means 'No Change'
    Default,    // Reset to Terminal Default
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
    THEME_FG_DEFS
    ThemeEnd = 255
};


/** @brief Background colors */
enum class ColorBG : uint8_t
{
    None,       // Means 'No Change'
    Default,    // Reset to Terminal Default
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
    THEME_BG_DEFS
    ThemeEnd = 255
};

/** @brief Convert color identifier to ASCII ESC code */
const char* encodeCl(ColorFG cl);

/** @brief Convert color identifier to ASCII ESC code */
const char* encodeCl(ColorBG cl);

/** @brief Color increment operator */
ColorFG operator+(ColorFG cl, uint8_t n);
ColorBG operator+(ColorBG cl, uint8_t n);
inline ColorFG operator++(ColorFG &cl) { cl = cl + 1; return cl; };
inline ColorBG operator++(ColorBG &cl) { cl = cl + 1; return cl; };

/**
 * @brief
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
};

/**
 * @brief ProgressBar style
 */
enum class PgBarStyle : uint8_t
{
    Hash,
    Shade,
    Rectangle,
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
using WID = int16_t;

/** @brief Forward declaration */
struct Widget;

/** @brief Window state and event handler */
class IWindowState
{
public:
    virtual ~IWindowState() = default;
    // events
    virtual void onButtonDown(const twins::Widget* pWgt) {}
    virtual void onButtonUp(const twins::Widget* pWgt) {}
    virtual void onEditChange(const twins::Widget* pWgt, twins::String &&str) {}
    virtual void onCheckboxToggle(const twins::Widget* pWgt) {}
    virtual void onPageControlPageChange(const twins::Widget* pWgt, uint8_t newPageIdx) {}
    virtual void onListBoxSelect(const twins::Widget* pWgt, uint16_t highlightIdx) {}
    virtual void onListBoxChange(const twins::Widget* pWgt, uint16_t newIdx) {}
    virtual void onRadioSelect(const twins::Widget* pWgt) {}
    virtual void onCanvasDraw(const twins::Widget* pWgt) {}
    virtual void onCanvasMouseEvt(const twins::Widget* pWgt, const twins::KeyCode &kc) {}
    // common state queries
    virtual bool isEnabled(const twins::Widget*) { return true; }
    virtual bool isFocused(const twins::Widget*) { return false; }
    virtual bool isVisible(const twins::Widget*) { return true; }
    virtual WID& getFocusedID() = 0;
    // widget-specific queries
    virtual bool getCheckboxChecked(const twins::Widget*) { return false; }
    virtual void getLabelText(const twins::Widget*, twins::String &out) {}
    virtual void getEditText(const twins::Widget*, twins::String &out) {}
    virtual bool getLedLit(const twins::Widget*) { return false; }
    virtual void getLedText(const twins::Widget*, twins::String &out) {}
    virtual void getProgressBarState(const twins::Widget*, int &pos, int &max) {}
    virtual int  getPageCtrlPageIndex(const twins::Widget*) { return 0; }
    virtual void getListBoxState(const twins::Widget*, int &itemIdx, int &itemsCount) { itemIdx = 0; itemsCount = 0; }
    virtual void getListBoxItem(const twins::Widget*, int itemIdx, twins::String &out) {}
    virtual int  getRadioIndex(const twins::Widget*) { return -1; }
    // requests
    virtual void invalidate(twins::WID id) {}
};

struct Theme
{
};

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
        DropDownList,
        Canvas,
    };

    Type    type = {};
    WID     id = 0;
    Coord   coord;
    Size    size;

    /** parent <- this -> childs linking */
    union
    {
        __TWINS_LINK_SECRET;
        //
        struct
        {
            /** in constexpr the pointer cannot be calculated, thus,
              * we use flat Widgets array index instead */
            uint8_t ownIdx;     /// set in compile-time
            uint8_t parentIdx;  /// set in compile-time
            uint8_t childsIdx;  /// set in compile-time
            uint8_t childsCnt;  /// set in compile-time
        };
    } link;

    /** In this union each type of Widget has it's own space */
    union
    {
        struct
        {
            ColorBG         bgColor;
            ColorFG         fgColor;
            const char *    title;
            IWindowState *  (*getState)();
        } window;

        struct
        {
            ColorBG     bgColor;
            ColorFG     fgColor;
            const char *title;
        } panel;

        struct
        {
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
            ColorFG     fgColor;
        } checkbox;

        struct
        {
            const char *text;
            uint16_t    radioId;
            uint8_t     groupId;
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
            ColorBG     bgColorOff;
            ColorBG     bgColorOn;
            ColorFG     fgColor;
            const char *text;
        } led;

        struct
        {
            uint8_t     tabWidth;
        } pagectrl;

        struct
        {
            ColorFG     fgColor;
            const char *title;
        } page;

        struct
        {
            ColorFG     fgColor;
            PgBarStyle  style;
        } progressbar;

        struct
        {
        } listbox;

        struct
        {
        } dropdownlist;

        struct
        {
        } canvas;
    };

};

static constexpr WID WIDGET_ID_NONE = 0;    // convenient; default value points to nothing
static constexpr WID WIDGET_ID_ALL = -1;

/** @brief Object remembers terminal font colors and attribute,
 *         to restore them on destruction
 */
struct FontMemento
{
    FontMemento();
    ~FontMemento();

private:
    uint8_t szFg;
    uint8_t szBg;
    uint8_t szAttr;
};

// -----------------------------------------------------------------------------

/**
 * @brief Initialize TWins
 */
void init(IPal *pal);

/** @brief used by TWINS_LOG() */
void log(const char *file, const char *func, unsigned line, const char *fmt, ...);

/** @brief Control wheather all succesive write are stored in a buffer and then written at once, asynchronously */
void bufferBegin();
void bufferEnd();

/**
 * @brief Write char or string to the output
 */
int writeChar(char c, int16_t count = 1);
int writeStr(const char *s);
int writeStr(const char *s, int16_t count);
int writeStrFmt(const char *fmt, ...);

/**
 * @brief Foreground color stack
 */
void pushClFg(ColorFG cl);
void popClFg(int n = 1);
void resetClFg();

/**
 * @brief Background color stack
 */
void pushClBg(ColorBG cl);
void popClBg(int n = 1);
void resetClBg();

/**
 * @brief Font attributes stack
 */
void pushAttr(FontAttrib attr);
void popAttr(int n = 1);
void resetAttr();

/**
 * @brief Cursor manipulation
 */
void moveTo(uint16_t col, uint16_t row);
void moveToCol(uint16_t col);
void moveBy(int16_t cols, int16_t rows);
inline void moveToHome()            { writeStr(ESC_CURSOR_HOME); }

inline void cursorSavePos()         { writeStr(ESC_CURSOR_POS_SAVE); }
inline void cursorRestorePos()      { writeStr(ESC_CURSOR_POS_RESTORE); }
inline void cursorHide()            { writeStr(ESC_CURSOR_HIDE); }
inline void cursorShow()            { writeStr(ESC_CURSOR_SHOW); }

/**
 * @brief Lines manipulation
 */
inline void insertLines(uint16_t count) { writeStrFmt(ESC_LINE_INSERT_FMT, count); }

/**
 * @brief Screen manipulation
 */
inline void screenClrAbove()        { writeStr(ESC_SCREEN_ERASE_ABOVE); }
inline void screenClrBelow()        { writeStr(ESC_SCREEN_ERASE_BELOW); }
inline void screenClrAll()          { writeStr(ESC_SCREEN_ERASE_ALL); }

inline void screenSave()            { writeStr(ESC_SCREEN_SAVE); }
inline void screenRestore()         { writeStr(ESC_SCREEN_RESTORE); }

// -----------------------------------------------------------------------------

/**
 * @brief Decode ANSI keyboard/mouse sequence from \p input and produce readable Key Code \p output
 */
void decodeInputSeq(RingBuff<char> &input, KeyCode &output);

// -----------------------------------------------------------------------------

/**
 * @brief Draw single widget or entire window
 */
void drawWidget(const Widget *pWindowArray, WID widgetId = WIDGET_ID_ALL);

/**
 * @brief Draw selected widgets
 */
void drawWidgets(const Widget *pWindowArray, const WID *pWidgetIds, uint16_t count);

template<int N>
inline void drawWidgets(const Widget *pWindowArray, const WID (&widgetIds)[N])
{
    drawWidgets(pWindowArray, widgetIds, N);
}

inline void drawWidgets(const Widget *pWindowArray, const std::initializer_list<WID> &ids)
{
    drawWidgets(pWindowArray, ids.begin(), ids.size());
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
const Widget* getWidget(const Widget *pWindowArray, WID widgetId);

/**
 * @brief Process keyboard/mouse signal received by console
 */
bool processKey(const Widget *pWindow, const KeyCode &kc);


// -----------------------------------------------------------------------------

} // namespace
