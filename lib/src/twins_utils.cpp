/******************************************************************************
 * @brief   TWins - utility code
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#include "twins_utils.hpp"

#include <string.h>

// -----------------------------------------------------------------------------

namespace twins::util
{

Vector<StringRange> splitWords(const char *str, const char *delim)
{
    if (!str || !*str)
        return {};
    if (!delim || !*delim)
        return {};

    unsigned nwords = 1;

    Vector<StringRange> out;
    unsigned span = 0;

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
        out.append(StringRange{pstr, span});
        pstr += span;
        pstr += strspn(pstr, delim);
    }

    return out;
}

Vector<String> splitWordsCpy(const char *str, const char *delim)
{
    if (!str || !*str)
        return {};
    if (!delim || !*delim)
        return {};

    unsigned nwords = 1;

    Vector<String> out;
    unsigned span = 0;

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
        String s;
        s.appendLen(pstr, span);
        out.append(std::move(s));
        pstr += span;
        pstr += strspn(pstr, delim);
    }

    return out;
}

String wordWrap(const char *str, uint16_t maxLineLen, const char *newLine)
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
    out.reserve(strlen(str));

    for (const auto &s : words)
    {
        auto s_len = twins::String::u8len(s.data, s.data + s.size, true);

        if (s_len > maxLineLen)
        {
            if (line_len > 0)
                out << newLine;

            // TODO: this may break ESC sequence; splitWords should also split ESC sequences
            out.appendLen(s.data, maxLineLen-1);
            out << "…" << newLine;
            line_len = 0;
        }
        else
        {
            if (line_len + s_len <= maxLineLen)
            {
                out.appendLen(s.data, s.size);
                line_len += s_len;
            }
            else
            {
                if (out.size() > 0)
                    out << newLine;

                out.appendLen(s.data, s.size);
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

Vector<StringRange> splitLines(const char *str)
{
    if (!str || !*str)
        return {};

    Vector<StringRange> out;

    while (const char *nl = strchr(str, '\n'))
    {
        // possible and allowed: nl == str
        out.append(StringRange{str, (unsigned)(nl-str)});
        str = nl+1;
    }

    if (*str)
        out.append(StringRange{str, (unsigned)strlen(str)});

    return out;
}

// -----------------------------------------------------------------------------

}
