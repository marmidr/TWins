/******************************************************************************
 * @brief   TWins - key sequence decoder
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 * @note    Based on Python script : https://wiki.bash-hackers.org/scripting/terminalcodes
 *****************************************************************************/

#include "twins_common.hpp"

#include <stdint.h>
#include <string.h>

// -----------------------------------------------------------------------------

#define TWINS_USE_KEY_NAMES 1

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
    char        ctrl_code;
#if TWINS_USE_KEY_NAMES
    const char *name;
#endif
    Key         key;
    uint8_t     mod;
};

const SeqMap keys_map[] =
{
    KEY_DEF("\x09",     "Tab",      '\t', 0)
    KEY_DEF("\x0C",     "Return",   '\n', 0)
    KEY_DEF("\x1B",     "Esc",      '\e', 0)
};

const SeqMap esc_keys_map[] =
{
    KEY_DEF("[A",       "Up",       Key::Up,        KEY_MOD_NONE)   // xterm
    KEY_DEF("[B",       "Down",     Key::Down,      KEY_MOD_NONE)   // xterm
    KEY_DEF("[C",       "Right",    Key::Right,     KEY_MOD_NONE)   // xterm
    KEY_DEF("[D",       "Left",     Key::Left,      KEY_MOD_NONE)   // xterm
    KEY_DEF("[F",       "End",      Key::End,       KEY_MOD_NONE)   // xterm
    KEY_DEF("[H",       "Home",     Key::Home,      KEY_MOD_NONE)   // xterm
    KEY_DEF("[1~",      "Home",     Key::Home,      KEY_MOD_NONE)   // vt
    KEY_DEF("[2~",      "Ins",      Key::Insert,    KEY_MOD_NONE)   // vt
    KEY_DEF("[3~",      "Del",      Key::Delete,    KEY_MOD_NONE)   // vt
    KEY_DEF("[4~",      "End",      Key::End,       KEY_MOD_NONE)   // vt
    KEY_DEF("[5~",      "PgUp",     Key::PgUp,      KEY_MOD_NONE)   // vt
    KEY_DEF("[6~",      "PdDown",   Key::PgDown,    KEY_MOD_NONE)   // vt
    KEY_DEF("[7~",      "Home",     Key::Home,      KEY_MOD_NONE)   // vt
    KEY_DEF("[8~",      "End",      Key::End,       KEY_MOD_NONE)   // vt
    KEY_DEF("OP",       "F1",       Key::F1,        KEY_MOD_NONE)
    KEY_DEF("OQ",       "F2",       Key::F2,        KEY_MOD_NONE)
    KEY_DEF("OR",       "F3",       Key::F3,        KEY_MOD_NONE)
    KEY_DEF("OS",       "F4",       Key::F4,        KEY_MOD_NONE)
    KEY_DEF("[15~",     "F5",       Key::F5,        KEY_MOD_NONE)
    KEY_DEF("[17~",     "F6",       Key::F6,        KEY_MOD_NONE)
    KEY_DEF("[18~",     "F7",       Key::F7,        KEY_MOD_NONE)
    KEY_DEF("[19~",     "F8",       Key::F8,        KEY_MOD_NONE)
    KEY_DEF("[20~",     "F9",       Key::F9,        KEY_MOD_NONE)
    KEY_DEF("[21~",     "F10",      Key::F10,       KEY_MOD_NONE)
    KEY_DEF("[23~",     "F11",      Key::F11,       KEY_MOD_NONE)
    KEY_DEF("[24~",     "F12",      Key::F12,       KEY_MOD_NONE)
    KEY_DEF("[29~",     "Apps",     0,              0)
    KEY_DEF("[34~",     "Win",      0,              0)
    // + Shift
    KEY_DEF("[1;2A",    "S-Up",     Key::Up,        KEY_MOD_SHIFT)
    KEY_DEF("[1;2B",    "S-Down",   Key::Down,      KEY_MOD_SHIFT)
    KEY_DEF("[1;2C",    "S-Right",  Key::Right,     KEY_MOD_SHIFT)
    KEY_DEF("[1;2D",    "S-Left",   Key::Left,      KEY_MOD_SHIFT)
    KEY_DEF("[1;2F",    "S-End",    Key::End,       KEY_MOD_SHIFT)
    KEY_DEF("[1;2H",    "S-Home",   Key::Home,      KEY_MOD_SHIFT)
    KEY_DEF("[1;2~",    "S-Home",   Key::Home,      KEY_MOD_SHIFT)
    KEY_DEF("[2;2~",    "S-Ins",    Key::Insert,    KEY_MOD_SHIFT)
    KEY_DEF("[3;2~",    "S-Del",    Key::Delete,    KEY_MOD_SHIFT)
    KEY_DEF("[4;2~",    "S-End",    Key::End,       KEY_MOD_SHIFT)
    KEY_DEF("[5;2~",    "S-PgUp",   Key::PgUp,      KEY_MOD_SHIFT)
    KEY_DEF("[6;2~",    "S-PdDown", Key::PgDown,    KEY_MOD_SHIFT)
    KEY_DEF("[1;2P",    "S-F1",     Key::F1,        KEY_MOD_SHIFT)
    KEY_DEF("[1;2Q",    "S-F2",     Key::F2,        KEY_MOD_SHIFT)
    KEY_DEF("[1;2R",    "S-F3",     Key::F3,        KEY_MOD_SHIFT)
    KEY_DEF("[1;2S",    "S-F4",     Key::F4,        KEY_MOD_SHIFT)
    KEY_DEF("[15;2~",   "S-F5",     Key::F5,        KEY_MOD_SHIFT)
    KEY_DEF("[17;2~",   "S-F6",     Key::F6,        KEY_MOD_SHIFT)
    KEY_DEF("[18;2~",   "S-F7",     Key::F7,        KEY_MOD_SHIFT)
    KEY_DEF("[19;2~",   "S-F8",     Key::F8,        KEY_MOD_SHIFT)
    KEY_DEF("[20;2~",   "S-F9",     Key::F9,        KEY_MOD_SHIFT)
    KEY_DEF("[21;2~",   "S-F10",    Key::F10,       KEY_MOD_SHIFT)
    KEY_DEF("[23;2~",   "S-F11",    Key::F11,       KEY_MOD_SHIFT)
    KEY_DEF("[24;2~",   "S-F12",    Key::F12,       KEY_MOD_SHIFT)
    KEY_DEF("[29;2~",   "S-Apps",   0,              0)
    KEY_DEF("[34;2~",   "S-Win",    0,              0)
    // + Alt
    KEY_DEF("[1;3A",    "M-Up",     Key::Up,        KEY_MOD_ALT)
    KEY_DEF("[1;3B",    "M-Down",   Key::Down,      KEY_MOD_ALT)
    KEY_DEF("[1;3C",    "M-Right",  Key::Right,     KEY_MOD_ALT)
    KEY_DEF("[1;3D",    "M-Left",   Key::Left,      KEY_MOD_ALT)
    KEY_DEF("[1;3F",    "M-End",    Key::End,       KEY_MOD_ALT)
    KEY_DEF("[1;3H",    "M-Home",   Key::Home,      KEY_MOD_ALT)
    KEY_DEF("",         "M-Home",   0,              0)
    KEY_DEF("[2;3~",    "M-Ins",    Key::Insert,    KEY_MOD_ALT)
    KEY_DEF("[3;3~",    "M-Del",    Key::Delete,    KEY_MOD_ALT)
    KEY_DEF("",         "M-End",    0,              0)
    KEY_DEF("[5;3~",    "M-PgUp",   Key::PgUp,      KEY_MOD_ALT)
    KEY_DEF("[6;3~",    "M-PdDown", Key::PgDown,    KEY_MOD_ALT)
    KEY_DEF("[1;3P",    "M-F1",     Key::F1,        KEY_MOD_ALT)
    KEY_DEF("[1;3Q",    "M-F2",     Key::F2,        KEY_MOD_ALT)
    KEY_DEF("[1;3R",    "M-F3",     Key::F3,        KEY_MOD_ALT)
    KEY_DEF("[1;3S",    "M-F4",     Key::F4,        KEY_MOD_ALT)
    KEY_DEF("[15;3~",   "M-F5",     Key::F5,        KEY_MOD_ALT)
    KEY_DEF("[17;3~",   "M-F6",     Key::F6,        KEY_MOD_ALT)
    KEY_DEF("[18;3~",   "M-F7",     Key::F7,        KEY_MOD_ALT)
    KEY_DEF("[19;3~",   "M-F8",     Key::F8,        KEY_MOD_ALT)
    KEY_DEF("[20;3~",   "M-F9",     Key::F9,        KEY_MOD_ALT)
    KEY_DEF("[21;3~",   "M-F10",    Key::F10,       KEY_MOD_ALT)
    KEY_DEF("[23;3~",   "M-F11",    Key::F11,       KEY_MOD_ALT)
    KEY_DEF("[24;3~",   "M-F12",    Key::F12,       KEY_MOD_ALT)
    KEY_DEF("[29;3~",   "M-Apps",   0, 0)
    KEY_DEF("[34;3~",   "M-Win",    0, 0)
    // + Ctrl
    KEY_DEF("[1;5A",    "C-Up",     Key::Up,        KEY_MOD_CTRL)
    KEY_DEF("[1;5B",    "C-Down",   Key::Down,      KEY_MOD_CTRL)
    KEY_DEF("[1;5C",    "C-Right",  Key::Right,     KEY_MOD_CTRL)
    KEY_DEF("[1;5D",    "C-Left",   Key::Left,      KEY_MOD_CTRL)
    KEY_DEF("[1;5F",    "C-End",    Key::End,       KEY_MOD_CTRL)
    KEY_DEF("[1;5H",    "C-Home",   Key::Home,      KEY_MOD_CTRL)
    KEY_DEF("[2;5~",    "C-Ins",    Key::Insert,    KEY_MOD_CTRL)
    KEY_DEF("[3;5~",    "C-Del",    Key::Delete,    KEY_MOD_CTRL)
    KEY_DEF("",         "C-End",    0,              0)
    KEY_DEF("[5;5~",    "C-PgUp",   Key::PgUp,      KEY_MOD_CTRL)
    KEY_DEF("[6;5~",    "C-PdDown", Key::PgDown,    KEY_MOD_CTRL)
    KEY_DEF("[1;5P",    "C-F1",     Key::F1,        KEY_MOD_CTRL)
    KEY_DEF("[1;5Q",    "C-F2",     Key::F2,        KEY_MOD_CTRL)
    KEY_DEF("[1;5R",    "C-F3",     Key::F3,        KEY_MOD_CTRL)
    KEY_DEF("[1;5S",    "C-F4",     Key::F4,        KEY_MOD_CTRL)
    KEY_DEF("[15;5~",   "C-F5",     Key::F5,        KEY_MOD_CTRL)
    KEY_DEF("[17;5~",   "C-F6",     Key::F6,        KEY_MOD_CTRL)
    KEY_DEF("[18;5~",   "C-F7",     Key::F7,        KEY_MOD_CTRL)
    KEY_DEF("[19;5~",   "C-F8",     Key::F8,        KEY_MOD_CTRL)
    KEY_DEF("[20;5~",   "C-F9",     Key::F9,        KEY_MOD_CTRL)
    KEY_DEF("[21;5~",   "C-F10",    Key::F10,       KEY_MOD_CTRL)
    KEY_DEF("[23;5~",   "C-F11",    Key::F11,       KEY_MOD_CTRL)
    KEY_DEF("[24;5~",   "C-F12",    Key::F12,       KEY_MOD_CTRL)
    KEY_DEF("[29;5~",   "C-Apps",   0,              0)
    KEY_DEF("[34;5~",   "C-Win",    0,              0)
    // + Shit + Ctrl
    KEY_DEF("[1;6A",    "S-C-Up",       Key::Up,        KEY_MOD_SHIFT | KEY_MOD_CTRL)
    KEY_DEF("[1;6B",    "S-C-Down",     Key::Down,      KEY_MOD_SHIFT | KEY_MOD_CTRL)
    KEY_DEF("[1;6C",    "S-C-Right",    Key::Right,     KEY_MOD_SHIFT | KEY_MOD_CTRL)
    KEY_DEF("[1;6D",    "S-C-Left",     Key::Left,      KEY_MOD_SHIFT | KEY_MOD_CTRL)
    KEY_DEF("[1;6F",    "S-C-End",      Key::End,       KEY_MOD_SHIFT | KEY_MOD_CTRL)
    KEY_DEF("[1;6H",    "S-C-Home",     Key::Home,      KEY_MOD_SHIFT | KEY_MOD_CTRL)
    KEY_DEF("[2;6~",    "S-C-Ins",      Key::Insert,    KEY_MOD_SHIFT | KEY_MOD_CTRL)
    KEY_DEF("[3;6~",    "S-C-Del",      Key::Delete,    KEY_MOD_SHIFT | KEY_MOD_CTRL)
    KEY_DEF("",         "S-C-End",      0,              0)
    KEY_DEF("[5;6~",    "S-C-PgUp",     Key::PgUp,      KEY_MOD_SHIFT | KEY_MOD_CTRL)
    KEY_DEF("[6;6~",    "S-C-PdDown",   Key::PgDown,    KEY_MOD_SHIFT | KEY_MOD_CTRL)
    KEY_DEF("[1;6P",    "S-C-F1",       Key::F1,        KEY_MOD_SHIFT | KEY_MOD_CTRL)
    KEY_DEF("[1;6Q",    "S-C-F2",       Key::F2,        KEY_MOD_SHIFT | KEY_MOD_CTRL)
    KEY_DEF("[1;6R",    "S-C-F3",       Key::F3,        KEY_MOD_SHIFT | KEY_MOD_CTRL)
    KEY_DEF("[1;6S",    "S-C-F4",       Key::F4,        KEY_MOD_SHIFT | KEY_MOD_CTRL)
    KEY_DEF("[15;6~",   "S-C-F5",       Key::F5,        KEY_MOD_SHIFT | KEY_MOD_CTRL)
    KEY_DEF("[17;6~",   "S-C-F6",       Key::F6,        KEY_MOD_SHIFT | KEY_MOD_CTRL)
    KEY_DEF("[18;6~",   "S-C-F7",       Key::F7,        KEY_MOD_SHIFT | KEY_MOD_CTRL)
    KEY_DEF("[19;6~",   "S-C-F8",       Key::F8,        KEY_MOD_SHIFT | KEY_MOD_CTRL)
    KEY_DEF("[20;6~",   "S-C-F9",       Key::F9,        KEY_MOD_SHIFT | KEY_MOD_CTRL)
    KEY_DEF("[21;6~",   "S-C-F10",      Key::F10,       KEY_MOD_SHIFT | KEY_MOD_CTRL)
    KEY_DEF("[23;6~",   "S-C-F11",      Key::F11,       KEY_MOD_SHIFT | KEY_MOD_CTRL)
    KEY_DEF("[24;6~",   "S-C-F12",      Key::F12,       KEY_MOD_SHIFT | KEY_MOD_CTRL)
    KEY_DEF("[29;6~",   "S-C-Apps",     0,              0)
    KEY_DEF("[34;6~",   "S-C-Win",      0,              0)
    // + Ctrl + Alt
    KEY_DEF("[1;7A",    "C-M-Up",       Key::Up,        KEY_MOD_CTRL | KEY_MOD_ALT)
    KEY_DEF("[1;7B",    "C-M-Down",     Key::Down,      KEY_MOD_CTRL | KEY_MOD_ALT)
    KEY_DEF("[1;7C",    "C-M-Right",    Key::Right,     KEY_MOD_CTRL | KEY_MOD_ALT)
    KEY_DEF("[1;7D",    "C-M-Left",     Key::Left,      KEY_MOD_CTRL | KEY_MOD_ALT)
    KEY_DEF("[1;7F",    "C-M-End",      Key::End,       KEY_MOD_CTRL | KEY_MOD_ALT)
    KEY_DEF("[1;7H",    "C-M-Home",     Key::Home,      KEY_MOD_CTRL | KEY_MOD_ALT)
    KEY_DEF("[2;7~",    "C-M-Ins",      Key::Insert,    KEY_MOD_CTRL | KEY_MOD_ALT)
    KEY_DEF("[3;7~",    "C-M-Del",      Key::Delete,    KEY_MOD_CTRL | KEY_MOD_ALT)
    KEY_DEF("",         "C-M-End",      0,              0)
    KEY_DEF("[5;7~",    "C-M-PgUp",     Key::PgUp,      KEY_MOD_CTRL | KEY_MOD_ALT)
    KEY_DEF("[6;7~",    "C-M-PdDown",   Key::PgDown,    KEY_MOD_CTRL | KEY_MOD_ALT)
    KEY_DEF("[1;7P",    "C-M-F1",       Key::F1,        KEY_MOD_CTRL | KEY_MOD_ALT)
    KEY_DEF("[1;7Q",    "C-M-F2",       Key::F2,        KEY_MOD_CTRL | KEY_MOD_ALT)
    KEY_DEF("[1;7R",    "C-M-F3",       Key::F3,        KEY_MOD_CTRL | KEY_MOD_ALT)
    KEY_DEF("[1;7S",    "C-M-F4",       Key::F4,        KEY_MOD_CTRL | KEY_MOD_ALT)
    KEY_DEF("[15;7~",   "C-M-F5",       Key::F5,        KEY_MOD_CTRL | KEY_MOD_ALT)
    KEY_DEF("[17;7~",   "C-M-F6",       Key::F6,        KEY_MOD_CTRL | KEY_MOD_ALT)
    KEY_DEF("[18;7~",   "C-M-F7",       Key::F7,        KEY_MOD_CTRL | KEY_MOD_ALT)
    KEY_DEF("[19;7~",   "C-M-F8",       Key::F8,        KEY_MOD_CTRL | KEY_MOD_ALT)
    KEY_DEF("[20;7~",   "C-M-F9",       Key::F9,        KEY_MOD_CTRL | KEY_MOD_ALT)
    KEY_DEF("[21;7~",   "C-M-F10",      Key::F10,       KEY_MOD_CTRL | KEY_MOD_ALT)
    KEY_DEF("[23;7~",   "C-M-F11",      Key::F11,       KEY_MOD_CTRL | KEY_MOD_ALT)
    KEY_DEF("[24;7~",   "C-M-F12",      Key::F12,       KEY_MOD_CTRL | KEY_MOD_ALT)
    KEY_DEF("[29;7~",   "C-M-Apps",     0,              0)
    KEY_DEF("[34;7~",   "C-M-Win",      0,              0)
    // + Shift + Alt
};

