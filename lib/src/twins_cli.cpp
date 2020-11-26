/******************************************************************************
 * @brief   TWins - command line interface
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#include "twins.hpp"
#include "twins_cli.hpp"
#include "twins_ringbuffer.hpp"

#include <string.h>

// -----------------------------------------------------------------------------

namespace twins::cli
{

using Argv = Vector<const char*>;


class StringBuff : public String
{
public:
    StringBuff& operator =(String &&other)
    {
        String::operator=(std::move(other));
        return *this;
    }

    char *data()
    {
        if (!mpBuff) append("");
        return mpBuff;
    }
};


struct CliState
{
    String      lineBuff;
    StringBuff  cmd;
    History     history;
    int16_t     cursorPos = 0;
    int16_t     historyIdx = 0;
    RingBuff<char> keybInput;
};

// trick to avoid automatic variable creation/destruction causing calls to uninitialized PAL
static char cs_buff alignas(CliState) [sizeof(CliState)];
CliState& g_cs = (CliState&)cs_buff;

// -----------------------------------------------------------------------------

void init(void)
{
    new (&g_cs) CliState{};
    g_cs.keybInput.init(20);
}

void deInit(void)
{
    g_cs.~CliState();
}

// -----------------------------------------------------------------------------

void reset(void)
{
    g_cs.keybInput.clear();
    g_cs.cmd.clear();
    g_cs.lineBuff.clear();
    g_cs.cursorPos = 0;
    g_cs.history.clear();
    g_cs.historyIdx = 0;
}

void write(const char* str, uint8_t len)
{
    if (!len) len = strlen(str);
    if (!len) return;
    g_cs.keybInput.write(str, len);

    while (true)
    {
        KeyCode kc = {};

        auto processen_len = g_cs.keybInput.size();
        decodeInputSeq(g_cs.keybInput, kc);
        processen_len = processen_len - g_cs.keybInput.size();

        if (kc.key == Key::None)
            break;

        if (kc.m_spec)
        {
            switch (kc.key)
            {
            case Key::Up:
            case Key::Down:
                if (g_cs.history.size())
                {
                    g_cs.historyIdx += kc.key == Key::Up ? -1 : 1;

                    if (g_cs.historyIdx < 0)
                        g_cs.historyIdx = 0;
                    else if (g_cs.historyIdx >= (int)g_cs.history.size())
                        g_cs.historyIdx = g_cs.history.size()-1;

                    moveBy(-g_cs.lineBuff.u8len(), 0);
                    writeStr(ESC_LINE_ERASE_RIGHT);
                    g_cs.lineBuff = g_cs.history[g_cs.historyIdx];
                    g_cs.cursorPos = g_cs.lineBuff.u8len();
                    writeStrLen(g_cs.lineBuff.cstr(), g_cs.lineBuff.size());
                }
                str = nullptr; // suppress echo
                break;
            case Key::Left:
                if (g_cs.cursorPos > 0)
                    g_cs.cursorPos--;
                else
                    str = nullptr; // suppress echo
                break;
            case Key::Right:
                if (g_cs.cursorPos < (signed)g_cs.lineBuff.u8len())
                    g_cs.cursorPos++;
                else
                    str = nullptr; // suppress echo
                break;
            case Key::Home:
                moveBy(-g_cs.cursorPos, 0);
                g_cs.cursorPos = 0;
                str = nullptr; // suppress echo
                break;
            case Key::End:
                moveBy(-g_cs.cursorPos, 0);
                g_cs.cursorPos = g_cs.lineBuff.u8len();
                moveBy(g_cs.cursorPos, 0);
                str = nullptr; // suppress echo
                break;
            case Key::Delete:
                if (g_cs.cursorPos > 0)
                {
                    if (kc.m_ctrl)
                    {
                        g_cs.lineBuff.trim(g_cs.cursorPos);
                        writeStr(ESC_LINE_ERASE_RIGHT);
                    }
                    else
                    {
                        g_cs.lineBuff.erase(g_cs.cursorPos);
                        writeStr(ESC_CHAR_DELETE(1));
                    }
                }
                str = nullptr; // suppress echo
                break;
            case Key::Backspace:
                if (g_cs.cursorPos > 0)
                {
                    if (kc.m_ctrl)
                    {
                        g_cs.lineBuff.erase(0, g_cs.cursorPos);
                        g_cs.cursorPos = 0;
                    }
                    else
                    {
                        g_cs.lineBuff.erase(g_cs.cursorPos-1);
                        g_cs.cursorPos--;
                    }

                    moveBy(-1, 0);
                    writeStr(ESC_CHAR_DELETE(1));
                }
                str = nullptr; // suppress echo
                break;
            case Key::Tab:
                // auto complete
                str = nullptr; // suppress echo
                break;
            case Key::Enter:
                if (g_cs.lineBuff.size())
                {
                    // append to history, limit history to 20
                    int idx = 0;

                    if (auto *str = g_cs.history.find(g_cs.lineBuff, &idx))
                    {
                        // move to top
                        String tmp = std::move(*str);
                        g_cs.history.remove(idx, true);
                        g_cs.history.append(std::move(tmp));
                    }
                    else
                    {
                        g_cs.history.append(g_cs.lineBuff);
                        if (g_cs.history.size() > 20)
                            g_cs.history.remove(0, true);
                    }

                    // cmd X \r cmd Y \r cmd Z...
                    // TODO: move only part before '\r'
                    g_cs.historyIdx = g_cs.history.size();
                    g_cs.cmd = std::move(g_cs.lineBuff);
                    g_cs.lineBuff.clear();
                }
                else
                {
                    prompt(true);
                    str = nullptr; // suppress echo
                }
                g_cs.cursorPos = 0;
                break;
            default:
                break;
            }

            if (str) writeStrLen(str, len);
        }
        else
        {
            // append/insert
            g_cs.lineBuff.insert(g_cs.cursorPos, kc.utf8);
            g_cs.cursorPos += 1;
            // echo
            writeStr(ESC_CHAR_INSERT(1));
            writeStrLen(str, len);
        }
    }

    // writeStrFmt(" cur:%d ", g_cs.cursorPos);
    flushBuffer();
}

History& getHistory(void)
{
    return g_cs.history;
}

void printHelp(const Cmd* pCommands)
{
    while (pCommands->name)
    {
        writeStr(ESC_BOLD);
        writeStr(pCommands->name);
        writeStr(ESC_NORMAL " ");
        writeStr(pCommands->help);
        writeStr("\r\n");
        flushBuffer();
        pCommands++;
    }
}

void tokenize(Argv &argv)
{
    char *p = g_cs.cmd.data();

    // skip leading spaces
    while (*p == ' ')
        p++;

    while (*p)
    {
        argv.append(p);

        // search for separator
        while (*p && *p != ' ')
            p++;

        if (!*p) // end of command line ?
            break;

        *p = '\0'; // mark end of argument
        p++;

        // skip extra spaces
        while (*p == ' ')
            p++;
    }

    // debug:
    writeStr(ESC_ITALICS_ON ESC_FG_BLACK_INTENSE "Command: ");
    for (const char *a : argv)
        writeStrFmt("\'%s\' ", a);
    writeStr(ESC_ITALICS_OFF ESC_FG_DEFAULT "\r\n");
    flushBuffer();
}

const Cmd* find(const Cmd* pCommands, Argv &argv)
{
    if (argv.size())
    {
        const char *cmd = argv[0];

        for (; pCommands->name; pCommands++)
            if (strcmp(pCommands->name, cmd) == 0)
                return pCommands;
    }

    return nullptr;
}

void exec(const Cmd &cmd, Argv &argv)
{
    cmd.handler(argv.size(), argv.data());
}

void prompt(bool newln)
{
    if (newln) writeStr("\r\n");
    writeStr(ESC_FG_GREEN_INTENSE "> " ESC_FG_WHITE_INTENSE);
}

bool checkAndExec(const Cmd* pCommands)
{
    if (!g_cs.cmd.size())
        return false;

    writeStr(ESC_FG_DEFAULT);

    if (g_cs.cmd == "help")
    {
        printHelp(pCommands);
        prompt(false);
        flushBuffer();
        g_cs.cmd.clear();
        return true;
    }

    Argv argv;
    tokenize(argv);

    if (const auto *pcmd = find(pCommands, argv))
        exec(*pcmd, argv);
    else
        writeStr("unknown command" "\r\n");

    prompt(false);
    flushBuffer();
    g_cs.cmd.clear();
    return true;
}

// -----------------------------------------------------------------------------

}
