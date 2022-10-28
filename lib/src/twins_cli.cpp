/******************************************************************************
 * @brief   TWins - command line interface
 * @author  Mariusz Midor
 *          https://bitbucket.org/marmidr/twins
 *          https://github.com/marmidr/twins
 *****************************************************************************/

#include "twins.hpp"
#include "twins_cli.hpp"
#include "twins_ringbuffer.hpp"
#include "twins_queue.hpp"

#include <string.h>

// -----------------------------------------------------------------------------

#ifndef TWINS_CLI_MAXCMDLEN
# define TWINS_CLI_MAXCMDLEN    120
#endif

#ifndef TWINS_CLI_MAXHIST
# define TWINS_CLI_MAXHIST      20
#endif

static_assert(TWINS_CLI_MAXCMDLEN > 5);
static_assert(TWINS_CLI_MAXHIST > 0);

#define PASSWORD    "35786"

namespace twins::cli
{

struct CliState
{
    String          lineBuff;
    History         history;
    int16_t         cursorPos = 0;
    int16_t         historyIdx = 0;
    RingBuff<char>  seqRingBuff;
    Queue<String>   cmdQue;
    CmdHandler      overrideHandler;
};

// trick to avoid automatic variable creation/destruction causing calls to uninitialized PAL
static char cs_buff alignas(CliState) [sizeof(CliState)];
CliState& g_cs = (CliState&)cs_buff;

// global variables
bool verbose = true;
bool echoNlAfterCr = false;
bool cliEnabled = true;

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
    g_cs.seqRingBuff.clear();
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

    if (g_cs.seqRingBuff.capacity() == 0)
        g_cs.seqRingBuff.init(ESC_SEQ_MAX_LENGTH+2);

    while (dataLen)
    {
        uint8_t to_write = dataLen > ESC_SEQ_MAX_LENGTH ? ESC_SEQ_MAX_LENGTH : dataLen;

        g_cs.seqRingBuff.write(data, to_write);
        processInput(g_cs.seqRingBuff);

        data += to_write;
        dataLen -= to_write;
    }
}

void processInput(twins::RingBuff<char> &rb)
{
    char seq[ESC_SEQ_MAX_LENGTH];

    while (true)
    {
        if(cliEnabled)
            rb.copy(seq, sizeof(seq));
        else
        {
            for(uint16_t i = 0; i<rb.size(); i++)
            {
                seq[i] = '*';
                if(i >= ESC_SEQ_MAX_LENGTH)
                    break;
            }
        }

        KeyCode kc = {};
        uint8_t seq_sz = decodeInputSeq(rb, kc);

        if (kc.key == Key::None)
            break;

        if (seq_sz >= ESC_SEQ_MAX_LENGTH)
            seq_sz = ESC_SEQ_MAX_LENGTH-1;

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

                    // append to history, limit history size
                    int idx = 0;
                    if(cliEnabled)//prevents password to be stored in history
                    {
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
                            if (g_cs.history.size() > TWINS_CLI_MAXHIST)
                                g_cs.history.remove(0, true);
                        }
                    }
                    g_cs.historyIdx = g_cs.history.size();
                    g_cs.cmdQue.write(std::move(g_cs.lineBuff));
                    g_cs.lineBuff.clear();
                }
                else
                {
                    prompt(true);
                }
                p_seq = nullptr; // suppress echo
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
            if (g_cs.lineBuff.size() < TWINS_CLI_MAXCMDLEN)
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
//     fputs(">>", stderr);
//     fputs(p, stderr);
//     fputs("<<\n", stderr);

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

bool checkAndExec(const Cmd* pCommands, bool soleOrLastCommandsSet)
{
    assert(pCommands);
    if (g_cs.cmdQue.size() == 0)
        return false;

    // drop empty command string
    if (g_cs.cmdQue.front()->size() == 0)
    {
        g_cs.cmdQue.read();
        return false;
    }

    if (g_cs.overrideHandler)
        soleOrLastCommandsSet = true;

    StringBuff cmd;
    if (soleOrLastCommandsSet)
        cmd = g_cs.cmdQue.read();
    else
        cmd = *g_cs.cmdQue.front();

    writeStr(ESC_FG_DEFAULT);

    if (!g_cs.overrideHandler)
    {
        if(not cliEnabled)
        {
            if (cmd == PASSWORD)
            {
                cliEnabled = true;
                writeStr(ESC_FG_GREEN_INTENSE);
                writeStr("Access granted." "\r\n");
                writeStr(ESC_FG_DEFAULT);
                writeStr("CLI interface ready; type 'help' for available commands." "\r\n");
                prompt(false);
                flushBuffer();
                g_cs.cmdQue.read();
                return true;
            }
            else
            {
                writeStr(ESC_FG_RED_INTENSE);
                writeStr("Incorrect password, access denied." "\r\n");
                writeStr(ESC_FG_DEFAULT);
                prompt(false);
                flushBuffer();
                g_cs.cmdQue.read();
                return true;
            }
        }

        if (cmd == "help")
        {
            printHelp(pCommands);
            prompt(false);
            flushBuffer();
            g_cs.cmdQue.read();
            return true;
        }

        if (cmd == "hist")
        {
            printHistory();
            prompt(false);
            flushBuffer();
            g_cs.cmdQue.read();
            return true;
        }
    }

    Argv argv;
    tokenize(cmd, argv);
    bool found = false;

    if (g_cs.overrideHandler)
    {
        g_cs.overrideHandler(argv);
        found = true;
    }
    else if (const auto *p_cmd = find(pCommands, argv))
    {
        p_cmd->handler(argv);
        found = true;
        if (!soleOrLastCommandsSet) g_cs.cmdQue.read();
    }
    else
    {
        if (soleOrLastCommandsSet)
            writeStr("unknown command" "\r\n");
    }

    if (soleOrLastCommandsSet)
        prompt(false);

    flushBuffer();
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

void setOverrideHandler(CmdHandler handler)
{
    g_cs.overrideHandler = std::move(handler);
}

// -----------------------------------------------------------------------------

}