const CtrlMap ctrl_keys_map[] =
{
    // KEY_DEF( 0, "C-2", '2', KEY_MOD_CTRL)
    KEY_DEF( 1, "C-A", 'A', KEY_MOD_CTRL)
    KEY_DEF( 2, "C-B", 'B', KEY_MOD_CTRL)
    KEY_DEF( 3, "C-C", 'C', KEY_MOD_CTRL)
    KEY_DEF( 4, "C-D", 'D', KEY_MOD_CTRL)
    KEY_DEF( 5, "C-E", 'E', KEY_MOD_CTRL)
    KEY_DEF( 6, "C-F", 'F', KEY_MOD_CTRL)
    KEY_DEF( 7, "C-G", 'G', KEY_MOD_CTRL)
    KEY_DEF( 8, "C-H", 'H', KEY_MOD_CTRL)
    KEY_DEF( 9, "C-I", 'I', KEY_MOD_CTRL)
    KEY_DEF(10, "C-J", 'J', KEY_MOD_CTRL)
    KEY_DEF(11, "C-K", 'K', KEY_MOD_CTRL)
    KEY_DEF(12, "C-L", 'L', KEY_MOD_CTRL)
    KEY_DEF(13, "C-M", 'M', KEY_MOD_CTRL)
    KEY_DEF(14, "C-N", 'N', KEY_MOD_CTRL)
    KEY_DEF(15, "C-O", 'O', KEY_MOD_CTRL)
    KEY_DEF(16, "C-P", 'P', KEY_MOD_CTRL)
    KEY_DEF(17, "C-Q", 'Q', KEY_MOD_CTRL)
    KEY_DEF(18, "C-R", 'R', KEY_MOD_CTRL)
    KEY_DEF(19, "C-S", 'S', KEY_MOD_CTRL)
    KEY_DEF(20, "C-T", 'T', KEY_MOD_CTRL)
    KEY_DEF(21, "C-U", 'U', KEY_MOD_CTRL)
    KEY_DEF(22, "C-V", 'V', KEY_MOD_CTRL)
    KEY_DEF(23, "C-W", 'W', KEY_MOD_CTRL)
    KEY_DEF(24, "C-X", 'X', KEY_MOD_CTRL)
    KEY_DEF(25, "C-Y", 'Y', KEY_MOD_CTRL)
    KEY_DEF(26, "C-Z", 'Z', KEY_MOD_CTRL)
    // KEY_DEF(27, "C-3", '3', KEY_MOD_CTRL)
    // KEY_DEF(29, "C-5", '5', KEY_MOD_CTRL)
    // KEY_DEF(30, "C-6", '6', KEY_MOD_CTRL)
    // KEY_DEF(31, "C-7", '7', KEY_MOD_CTRL)
};

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

