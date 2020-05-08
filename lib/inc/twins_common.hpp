/******************************************************************************
 * @brief   TWins - common definitions
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#pragma once
#include <stdint.h>
#include <stdarg.h>

// -----------------------------------------------------------------------------

namespace twins
{

// forward decl
class String;

/**
 * @brief Template returning length of array of type T
 */
template<unsigned N, typename T>
unsigned arrSize(const T (&arr)[N]) { return N; }

/**
 * @brief array template usefull in const expressions
 */
template <typename T, unsigned N>
struct Array
{
    constexpr       T& operator[](unsigned i)       { return data[i]; }
    constexpr const T& operator[](unsigned i) const { return data[i]; }
    constexpr const T* begin()                const { return data; }
    constexpr const T* end()                  const { return data + N; }
    constexpr unsigned size()                 const { return N; }

    T data[N] = {};
};

/**
 * @brief Interface for I/O layer for easy porting
 */
struct IOs
{
    struct Stats
    {
        uint16_t memChunks;
        uint16_t memChunksMax;
        int32_t  memAllocated;
        int32_t  memAllocatedMax;
    };

    virtual ~IOs() = default;
    virtual int   writeStr(const char *s) = 0;
    virtual int   writeStrAsync(twins::String &&str) = 0;
    virtual int   writeStrFmt(const char *fmt, va_list ap) = 0;
    virtual void  flushBuff() = 0;
    virtual void *memAlloc(uint32_t sz) = 0;
    virtual void  memFree(void *ptr) = 0;
    virtual uint16_t getLogsRow() = 0;
    virtual void sleep(uint16_t ms) = 0;
};

// pointer set by init()
extern IOs *pIOs;



/**
 * @brief ANSI codes
 */
enum class Ansi : uint8_t
{
    NUL = 0x00,  // Null
    SOH = 0x01,  // Start of Header
    STX = 0x02,  // Start of Text
    ETX = 0x03,  // End of Text
    EOT = 0x04,  // End of Transmission
    ENQ = 0x05,  // Enquiry
    ACK = 0x06,  // Acknowledgment
    BEL = 0x07,  // Bell
    BS  = 0x08,  // Backspace
    HT  = 0x09,  // Horizontal Tab
    LF  = 0x0A,  // Line Feed
    VT  = 0x0B,  // Vertical Tab
    FF  = 0x0C,  // Form Feed
    CR  = 0x0D,  // Carriage Return
    SO  = 0x0E,  // Shift Out
    SI  = 0x0F,  // Shift In
    DLE = 0x10,  // Data Link Escape
    DC1 = 0x11,  // XONDevice Control 1
    DC2 = 0x12,  // Device Control 2
    DC3 = 0x13,  // XOFFDevice Control 3
    DC4 = 0x14,  // Device Control 4
    NAK = 0x15,  // Negative Ack.
    SYN = 0x16,  // Synchronous Idle
    ETB = 0x17,  // End of Trans. Block
    CAN = 0x18,  // Cancel
    EM  = 0x19,  // End of Medium
    SUB = 0x1A,  // Substitute
    ESC = 0x1B,  // Escape
    FS  = 0x1C,  // File Separator
    GS  = 0x1D,  // Group Separator
    RS  = 0x1E,  // Record Separator
    US  = 0x1F,  // Unit Separator
    DEL = 0x7F   // Delete
};


/** Special keys */
enum class Key : uint8_t
{
    None,
    Esc,
    Tab,
    Enter,
    Backspace,
    //
    Up,
    Down,
    Left,
    Right,
    //
    Insert,
    Delete,
    Home,
    End,
    PgUp,
    PgDown,
    //
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,
    //
    MouseEvent
};

/** Mouse button click events */
enum class MouseBtn : uint8_t
{
    None,
    ButtonLeft,
    ButtonMid,
    ButtonRight,
    ButtonGoBack,
    ButtonGoForward,
    ButtonReleased,
    WheelUp,
    WheelDown,
};

/** Key modifiers */
#define KEY_MOD_NONE    0
#define KEY_MOD_CTRL    1
#define KEY_MOD_ALT     2
#define KEY_MOD_SHIFT   4
#define KEY_MOD_SPECIAL 8

/**
 * @brief Decoded terminal key
 */
struct KeyCode
{
    union
    {
        /** used for regular text input */
        char    utf8[5];    // NUL terminated UTF-8 code: 'a', '4', 'Ł'
        /** used for special keys */
        Key     key = {};   // 'F1', 'Enter'
        /** used for mouse events (when key == Key::MouseClick) */
        struct
        {
            // same as key above
            Key      key;
            /** button or wheel event */
            MouseBtn btn;
            /** 1:1 based terminal coordinates of the event */
            uint8_t  col;
            uint8_t  row;
        } mouse;
    };

    union
    {
        uint8_t mod_all = 0;    // KEY_MOD_CTRL | KEY_MOD_SHIFT
        struct
        {
            uint8_t m_ctrl  : 1; // KEY_MOD_CTRL
            uint8_t m_alt   : 1; // KEY_MOD_ALT
            uint8_t m_shift : 1; // KEY_MOD_SHIFT
            uint8_t m_spec  : 1; // KEY_MOD_SPECIAL
        };
    };

    const char *name = nullptr;
};

// -----------------------------------------------------------------------------

} // namespace
