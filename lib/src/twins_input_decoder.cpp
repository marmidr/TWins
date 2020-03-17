/******************************************************************************
 * @brief   TWins - key sequence decoder
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 * @note    Based on Python script : https://wiki.bash-hackers.org/scripting/terminalcodes
 *          constexpr sorted array based on @stryku example:
 *              https://stackoverflow.com/questions/19559808/constexpr-initialization-of-array-to-sort-contents
 *          and https://ideone.com/CeeJUy
 *****************************************************************************/

#include "twins_common.hpp"

#include <stdint.h>
#include <string.h>
#include <utility> // std::move
// #include <stdio.h>

// -----------------------------------------------------------------------------

#ifndef TWINS_USE_KEY_NAMES
# define TWINS_USE_KEY_NAMES 1
#endif


#if TWINS_USE_KEY_NAMES
# define KEY_DEF(seq, name, code, mod)   seq, name, (Key)code, mod,
#else
# define KEY_DEF(seq, name, code, mod)   seq,       (Key)code, mod,
#endif

// -----------------------------------------------------------------------------

namespace twins
{

struct SeqMap
{
    const char *seq;
#if TWINS_USE_KEY_NAMES
    const char *name;
#endif
    Key         key;
    uint8_t     mod;
};

struct CtrlMap
{
    char        c;
#if TWINS_USE_KEY_NAMES
    const char *name;
#endif
    Key         key;
    uint8_t     mod;
};

// -----------------------------------------------------------------------------

/// @brief contexpr array template
template <typename T, unsigned N>
struct CexArray
{
    constexpr       T& operator[](unsigned i)       { return arr[i]; }
    constexpr const T& operator[](unsigned i) const { return arr[i]; }
    constexpr const T* begin()                const { return arr; }
    constexpr const T* end()                  const { return arr + N; }
    constexpr unsigned size()                 const { return N; }

