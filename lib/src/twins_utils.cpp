/******************************************************************************
 * @brief   TWins - utility code
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#include "twins_utils.hpp"

#include <string.h>

// -----------------------------------------------------------------------------

namespace twins
{

twins::Vector<twins::String> splitWords(const char *str, const char *delim)
{
    if (!str || !*str)
        return {};
    if (!delim || !*delim)
        return {};

    unsigned nwords = 1;

    twins::Vector<twins::String> out;
    size_t span = 0;

    const char *pstr = str;
    // skip whitespaces
    while ((span = strspn(pstr, delim)) > 0)
        pstr += span;

    // count words
    while ((span = strcspn(pstr, delim)) > 0)
    {
        nwords++;
        pstr += span;
        pstr += strspn(pstr, delim);
    }

    out.reserve(nwords);
    pstr = str;
    // skip whitespaces
    while ((span = strspn(pstr, delim)) > 0)
        pstr += span;

    // put each word into separate cell
    while ((span = strcspn(pstr, delim)) > 0)
    {
        twins::String s;
        s.appendLen(pstr, span);
        out.append(std::move(s));
        pstr += span;
        pstr += strspn(pstr, delim);
    }

    return out;
}

twins::String wordWrap(const char *str, uint16_t maxLineLen, const char *newLine)
{
    if (maxLineLen < 1)
        return {};
    if (!str || !*str)
        return {};
    if (!newLine || !*newLine)
        return {};

    auto words = splitWords(str);
    unsigned line_len = 0;
    twins::String out;

    for (const auto &s : words)
    {
        auto s_len = s.u8len(true);

        if (s_len > maxLineLen)
        {
            if (line_len > 0)
                out << newLine;

            // TODO: this may break ESC sequence
            out.appendLen(s.cstr(), maxLineLen-1);
            out << "…" << newLine;
            line_len = 0;
        }
        else
        {
            if (line_len + s_len <= maxLineLen)
            {
                out << s;
                line_len += s_len;
            }
            else
            {
                if (out.size() > 0)
                    out << newLine;

                out << s;
                line_len = s_len;
            }

            if (line_len < maxLineLen)
            {
                out << " ";
                line_len++;
            }
        }
    }

    return out;
}

// -----------------------------------------------------------------------------

}
