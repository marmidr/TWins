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

const char* strnchr(const char *str, int strSz, char c)
{
    while (strSz-- > 0)
    {
        if (*str == c) return str;
        str++;
    }

    return nullptr;
}

Vector<StringRange> splitWords(const char *str, const char *delim, bool storeDelim)
{
    if (!str || !*str)
        return {};
    if (!delim || !*delim)
        return {};

    unsigned n_words = 1;

    Vector<StringRange> out;
    unsigned span = 0;

    const char *pstr = str;
    // skip whitespaces
    while ((span = strspn(pstr, delim)) > 0)
        pstr += span;

    // count words
    while ((span = strcspn(pstr, delim)) > 0)
    {
        n_words++;
        pstr += span;
        pstr += strspn(pstr, delim);
    }

    // prepare output
    if (storeDelim) n_words *= 2;
    out.reserve(n_words);
    pstr = str;

    // skip whitespaces
    while ((span = strspn(pstr, delim)) > 0)
        pstr += span;

    if (storeDelim && (pstr > str))
        out.append(StringRange{str, unsigned(pstr - str)});

    // put each word into separate cell
    while ((span = strcspn(pstr, delim)) > 0)
    {
        if (const char *pesc = strnchr(pstr, span, '\e'))
        {
            // span contains ESC sequence - store it separately

            // append text before ESC
            if (pesc > pstr)
                out.append(StringRange{pstr, unsigned(pesc - pstr)});
            auto esc_len = String::escLen(pesc);

            // append ESC seq
            out.append(StringRange{pesc, esc_len});

            // append text after ESC
            pesc += esc_len;
            if (pesc < pstr + span)
                out.append(StringRange{pesc, unsigned(pstr + span - pesc)});
        }
        else
        {
            // store word
            out.append(StringRange{pstr, span});
        }

        pstr += span;
        // check delimiters length
        span = strspn(pstr, delim);
        // store delimiter and skip it
        if (storeDelim) out.append(StringRange{pstr, span});
        pstr += span;
    }

    return out;
}

String wordWrap(const char *str, uint16_t maxLineLen, const char *delim, const char *separator)
{
    if (maxLineLen < 1)
        return {};
    if (!str || !*str)
        return {};
    if (!delim || !*delim)
        return {};
    if (!separator || !*separator)
        return {};

    auto words = splitWords(str, delim, true);
    unsigned line_len = 0;
    String out;
    out.reserve(strlen(str) + words.size());

    for (auto &w : words)
    {
        // w is a ESC sequence
        if (*w.data == '\e')
        {
            // append entire sequence and treat it as zero-length
            out.appendLen(w.data, w.size);
            continue;
        }

        // w is a delimiter char or sequence
        if (strchr(delim, *w.data))
        {
            // check if w contains \n and adjust line length accordingly
            if (const char *nl = strnchr(w.data, w.size, '\n'))
            {
                out.appendLen(w.data, w.size);
                line_len = w.size - (nl - w.data);
                continue;
            }

            if (line_len > 0)
            {
                out.appendLen(w.data, w.size);
                line_len += w.size;
            }
            continue;
        }

        auto word_len = twins::String::u8len(w.data, w.data + w.size, true);

        if (line_len + word_len <= maxLineLen)
        {
            out.appendLen(w.data, w.size);
            line_len += word_len;
        }
        else
        {
            if (word_len > maxLineLen)
            {
                if (line_len > 0)
                    out << separator;

                out.appendLen(w.data, maxLineLen-1);
                out << "…" << separator;
                line_len = 0;
                continue;
            }

            if (out.size() > 0)
                out << separator;

            out.appendLen(w.data, w.size);
            line_len = word_len;
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
