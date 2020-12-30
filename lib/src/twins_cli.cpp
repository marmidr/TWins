/******************************************************************************
 * @brief   TWins - command line interface
 * @author  Mariusz Midor
 *          https://bitbucket.org/marmidr/twins
 *****************************************************************************/

#include "twins.hpp"
#include "twins_cli.hpp"
#include "twins_ringbuffer.hpp"
#include "twins_queue.hpp"

#include <string.h>

// -----------------------------------------------------------------------------

namespace twins::cli
{

struct CliState
{
    String      lineBuff;
    StringBuff  cmd;
    History     history;
    int16_t     cursorPos = 0;
    int16_t     historyIdx = 0;
    RingBuff<char> ringBuff;
    Queue<String>  cmds;
};

// trick to avoid automatic variable creation/destruction causing calls to uninitialized PAL
static char cs_buff alignas(CliState) [sizeof(CliState)];
CliState& g_cs = (CliState&)cs_buff;

// global variables
bool verbose = true;
bool echoNlAfterCr = false;

// -----------------------------------------------------------------------------

void init(void)
{
    new (&g_cs) CliState{};
}

void deInit(void)
{
    g_cs.~CliState();
}

// -----------------------------------------------------------------------------

void reset(void)
{
    g_cs.ringBuff.clear();
    g_cs.cmd.clear();
    g_cs.lineBuff.clear();
    g_cs.cursorPos = 0;
    g_cs.history.clear();
    g_cs.historyIdx = 0;
}

void processInput(const char* data, uint8_t dataLen)
{
    if (!data)
        data = "";

    if (!dataLen)
        dataLen = strlen(data);

    if (g_cs.ringBuff.capacity() == 0)
        g_cs.ringBuff.init(ESC_SEQ_MAX_LENGTH+2);

    while (dataLen)
    {
        uint8_t to_write = dataLen > ESC_SEQ_MAX_LENGTH ? ESC_SEQ_MAX_LENGTH : dataLen;

        g_cs.ringBuff.write(data, to_write);
        processInput(g_cs.ringBuff);

        data += to_write;
        dataLen -= to_write;
    }
}

void processInput(twins::RingBuff<char> &rb)
{
    char seq[ESC_SEQ_MAX_LENGTH];

    while (true)
    {
        rb.copy(seq, sizeof(seq));

        KeyCode kc = {};
        uint8_t seq_sz = decodeInputSeq(rb, kc);

        if (kc.key == Key::None)
            break;

        seq[seq_sz] = '\0';
        const char *p_seq = seq; // echo decoded sequence

        if (kc.m_spec || kc.m_ctrl)
        {
            // fprintf(stderr, ESC_FG_RED "^" ESC_FG_DEFAULT); fflush(stderr);

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

                    moveBy(-(int16_t)g_cs.lineBuff.u8len(), 0);
                    writeStr(ESC_LINE_ERASE_RIGHT);
                    g_cs.lineBuff = g_cs.history[g_cs.historyIdx];
                    g_cs.cursorPos = g_cs.lineBuff.u8len();
                    writeStrLen(g_cs.lineBuff.cstr(), g_cs.lineBuff.size());
                }
                p_seq = nullptr; // suppress echo
                break;
            case Key::Left:
                if (g_cs.cursorPos > 0)
                    g_cs.cursorPos--;
                else
                    p_seq = nullptr; // suppress echo
                break;
            case Key::Right:
                if (g_cs.cursorPos < (signed)g_cs.lineBuff.u8len())
                    g_cs.cursorPos++;
                else
                    p_seq = nullptr; // suppress echo
                break;
            case Key::Home:
                moveBy(-g_cs.cursorPos, 0);
                g_cs.cursorPos = 0;
                p_seq = nullptr; // suppress echo
                break;
            case Key::End:
                moveBy(-g_cs.cursorPos, 0);
                g_cs.cursorPos = g_cs.lineBuff.u8len();
                moveBy(g_cs.cursorPos, 0);
                p_seq = nullptr; // suppress echo
                break;
            case Key::Delete:
                if (g_cs.cursorPos >= 0)
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
                p_seq = nullptr; // suppress echo
                break;
            case Key::Backspace:
                if (g_cs.cursorPos > 0)
                {
                    if (kc.m_ctrl)
                    {
                        g_cs.lineBuff.erase(0, g_cs.cursorPos);
                        moveBy(-g_cs.cursorPos, 0);
                        writeStrFmt(ESC_CHAR_DELETE_FMT, g_cs.cursorPos);
                        g_cs.cursorPos = 0;
                    }
                    else
                    {
                        g_cs.lineBuff.erase(g_cs.cursorPos-1);
                        g_cs.cursorPos--;

                        moveBy(-1, 0);
                        writeStr(ESC_CHAR_DELETE(1));
                    }
                }
                p_seq = nullptr; // suppress echo
                break;
            case Key::Tab:
                // auto complete
                p_seq = nullptr; // suppress echo
                break;
            case Key::Enter:
                if (g_cs.lineBuff.size())
                {
                    if (echoNlAfterCr) twins::writeChar('\n');

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

                    g_cs.historyIdx = g_cs.history.size();
                    g_cs.cmds.push(std::move(g_cs.lineBuff));
                    g_cs.lineBuff.clear();
                }
                else
                {
                    prompt(true);
                    p_seq = nullptr; // suppress echo
                }
                g_cs.cursorPos = 0;
                break;
            default:
                break;
            }

            // echo
            if (p_seq)
                writeStrLen(p_seq, seq_sz);
        }
        else
        {
            // append/insert
            if (g_cs.lineBuff.u8len() < 100)
            {
                g_cs.lineBuff.insert(g_cs.cursorPos, kc.utf8);
                g_cs.cursorPos += 1;
                // echo
                writeStr(ESC_CHAR_INSERT(1));
                writeStrLen(p_seq, seq_sz);
            }
            else
            {
                // line length limit exceeded
                writeStr(ESC_BELL);
            }
        }
    }

    flushBuffer();
}

