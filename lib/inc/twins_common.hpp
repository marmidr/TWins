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

/** @brief Template returning length of array of type T */
template<unsigned N, typename T>
unsigned arrSize(const T (&arr)[N]) { return N; }


/**
 * @brief I/O layer for easy porting
 */
struct IOs
{
    int     (*writeStr)(const char *s);
    int     (*writeStrFmt)(const char *fmt, va_list ap);
    void *  (*malloc)(uint32_t sz);
    void    (*mfree)(void *ptr);
};

// pointer set by init()
extern const IOs *pIOs;

/**
 * @brief RAW terminal keyboard keys sequence
 */
struct AnsiSequence
{
    /** ANSI key sequence, up to 8 characters, NUL terminated */
    char    data[9];  // 'A', '\x1B[A~', u8'Ź'
    /** sequence length */
    uint8_t len;
};

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
        char    utf8[5];// UTF-8 code: 'a', '4', 'Ł'
        Key     key;    // 'F1', 'Enter'
    };

    union
    {
        uint8_t mod_all;    // KEY_MOD_CTRL | KEY_MOD_SHIFT
        struct
        {
            uint8_t ctrl  : 1;
            uint8_t alt   : 1;
            uint8_t shift : 1;
            uint8_t spec  : 1;
        };
    };

    const char *name;
};

// -----------------------------------------------------------------------------

} // namespace