    T arr[N];
};

/// @brief contexpr comparison operator needed for sort function
constexpr bool operator <(const SeqMap &left, const SeqMap &right)
{
    const char *pl = left.seq;
    const char *pr = right.seq;

    while (*pl == *pr)
        pl++, pr++;
    return *pl < *pr;
}

/// @brief contexpr swap
template<class T>
constexpr void cex_swap(T& l, T& r)
{
    T tmp = std::move(l);
    l = std::move(r);
    r = std::move(tmp);
}

/// @brief contexpr sort
template <typename T, unsigned N>
constexpr void cex_sort_impl(CexArray<T, N> &array, unsigned left, unsigned right)
{
    if (left < right)
    {
        unsigned m = left;

        for (unsigned i = left + 1; i < right; i++)
            if (array[i] < array[left])
                cex_swap(array[++m], array[i]);

        cex_swap(array[left], array[m]);

        cex_sort_impl(array, left, m);
        cex_sort_impl(array, m + 1, right);
    }
}

/// @brief returns contexpr sorted array
template <typename T, unsigned N>
constexpr CexArray<T, N> cex_sort_arr(CexArray<T, N> array)
{
    auto sorted = array;
    cex_sort_impl(sorted, 0, N);
    return sorted;
}

// -----------------------------------------------------------------------------

constexpr CexArray<SeqMap, 151> esc_keys_map_unsorted
{
    KEY_DEF("[A",       "Up",           Key::Up,        KEY_MOD_SPECIAL)   // xterm
    KEY_DEF("[B",       "Down",         Key::Down,      KEY_MOD_SPECIAL)   // xterm
    KEY_DEF("[C",       "Right",        Key::Right,     KEY_MOD_SPECIAL)   // xterm
    KEY_DEF("[D",       "Left",         Key::Left,      KEY_MOD_SPECIAL)   // xterm
    KEY_DEF("[F",       "End",          Key::End,       KEY_MOD_SPECIAL)   // xterm
    KEY_DEF("[H",       "Home",         Key::Home,      KEY_MOD_SPECIAL)   // xterm
    KEY_DEF("[1~",      "Home",         Key::Home,      KEY_MOD_SPECIAL)   // vt
    KEY_DEF("[2~",      "Ins",          Key::Insert,    KEY_MOD_SPECIAL)   // vt
    KEY_DEF("[3~",      "Del",          Key::Delete,    KEY_MOD_SPECIAL)   // vt
    KEY_DEF("[4~",      "End",          Key::End,       KEY_MOD_SPECIAL)   // vt
    KEY_DEF("[5~",      "PgUp",         Key::PgUp,      KEY_MOD_SPECIAL)   // vt
    KEY_DEF("[6~",      "PdDown",       Key::PgDown,    KEY_MOD_SPECIAL)   // vt
    KEY_DEF("[7~",      "Home",         Key::Home,      KEY_MOD_SPECIAL)   // vt
    KEY_DEF("[8~",      "End",          Key::End,       KEY_MOD_SPECIAL)   // vt
    KEY_DEF("OP",       "F1",           Key::F1,        KEY_MOD_SPECIAL)
    KEY_DEF("OQ",       "F2",           Key::F2,        KEY_MOD_SPECIAL)
    KEY_DEF("OR",       "F3",           Key::F3,        KEY_MOD_SPECIAL)
    KEY_DEF("OS",       "F4",           Key::F4,        KEY_MOD_SPECIAL)
    KEY_DEF("[15~",     "F5",           Key::F5,        KEY_MOD_SPECIAL)
    KEY_DEF("[17~",     "F6",           Key::F6,        KEY_MOD_SPECIAL)
    KEY_DEF("[18~",     "F7",           Key::F7,        KEY_MOD_SPECIAL)
    KEY_DEF("[19~",     "F8",           Key::F8,        KEY_MOD_SPECIAL)
    KEY_DEF("[20~",     "F9",           Key::F9,        KEY_MOD_SPECIAL)
    KEY_DEF("[21~",     "F10",          Key::F10,       KEY_MOD_SPECIAL)
    KEY_DEF("[23~",     "F11",          Key::F11,       KEY_MOD_SPECIAL)
    KEY_DEF("[24~",     "F12",          Key::F12,       KEY_MOD_SPECIAL)
    // + Shift
    KEY_DEF("[1;2A",    "S-Up",         Key::Up,        KEY_MOD_SPECIAL | KEY_MOD_SHIFT)
    KEY_DEF("[1;2B",    "S-Down",       Key::Down,      KEY_MOD_SPECIAL | KEY_MOD_SHIFT)
    KEY_DEF("[1;2C",    "S-Right",      Key::Right,     KEY_MOD_SPECIAL | KEY_MOD_SHIFT)
    KEY_DEF("[1;2D",    "S-Left",       Key::Left,      KEY_MOD_SPECIAL | KEY_MOD_SHIFT)
    KEY_DEF("[1;2F",    "S-End",        Key::End,       KEY_MOD_SPECIAL | KEY_MOD_SHIFT)
    KEY_DEF("[1;2H",    "S-Home",       Key::Home,      KEY_MOD_SPECIAL | KEY_MOD_SHIFT)
    KEY_DEF("[1;2~",    "S-Home",       Key::Home,      KEY_MOD_SPECIAL | KEY_MOD_SHIFT)
    KEY_DEF("[2;2~",    "S-Ins",        Key::Insert,    KEY_MOD_SPECIAL | KEY_MOD_SHIFT)
    KEY_DEF("[3;2~",    "S-Del",        Key::Delete,    KEY_MOD_SPECIAL | KEY_MOD_SHIFT)
    KEY_DEF("[4;2~",    "S-End",        Key::End,       KEY_MOD_SPECIAL | KEY_MOD_SHIFT)
    KEY_DEF("[5;2~",    "S-PgUp",       Key::PgUp,      KEY_MOD_SPECIAL | KEY_MOD_SHIFT)
    KEY_DEF("[6;2~",    "S-PdDown",     Key::PgDown,    KEY_MOD_SPECIAL | KEY_MOD_SHIFT)
    KEY_DEF("[1;2P",    "S-F1",         Key::F1,        KEY_MOD_SPECIAL | KEY_MOD_SHIFT)
    KEY_DEF("[1;2Q",    "S-F2",         Key::F2,        KEY_MOD_SPECIAL | KEY_MOD_SHIFT)
    KEY_DEF("[1;2R",    "S-F3",         Key::F3,        KEY_MOD_SPECIAL | KEY_MOD_SHIFT)
    KEY_DEF("[1;2S",    "S-F4",         Key::F4,        KEY_MOD_SPECIAL | KEY_MOD_SHIFT)
    KEY_DEF("[15;2~",   "S-F5",         Key::F5,        KEY_MOD_SPECIAL | KEY_MOD_SHIFT)
    KEY_DEF("[17;2~",   "S-F6",         Key::F6,        KEY_MOD_SPECIAL | KEY_MOD_SHIFT)
    KEY_DEF("[18;2~",   "S-F7",         Key::F7,        KEY_MOD_SPECIAL | KEY_MOD_SHIFT)
    KEY_DEF("[19;2~",   "S-F8",         Key::F8,        KEY_MOD_SPECIAL | KEY_MOD_SHIFT)
    KEY_DEF("[20;2~",   "S-F9",         Key::F9,        KEY_MOD_SPECIAL | KEY_MOD_SHIFT)
    KEY_DEF("[21;2~",   "S-F10",        Key::F10,       KEY_MOD_SPECIAL | KEY_MOD_SHIFT)
    KEY_DEF("[23;2~",   "S-F11",        Key::F11,       KEY_MOD_SPECIAL | KEY_MOD_SHIFT)
    KEY_DEF("[24;2~",   "S-F12",        Key::F12,       KEY_MOD_SPECIAL | KEY_MOD_SHIFT)
    KEY_DEF("[Z",       "S-Tab",        Key::Tab,       KEY_MOD_SPECIAL | KEY_MOD_SHIFT)
    // + Alt
    KEY_DEF("[1;3A",    "M-Up",         Key::Up,        KEY_MOD_SPECIAL | KEY_MOD_ALT)
    KEY_DEF("[1;3B",    "M-Down",       Key::Down,      KEY_MOD_SPECIAL | KEY_MOD_ALT)
    KEY_DEF("[1;3C",    "M-Right",      Key::Right,     KEY_MOD_SPECIAL | KEY_MOD_ALT)
    KEY_DEF("[1;3D",    "M-Left",       Key::Left,      KEY_MOD_SPECIAL | KEY_MOD_ALT)
    KEY_DEF("[1;3F",    "M-End",        Key::End,       KEY_MOD_SPECIAL | KEY_MOD_ALT)
    KEY_DEF("[1;3H",    "M-Home",       Key::Home,      KEY_MOD_SPECIAL | KEY_MOD_ALT)
    KEY_DEF("[2;3~",    "M-Ins",        Key::Insert,    KEY_MOD_SPECIAL | KEY_MOD_ALT)
    KEY_DEF("[3;3~",    "M-Del",        Key::Delete,    KEY_MOD_SPECIAL | KEY_MOD_ALT)
    KEY_DEF("[5;3~",    "M-PgUp",       Key::PgUp,      KEY_MOD_SPECIAL | KEY_MOD_ALT)
    KEY_DEF("[6;3~",    "M-PdDown",     Key::PgDown,    KEY_MOD_SPECIAL | KEY_MOD_ALT)
    KEY_DEF("[1;3P",    "M-F1",         Key::F1,        KEY_MOD_SPECIAL | KEY_MOD_ALT)
    KEY_DEF("[1;3Q",    "M-F2",         Key::F2,        KEY_MOD_SPECIAL | KEY_MOD_ALT)
    KEY_DEF("[1;3R",    "M-F3",         Key::F3,        KEY_MOD_SPECIAL | KEY_MOD_ALT)
    KEY_DEF("[1;3S",    "M-F4",         Key::F4,        KEY_MOD_SPECIAL | KEY_MOD_ALT)
    KEY_DEF("[15;3~",   "M-F5",         Key::F5,        KEY_MOD_SPECIAL | KEY_MOD_ALT)
    KEY_DEF("[17;3~",   "M-F6",         Key::F6,        KEY_MOD_SPECIAL | KEY_MOD_ALT)
    KEY_DEF("[18;3~",   "M-F7",         Key::F7,        KEY_MOD_SPECIAL | KEY_MOD_ALT)
    KEY_DEF("[19;3~",   "M-F8",         Key::F8,        KEY_MOD_SPECIAL | KEY_MOD_ALT)
    KEY_DEF("[20;3~",   "M-F9",         Key::F9,        KEY_MOD_SPECIAL | KEY_MOD_ALT)
    KEY_DEF("[21;3~",   "M-F10",        Key::F10,       KEY_MOD_SPECIAL | KEY_MOD_ALT)
    KEY_DEF("[23;3~",   "M-F11",        Key::F11,       KEY_MOD_SPECIAL | KEY_MOD_ALT)
    KEY_DEF("[24;3~",   "M-F12",        Key::F12,       KEY_MOD_SPECIAL | KEY_MOD_ALT)
    // + Ctrl
    KEY_DEF("[1;5A",    "C-Up",         Key::Up,        KEY_MOD_SPECIAL | KEY_MOD_CTRL)
    KEY_DEF("[1;5B",    "C-Down",       Key::Down,      KEY_MOD_SPECIAL | KEY_MOD_CTRL)
    KEY_DEF("[1;5C",    "C-Right",      Key::Right,     KEY_MOD_SPECIAL | KEY_MOD_CTRL)
    KEY_DEF("[1;5D",    "C-Left",       Key::Left,      KEY_MOD_SPECIAL | KEY_MOD_CTRL)
    KEY_DEF("[1;5F",    "C-End",        Key::End,       KEY_MOD_SPECIAL | KEY_MOD_CTRL)
    KEY_DEF("[1;5H",    "C-Home",       Key::Home,      KEY_MOD_SPECIAL | KEY_MOD_CTRL)
    KEY_DEF("[2;5~",    "C-Ins",        Key::Insert,    KEY_MOD_SPECIAL | KEY_MOD_CTRL)
    KEY_DEF("[3;5~",    "C-Del",        Key::Delete,    KEY_MOD_SPECIAL | KEY_MOD_CTRL)
    KEY_DEF("[5;5~",    "C-PgUp",       Key::PgUp,      KEY_MOD_SPECIAL | KEY_MOD_CTRL)
    KEY_DEF("[6;5~",    "C-PdDown",     Key::PgDown,    KEY_MOD_SPECIAL | KEY_MOD_CTRL)
    KEY_DEF("[1;5P",    "C-F1",         Key::F1,        KEY_MOD_SPECIAL | KEY_MOD_CTRL)
    KEY_DEF("[1;5Q",    "C-F2",         Key::F2,        KEY_MOD_SPECIAL | KEY_MOD_CTRL)
    KEY_DEF("[1;5R",    "C-F3",         Key::F3,        KEY_MOD_SPECIAL | KEY_MOD_CTRL)
    KEY_DEF("[1;5S",    "C-F4",         Key::F4,        KEY_MOD_SPECIAL | KEY_MOD_CTRL)
    KEY_DEF("[15;5~",   "C-F5",         Key::F5,        KEY_MOD_SPECIAL | KEY_MOD_CTRL)
    KEY_DEF("[17;5~",   "C-F6",         Key::F6,        KEY_MOD_SPECIAL | KEY_MOD_CTRL)
    KEY_DEF("[18;5~",   "C-F7",         Key::F7,        KEY_MOD_SPECIAL | KEY_MOD_CTRL)
    KEY_DEF("[19;5~",   "C-F8",         Key::F8,        KEY_MOD_SPECIAL | KEY_MOD_CTRL)
    KEY_DEF("[20;5~",   "C-F9",         Key::F9,        KEY_MOD_SPECIAL | KEY_MOD_CTRL)
    KEY_DEF("[21;5~",   "C-F10",        Key::F10,       KEY_MOD_SPECIAL | KEY_MOD_CTRL)
    KEY_DEF("[23;5~",   "C-F11",        Key::F11,       KEY_MOD_SPECIAL | KEY_MOD_CTRL)
    KEY_DEF("[24;5~",   "C-F12",        Key::F12,       KEY_MOD_SPECIAL | KEY_MOD_CTRL)
    // + Shit + Ctrl
    KEY_DEF("[1;6A",    "S-C-Up",       Key::Up,        KEY_MOD_SPECIAL | KEY_MOD_SHIFT | KEY_MOD_CTRL)
    KEY_DEF("[1;6B",    "S-C-Down",     Key::Down,      KEY_MOD_SPECIAL | KEY_MOD_SHIFT | KEY_MOD_CTRL)
    KEY_DEF("[1;6C",    "S-C-Right",    Key::Right,     KEY_MOD_SPECIAL | KEY_MOD_SHIFT | KEY_MOD_CTRL)
    KEY_DEF("[1;6D",    "S-C-Left",     Key::Left,      KEY_MOD_SPECIAL | KEY_MOD_SHIFT | KEY_MOD_CTRL)
    KEY_DEF("[1;6F",    "S-C-End",      Key::End,       KEY_MOD_SPECIAL | KEY_MOD_SHIFT | KEY_MOD_CTRL)
    KEY_DEF("[1;6H",    "S-C-Home",     Key::Home,      KEY_MOD_SPECIAL | KEY_MOD_SHIFT | KEY_MOD_CTRL)
    KEY_DEF("[2;6~",    "S-C-Ins",      Key::Insert,    KEY_MOD_SPECIAL | KEY_MOD_SHIFT | KEY_MOD_CTRL)
    KEY_DEF("[3;6~",    "S-C-Del",      Key::Delete,    KEY_MOD_SPECIAL | KEY_MOD_SHIFT | KEY_MOD_CTRL)
    KEY_DEF("[5;6~",    "S-C-PgUp",     Key::PgUp,      KEY_MOD_SPECIAL | KEY_MOD_SHIFT | KEY_MOD_CTRL)
    KEY_DEF("[6;6~",    "S-C-PdDown",   Key::PgDown,    KEY_MOD_SPECIAL | KEY_MOD_SHIFT | KEY_MOD_CTRL)
    KEY_DEF("[1;6P",    "S-C-F1",       Key::F1,        KEY_MOD_SPECIAL | KEY_MOD_SHIFT | KEY_MOD_CTRL)
    KEY_DEF("[1;6Q",    "S-C-F2",       Key::F2,        KEY_MOD_SPECIAL | KEY_MOD_SHIFT | KEY_MOD_CTRL)
    KEY_DEF("[1;6R",    "S-C-F3",       Key::F3,        KEY_MOD_SPECIAL | KEY_MOD_SHIFT | KEY_MOD_CTRL)
    KEY_DEF("[1;6S",    "S-C-F4",       Key::F4,        KEY_MOD_SPECIAL | KEY_MOD_SHIFT | KEY_MOD_CTRL)
    KEY_DEF("[15;6~",   "S-C-F5",       Key::F5,        KEY_MOD_SPECIAL | KEY_MOD_SHIFT | KEY_MOD_CTRL)
    KEY_DEF("[17;6~",   "S-C-F6",       Key::F6,        KEY_MOD_SPECIAL | KEY_MOD_SHIFT | KEY_MOD_CTRL)
    KEY_DEF("[18;6~",   "S-C-F7",       Key::F7,        KEY_MOD_SPECIAL | KEY_MOD_SHIFT | KEY_MOD_CTRL)
    KEY_DEF("[19;6~",   "S-C-F8",       Key::F8,        KEY_MOD_SPECIAL | KEY_MOD_SHIFT | KEY_MOD_CTRL)
    KEY_DEF("[20;6~",   "S-C-F9",       Key::F9,        KEY_MOD_SPECIAL | KEY_MOD_SHIFT | KEY_MOD_CTRL)
    KEY_DEF("[21;6~",   "S-C-F10",      Key::F10,       KEY_MOD_SPECIAL | KEY_MOD_SHIFT | KEY_MOD_CTRL)
    KEY_DEF("[23;6~",   "S-C-F11",      Key::F11,       KEY_MOD_SPECIAL | KEY_MOD_SHIFT | KEY_MOD_CTRL)
    KEY_DEF("[24;6~",   "S-C-F12",      Key::F12,       KEY_MOD_SPECIAL | KEY_MOD_SHIFT | KEY_MOD_CTRL)
    KEY_DEF("[23^",     "S-C-F1",       Key::F1,        KEY_MOD_SPECIAL | KEY_MOD_SHIFT | KEY_MOD_CTRL)
    KEY_DEF("[24^",     "S-C-F2",       Key::F2,        KEY_MOD_SPECIAL | KEY_MOD_SHIFT | KEY_MOD_CTRL)
    KEY_DEF("[25^",     "S-C-F3",       Key::F3,        KEY_MOD_SPECIAL | KEY_MOD_SHIFT | KEY_MOD_CTRL)
    KEY_DEF("[26^",     "S-C-F4",       Key::F4,        KEY_MOD_SPECIAL | KEY_MOD_SHIFT | KEY_MOD_CTRL)
    KEY_DEF("[28^",     "S-C-F5",       Key::F5,        KEY_MOD_SPECIAL | KEY_MOD_SHIFT | KEY_MOD_CTRL)
    KEY_DEF("[29^",     "S-C-F6",       Key::F6,        KEY_MOD_SPECIAL | KEY_MOD_SHIFT | KEY_MOD_CTRL)
    KEY_DEF("[31^",     "S-C-F7",       Key::F7,        KEY_MOD_SPECIAL | KEY_MOD_SHIFT | KEY_MOD_CTRL)
    KEY_DEF("[32^",     "S-C-F8",       Key::F8,        KEY_MOD_SPECIAL | KEY_MOD_SHIFT | KEY_MOD_CTRL)
    KEY_DEF("[33^",     "S-C-F9",       Key::F9,        KEY_MOD_SPECIAL | KEY_MOD_SHIFT | KEY_MOD_CTRL)
    KEY_DEF("[34^",     "S-C-F10",      Key::F10,       KEY_MOD_SPECIAL | KEY_MOD_SHIFT | KEY_MOD_CTRL)
    KEY_DEF("[23@",     "S-C-F11",      Key::F11,       KEY_MOD_SPECIAL | KEY_MOD_SHIFT | KEY_MOD_CTRL)
    KEY_DEF("[24@",     "S-C-F12",      Key::F12,       KEY_MOD_SPECIAL | KEY_MOD_SHIFT | KEY_MOD_CTRL)
    // + Ctrl + Alt
    KEY_DEF("[1;7A",    "C-M-Up",       Key::Up,        KEY_MOD_SPECIAL | KEY_MOD_CTRL | KEY_MOD_ALT)
    KEY_DEF("[1;7B",    "C-M-Down",     Key::Down,      KEY_MOD_SPECIAL | KEY_MOD_CTRL | KEY_MOD_ALT)
    KEY_DEF("[1;7C",    "C-M-Right",    Key::Right,     KEY_MOD_SPECIAL | KEY_MOD_CTRL | KEY_MOD_ALT)
    KEY_DEF("[1;7D",    "C-M-Left",     Key::Left,      KEY_MOD_SPECIAL | KEY_MOD_CTRL | KEY_MOD_ALT)
    KEY_DEF("[1;7F",    "C-M-End",      Key::End,       KEY_MOD_SPECIAL | KEY_MOD_CTRL | KEY_MOD_ALT)
    KEY_DEF("[1;7H",    "C-M-Home",     Key::Home,      KEY_MOD_SPECIAL | KEY_MOD_CTRL | KEY_MOD_ALT)
    KEY_DEF("[2;7~",    "C-M-Ins",      Key::Insert,    KEY_MOD_SPECIAL | KEY_MOD_CTRL | KEY_MOD_ALT)
    KEY_DEF("[3;7~",    "C-M-Del",      Key::Delete,    KEY_MOD_SPECIAL | KEY_MOD_CTRL | KEY_MOD_ALT)
    KEY_DEF("[5;7~",    "C-M-PgUp",     Key::PgUp,      KEY_MOD_SPECIAL | KEY_MOD_CTRL | KEY_MOD_ALT)
    KEY_DEF("[6;7~",    "C-M-PdDown",   Key::PgDown,    KEY_MOD_SPECIAL | KEY_MOD_CTRL | KEY_MOD_ALT)
    KEY_DEF("[1;7P",    "C-M-F1",       Key::F1,        KEY_MOD_SPECIAL | KEY_MOD_CTRL | KEY_MOD_ALT)
    KEY_DEF("[1;7Q",    "C-M-F2",       Key::F2,        KEY_MOD_SPECIAL | KEY_MOD_CTRL | KEY_MOD_ALT)
    KEY_DEF("[1;7R",    "C-M-F3",       Key::F3,        KEY_MOD_SPECIAL | KEY_MOD_CTRL | KEY_MOD_ALT)
    KEY_DEF("[1;7S",    "C-M-F4",       Key::F4,        KEY_MOD_SPECIAL | KEY_MOD_CTRL | KEY_MOD_ALT)
    KEY_DEF("[15;7~",   "C-M-F5",       Key::F5,        KEY_MOD_SPECIAL | KEY_MOD_CTRL | KEY_MOD_ALT)
    KEY_DEF("[17;7~",   "C-M-F6",       Key::F6,        KEY_MOD_SPECIAL | KEY_MOD_CTRL | KEY_MOD_ALT)
    KEY_DEF("[18;7~",   "C-M-F7",       Key::F7,        KEY_MOD_SPECIAL | KEY_MOD_CTRL | KEY_MOD_ALT)
    KEY_DEF("[19;7~",   "C-M-F8",       Key::F8,        KEY_MOD_SPECIAL | KEY_MOD_CTRL | KEY_MOD_ALT)
    KEY_DEF("[20;7~",   "C-M-F9",       Key::F9,        KEY_MOD_SPECIAL | KEY_MOD_CTRL | KEY_MOD_ALT)
    KEY_DEF("[21;7~",   "C-M-F10",      Key::F10,       KEY_MOD_SPECIAL | KEY_MOD_CTRL | KEY_MOD_ALT)
    KEY_DEF("[23;7~",   "C-M-F11",      Key::F11,       KEY_MOD_SPECIAL | KEY_MOD_CTRL | KEY_MOD_ALT)
    KEY_DEF("[24;7~",   "C-M-F12",      Key::F12,       KEY_MOD_SPECIAL | KEY_MOD_CTRL | KEY_MOD_ALT)
    // + Shift + Alt
};

constexpr auto esc_keys_map_sorted = cex_sort_arr(esc_keys_map_unsorted);


const CtrlMap ctrl_keys_map[] =
{
    // KEY_DEF( 0, "C-2", '2', KEY_MOD_CTRL)
    KEY_DEF(0x01, "C-A", 'A', KEY_MOD_CTRL)
    KEY_DEF(0x02, "C-B", 'B', KEY_MOD_CTRL)
    KEY_DEF(0x03, "C-C", 'C', KEY_MOD_CTRL)
    KEY_DEF(0x04, "C-D", 'D', KEY_MOD_CTRL)
    KEY_DEF(0x05, "C-E", 'E', KEY_MOD_CTRL)
    KEY_DEF(0x06, "C-F", 'F', KEY_MOD_CTRL)
    KEY_DEF(0x07, "C-G", 'G', KEY_MOD_CTRL)
    KEY_DEF(0x08, "C-H", 'H', KEY_MOD_CTRL) // BS
    KEY_DEF(0x09, "C-I", 'I', KEY_MOD_CTRL) // HT
    KEY_DEF(0x0A, "C-J", 'J', KEY_MOD_CTRL) // LF
    KEY_DEF(0x0B, "C-K", 'K', KEY_MOD_CTRL)
    KEY_DEF(0x0C, "C-L", 'L', KEY_MOD_CTRL)
    KEY_DEF(0x0D, "C-M", 'M', KEY_MOD_CTRL) // CR
    KEY_DEF(0x0E, "C-N", 'N', KEY_MOD_CTRL)
    KEY_DEF(0x0F, "C-O", 'O', KEY_MOD_CTRL)
    KEY_DEF(0x10, "C-P", 'P', KEY_MOD_CTRL)
    KEY_DEF(0x11, "C-Q", 'Q', KEY_MOD_CTRL)
    KEY_DEF(0x12, "C-R", 'R', KEY_MOD_CTRL)
    KEY_DEF(0x13, "C-S", 'S', KEY_MOD_CTRL)
    KEY_DEF(0x14, "C-T", 'T', KEY_MOD_CTRL)
    KEY_DEF(0x15, "C-U", 'U', KEY_MOD_CTRL)
    KEY_DEF(0x16, "C-V", 'V', KEY_MOD_CTRL)
    KEY_DEF(0x17, "C-W", 'W', KEY_MOD_CTRL)
    KEY_DEF(0x18, "C-X", 'X', KEY_MOD_CTRL)
    KEY_DEF(0x19, "C-Y", 'Y', KEY_MOD_CTRL)
    KEY_DEF(0x1A, "C-Z", 'Z', KEY_MOD_CTRL)
};


const CtrlMap ansi_map[] =
{
    KEY_DEF((char)Ansi::BS,   "Backspace",    Key::Backspace, KEY_MOD_SPECIAL)
    KEY_DEF((char)Ansi::DEL,  "Backspace",    Key::Backspace, KEY_MOD_SPECIAL)
    KEY_DEF((char)Ansi::HT,   "Tab",          Key::Tab,       KEY_MOD_SPECIAL)
    KEY_DEF((char)Ansi::LF,   "Enter",        Key::Enter,     KEY_MOD_SPECIAL)
    KEY_DEF((char)Ansi::ESC,  "Esc",          Key::Esc,       KEY_MOD_SPECIAL)
};

// -----------------------------------------------------------------------------
#define ESC_CURSOR_HOME "\033[H"

/**
 * @brief fast binary search of key-sequence \p seq in sorted \p map
 * @return pointer if found, nullptr otherwise
 */
static const SeqMap *binary_search(const char *seq, const SeqMap map[], unsigned mapsize)
{
    if (!seq || !*seq || !mapsize)
        return nullptr;

    short lo = 0;
    short hi = mapsize - 1;
    short mid = (hi - lo) / 2;
    //short steps = 1;

    do
    {
        int compare = strcmp(seq, map[mid].seq);

        if (compare == 0)
        {
            //printf("%s binar.found in %u steps\n", ESC_CURSOR_HOME, steps);
            return &map[mid];
        }
        else if (compare > 0)
            lo = mid + 1;
        else
            hi = mid - 1;

        mid = lo + ((hi - lo) / 2);
        //steps++;
    }
    while (hi >= lo);

    // seq not found
    return nullptr;
}

void decodeInputSeq(AnsiSequence &input, KeyCode &output)
{
    output = {};

    if (input.len == 0)
        return;

    // single byte: char, special or control key
    if (input.len == 1)
    {
        const char c0 = input.data[0];

        // check for ansi key
        // note: it conflicts with ctrl_keys_map[] but has higher priority
        for (const auto &key : ansi_map)
        {
            if (c0 == key.c)
            {
                output.key = key.key;
                output.mod_all = key.mod;
                #if TWINS_USE_KEY_NAMES
                output.name = key.name;
                #endif
                return;
            }
        }


        // check for one of Ctrl+[A..Z]
        for (const auto &key : ctrl_keys_map)
        {
            if (c0 == key.c)
            {
                output.utf8[0] = (char)key.key;
                output.utf8[1] = '\0';
                output.mod_all = key.mod;
                #if TWINS_USE_KEY_NAMES
                output.name = key.name;
                #endif
                return;
            }
        }
    }

    // ANSI escape sequence
    if (input.data[0] == (char)Ansi::ESC)
    {
        // binary search: find key in max 7 steps
        if (auto *p = binary_search(input.data+1, esc_keys_map_sorted.arr, esc_keys_map_sorted.size()))
        {
            output.key = p->key;
            output.mod_all = p->mod | KEY_MOD_SPECIAL;
            #if TWINS_USE_KEY_NAMES
            output.name = p->name;
            #endif
        }

        // old, linear search:
        // for (const auto &km : esc_keys_map_sorted)
        // {
        //     int cmp = strcmp(input.data + 1, km.seq);
        //     if (cmp == 0)
        //     {
        //         output.key = km.key;
        //         output.mod_all = km.mod | KEY_MOD_SPECIAL;
        //         #if TWINS_USE_KEY_NAMES
        //         output.name = km.name;
        //         #endif
        //         break;
        //     }
        // }

        if (!output.name) output.name = "<?>";
    }
    else
    {
        // copy UTF-8 seq
        output.utf8[0] = input.data[0];
        output.utf8[1] = input.data[1];
        output.utf8[2] = input.data[2];
        output.utf8[3] = input.data[3];
        output.utf8[sizeof(output.utf8)-1] = '\0';
        #if TWINS_USE_KEY_NAMES
        output.name = output.utf8;
        #endif
    }
}

// -----------------------------------------------------------------------------

}