History& getHistory(void)
{
    return g_cs.history;
}

void printHelp(const Cmd* pCommands)
{
    writeStr(ESC_BOLD "help" ESC_NORMAL "\r\n" "    this help" "\r\n");
    writeStr(ESC_BOLD "hist" ESC_NORMAL "\r\n" "    commands history" "\r\n");

    while (pCommands->name)
    {
        // default cmd has empty name
        if (pCommands->name[0] != '\0')
        {
            writeStr(ESC_BOLD);
            writeStr(pCommands->name);
            writeStr(ESC_NORMAL " ");
            writeStr(pCommands->help);
            writeStr("\r\n");
            flushBuffer();
        }
        pCommands++;
    }
}

void printHistory()
{
    int i = 1;
    for (const auto &s : g_cs.history)
        writeStrFmt("%2d. %s\r\n", i++, s.cstr());
    flushBuffer();
}

void tokenize(StringBuff &cmd, Argv &argv)
{
    char *p = cmd.data();
    // fputs(">>", stderr);
    // fputs(p, stderr);
    // fputs("<<\n", stderr);

    // skip leading spaces
    while (*p == ' ')
        p++;

    while (*p)
    {
        // support for quoted arguments, eg: `cmd -n "Bob Walker"`
        if (*p == '"')
        {
            p++;
            argv.append(p);

            while (*p && *p != '"')
                p++;

            if (!*p) // end of command line ?
                break;

            // erase closing quote
            *p = '\0';
            p++;
        }
        else
        {
            argv.append(p);
        }

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

    if (verbose)
    {
        // debug:
        writeStr(ESC_ITALICS_ON ESC_FG_BLACK_INTENSE "Command: ");
        for (const char *a : argv)
            writeStrFmt("\'%s\' ", a);
        writeStr(ESC_ITALICS_OFF ESC_FG_DEFAULT "\r\n");
        flushBuffer();
    }
}

const Cmd* find(const Cmd* pCommands, Argv &argv)
{
    const Cmd* p_dflt = nullptr;

    if (argv.size())
    {
        const char *cmd_name = argv[0];

        for (; pCommands->name; pCommands++)
        {
            // name
            // name|alias
            const char *name = pCommands->name;

            if (*name == '\0')
            {
                p_dflt = pCommands;
                continue;
            }

            const char *ename = strchr(name, '|');
            if (!ename)
                ename = name + strlen(name);

            if (strncmp(name, cmd_name, ename-name) == 0)
                return pCommands;

            if (*ename == '|')
            {
                name = ename+1;
                if (strcmp(name, cmd_name) == 0)
                    return pCommands;
            }
        }
    }

    return p_dflt ? p_dflt : nullptr;
}

void prompt(bool newLn)
{
    if (newLn) writeStr("\r\n");
    writeStr(ESC_FG_GREEN_INTENSE "> " ESC_FG_WHITE_INTENSE);
}

bool checkAndExec(const Cmd* pCommands)
{
    assert(pCommands);
    if (g_cs.cmds.size() == 0)
        return false;

    g_cs.cmd = std::move(*g_cs.cmds.pop());

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

    if (g_cs.cmd == "hist")
    {
        printHistory();
        prompt(false);
        flushBuffer();
        g_cs.cmd.clear();
        return true;
    }

    Argv argv;
    tokenize(g_cs.cmd, argv);
    bool found = false;

    if (const auto *p_cmd = find(pCommands, argv))
    {
        found = true;
        p_cmd->handler(argv);
    }
    else
    {
        writeStr("unknown command" "\r\n");
    }

    prompt(false); // TODO: only if command handler finished it's execution;
                   // handler may call async job and later tell CLI the job is done
    flushBuffer();
    g_cs.cmd.clear();
    return found;
}

bool execLine(const char *cmdline, const Cmd* pCommands)
{
    assert(cmdline);
    assert(pCommands);

    Argv argv;
    StringBuff cmd(cmdline);
    tokenize(cmd, argv);
    bool found = false;

    if (const auto *p_cmd = find(pCommands, argv))
    {
        found = true;
        p_cmd->handler(argv);
    }
    else
    {
        writeStr("unknown command" "\r\n");
    }

    prompt(false);
    flushBuffer();
    return found;
}

// -----------------------------------------------------------------------------

}
