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
#define __TWINS_LINK_SECRET void*_
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
    Default,    // Reset Do Terminal Default
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
    WhiteIntense
};


/** @brief Background colors */
enum class ColorBG : uint8_t
{
    None,       // Means 'No Change'
    Default,    // Reset Do Terminal Default
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
    WhiteIntense
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
    None,           ///< normal style
    Bold,           ///< excludes faint
    Faint,          ///< excludes bold
    Italics,        ///<
    Underline,      ///<
    BlinkSlow,      ///<
    Inverse,        ///<
    Invisible,      ///<
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
 * @brief Unique Widget-ID
 */
using WID = int16_t;

/** @brief Forward declaration */
struct Widget;

/** @brief Each window uses these callbacks while drawing */
class IWindowState
{
public:
    virtual ~IWindowState() = default;
    // events
    virtual void onButtonClick(const twins::Widget* pWgt) = 0;
    virtual void onEditChange(const twins::Widget* pWgt, twins::String &str) = 0;
    virtual void onCheckboxToggle(const twins::Widget* pWgt) = 0;
    virtual void onPageControlPageChange(const twins::Widget* pWgt, uint8_t newPageIdx) = 0;
    // state queries
    virtual bool isEnabled(const Widget*) = 0;
    virtual bool isFocused(const Widget*) = 0;
    virtual bool isVisible(const Widget*) = 0;
    virtual WID& getFocusedID() = 0;
    virtual bool getCheckboxChecked(const Widget*) = 0;
    virtual void getLabelText(const Widget*, String &out) = 0;
    virtual void getEditText(const Widget*, String &out) = 0;
    virtual bool getLedLit(const Widget*) = 0;
    virtual void getProgressBarNfo(const Widget*, int &pos, int &max) = 0;
    virtual int  getPageCtrlPageIndex(const Widget*) = 0;
    // requests
    virtual void invalidate(twins::WID id) = 0;
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
        Button,
        Led,
        PageCtrl,
        Page,
        ProgressBar,
        List,
        DropDownList,
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
            ColorFG     fgColor;
        } checkbox;

        struct
        {
            const char *text;
            uint8_t     groupId;
            ColorFG     fgColor;
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
        } progressbar;

        struct
        {

        } list;

        struct
        {

        } dropdownlist;
    };

};

static constexpr WID WIDGET_ID_NONE = 0;    // convenient, any id by default points to nothing
static constexpr WID WIDGET_ID_ALL = -1;

/** @brief Object remembers terminal font colors and attribute,
 *         to restore them upon destruction
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
void init(const IOs *ios);

/**
 *
 */
void log(const char *file, const char *func, unsigned line, const char *fmt, ...);

/**
 * @brief Write char or string to the output
 */
int writeChar(char c, int16_t count = 1);
int writeStr(const char *s);
int writeStr(const char *s, int16_t count);
int writeStrFmt(const char *fmt, ...);

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

/**
 * @brief Decode given ANSI sequence and produce readable Key Code
 */
void decodeInputSeq(RingBuff<char> &input, KeyCode &output);

/**
 * @brief Process keyboard signal received by console
 */
void processKey(const Widget *pWindow, const KeyCode &kc);

//void quit();

// -----------------------------------------------------------------------------

} // namespace
