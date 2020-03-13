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
struct KeySequence
{
    /** ANSI key sequence, up to 8 characters, NUL terminated */
    char    keySeq[9];  // 'A', '\x1B[A~'
    /** sequence length */
    uint8_t seqLen;
};


/** Special keys */
enum class Key : uint8_t
{
    None,
    Esc,
    Return,
    Tab,
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
        char    code[5];    // UTF-8 code: 'a', '4', 'Ł'
        Key     key;    // 'F1'
    };

    union
    {
        uint8_t mod;    // KEY_MOD_CTRL | KEY_MOD_SHIFT
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