// -----------------------------------------------------------------------------

void decodeInputSeq(KeySequence &input, KeyCode &output)
{
    output = {};

    if (input.seqLen == 0)
        return;

    if (input.seqLen == 1)
    {
        const char c0 = input.keySeq[0];
        const char *name = "?";

        switch ((Ansi)c0)
        {
        case Ansi::BS:
        case Ansi::DEL: output.key = Key::Backspace; name = "Backspace"; break;
        case Ansi::HT:  output.key = Key::Tab;       name = "Tab"; break;
        case Ansi::LF:  output.key = Key::Enter;     name = "Enter"; break;
        case Ansi::ESC: output.key = Key::Esc;       name = "Esc"; break;
        default: break;
        }

        if (output.key != Key::None)
        {
            output.mod_all = KEY_MOD_SPECIAL;
            #if TWINS_USE_KEY_NAMES
            output.name = name;
            #endif
            return;
        }

        for (unsigned i = 0; i < arrSize(ctrl_keys_map); i++)
        {
            if (c0 == ctrl_keys_map[i].ctrl_code)
            {
                output.utf8[0] = (char)ctrl_keys_map[i].key;
                output.utf8[1] = '\0';
                output.mod_all = ctrl_keys_map[i].mod;
                #if TWINS_USE_KEY_NAMES
                output.name = ctrl_keys_map[i].name;
                #endif
                return;
            }
        }
    }

    if (input.keySeq[0] == (char)Ansi::ESC)
    {
        // TODO: use sorted list of keys and binary search
        for (unsigned i = 0; i < arrSize(esc_keys_map); i++)
        {
            int cmp = strcmp(input.keySeq + 1, esc_keys_map[i].seq);
            if (cmp == 0)
            {
                output.key = esc_keys_map[i].key;
                output.mod_all = esc_keys_map[i].mod | KEY_MOD_SPECIAL;
                #if TWINS_USE_KEY_NAMES
                output.name = esc_keys_map[i].name;
                #endif
                break;
            }
        }
    }
    else
    {
        // copy UTF-8 seq
        output.utf8[0] = input.keySeq[0];
        output.utf8[1] = input.keySeq[1];
        output.utf8[2] = input.keySeq[2];
        output.utf8[3] = input.keySeq[3];
        output.utf8[sizeof(output.utf8)-1] = '\0';
        #if TWINS_USE_KEY_NAMES
        output.name = output.utf8;
        #endif
    }
}

// -----------------------------------------------------------------------------

}