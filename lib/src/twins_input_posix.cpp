/******************************************************************************
 * @brief   TWins - keyboard input - POSIX
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 * @note    Most of this code comes from http://0x80.pl/articles/terminals.html
 *****************************************************************************/

#include "twins_input_posix.hpp"
#include "twins_common.hpp"

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

// -----------------------------------------------------------------------------

namespace twins
{

static int         ttyFileNo;
static char        termEOFcode;
static termios     termIOs;
static uint16_t    termKeyTimeoutMs;

// -----------------------------------------------------------------------------

static void readKey(AnsiSequence &seq)
{
    // read up to 8 bytes
    int nb = read(ttyFileNo, seq.data, sizeof(seq.data)-1);
    if (nb == -1) nb = 0;
    seq.len = nb;
    seq.data[seq.len] = '\0';
    errno = 0;
}

static void waitForKey(AnsiSequence &seq)
{
    fd_set read_set;
    FD_ZERO(&read_set);
    FD_SET(ttyFileNo, &read_set);

    // wait for key
    timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = termKeyTimeoutMs * 1000;
    int rc = select(ttyFileNo + 1, &read_set, NULL, NULL, &tv);

    if (rc > 0)
        readKey(seq);
}

static void checkErrNo(int line)
{
    if (errno)
    {
        printf("Line:%u ", line);
        perror(strerror(errno));
        exit(EXIT_FAILURE);
    }
}

// -----------------------------------------------------------------------------

void inputPosixInit(uint16_t timeoutMs)
{
    termKeyTimeoutMs = timeoutMs;
    errno = 0;
    ttyFileNo = open("/dev/tty", O_RDONLY | O_NONBLOCK);
    checkErrNo(__LINE__);

    // get terminal configuration
    tcgetattr(ttyFileNo, &termIOs);
    checkErrNo(__LINE__);
    termEOFcode = termIOs.c_cc[VEOF];

    // disable canonical mode and echo
    termIOs.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(ttyFileNo, TCSAFLUSH, &termIOs);
    checkErrNo(__LINE__);
}

void inputPosixFree()
{
    // restore terminal configuration
    termIOs.c_lflag |= (ICANON | ECHO);
    tcsetattr(ttyFileNo, TCSAFLUSH, &termIOs);
    checkErrNo(__LINE__);
    close(ttyFileNo);
}

void inputPosixRead(AnsiSequence &output, bool &quitRequested)
{
    output.len = 0;
    waitForKey(output);

    if (output.len == 1 && output.data[0] == termEOFcode)
    {
        quitRequested = true;
        return;
    }
}

// -----------------------------------------------------------------------------

}
