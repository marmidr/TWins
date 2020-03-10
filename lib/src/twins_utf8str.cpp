/*
 * Copyright (c) 2007 Thomas J. Merritt
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain this noticein its entirety,
 *    including the above copyright notice, this list of conditions and the
 *    following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of any
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 * 4. Any distribution that includes this software must be registered at
 *    <http://www.merritts.org/utf8str/>.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

/* UTF-8 string functions */
/* Generated strings are always nul terminated with proper UTF-8 encoding */
/* of characters outside of the 1..127 range and never include a BOM. */
/* Destination strings are always specified with a length to avoid */
/* buffer overflow problems. */


/* --------- Changes in library ---------
 * because PIC33 is 16bit architecture, each usages of type int has to be changed
 * to type long int.
 *
 * moreover, every numeric constant in macros now has L suffix making the constant
 * explicit 32-bit value.
 *
 * Mariusz Midor  16/02/2011
 * egolhmi.eu
 *
 */


/*
 * UTF-8 info
 *
 *    0XXX XXXX                     00000 - 0007F
 *    110X XXXX 10XX XXXX               00080 - 007FF
 *    1110 XXXX 10XX XXXX 10XX XXXX         00800 - 0FFFF
 *    1111 0XXX 10XX XXXX 10XX XXXX 10XX XXXX       10000 - 3FFFF
 *    1111 10XX 10XX XXXX 10XX XXXX 10XX XXXX 10XX XXXX invalid
 *    1111 11XX                     invalid
 *
 *          XXX XXXX        00000-0007F
 *         XXX XXXX XXXX        00080-007FF
 *       XXXX XXXX XXXX XXXX        00800-0FFFF
 *    X XXXX XXXX XXXX XXXX XXXX        10000-3FFFF
 */

#include <stddef.h>


#define UTF8_SEQ_LENGTH(src)            \
    ((src[0] & 0x80) == 0 ? 1 :         \
     (((src[0] & 0xFF) >= 0xC2 &&       \
        (src[0] & 0xFF) <= 0xDF) ? 2 :  \
     ((src[0] & 0xF0) == 0xE0 ? 3 :     \
     ((src[0] & 0xFF) == 0xF0 ? 4 : 0))))


int utf8seqlen(char const *str)
{
    if (str == NULL)
        return 0;

    return UTF8_SEQ_LENGTH(str);
}

int utf8len(char const *str)
{
    if (str == NULL)
        return 0;

    int len = 0;

    while (*str)
    {
        int sl = UTF8_SEQ_LENGTH(str);

        if (sl)
        {
            len++;
            str += sl;
        }
        else
        {
            return len;
        }
    }

    return len;
}

int utf8nlen(char const *str, int strsize)
{
    int len = 0;

    if (str == NULL)
        return 0;

    while (*str && strsize > 0)
    {
        int sl = UTF8_SEQ_LENGTH(str);

        if (sl)
        {
            len++;
            str += sl;
            strsize -= sl;
        }
        else
        {
            return len;
        }
    }

    return len;
}
