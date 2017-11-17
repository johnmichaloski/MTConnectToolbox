#ifndef HEXDUMP_H
#define HEXDUMP_H

// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied
// or intended.
#include <string>
#include <stdarg.h>

namespace Nist
{
inline std::string StrFormat (const char *format, ...)
{
    va_list ap;

    va_start(ap, format);

    int         m;
    int         n = strlen(format) + 1028;
    std::string tmp(n, '0');

    // Kind of a bogus way to insure that we don't
    // exceed the limit of our buffer
    while ( ( m = vsnprintf(&tmp[0], n - 1, format, ap) ) < 0 )
    {
        n = n + 1028;
        tmp.resize(n, '0');
    }

    va_end(ap);
    return tmp.substr(0, m);
}
/**
Do a hex dump at location given by a memory  address  given a buffer size and
line length (default 16).
Displays the input address in hexadecimal, followed by 16 space-separated,
hexadecimal bytes, followed by the same 16 bytes in Ascii format.

@example    StringEx::HexDump(addr, numbytes);
*/
inline std::string HexDump (void *memory, int BytesToDump,
                            int BytesPerLine = 16)
{
    std::string   dump;
    unsigned char Bytes[1024];
    int           curChar, i, j;
    int           bytesDumped;

    char *pos          = (char *) memory;
    char *HeaderOffset = (char *) memory;

    if ( BytesToDump <= 0 )
    {
        return "";
    }

    while ( BytesToDump > 0 )
    {
        dump         += StrFormat("%08X: ", HeaderOffset);
        HeaderOffset += BytesPerLine;

        for ( i = bytesDumped = 0; i < BytesPerLine; i++ )
        {
            curChar = (char) *pos;
            pos++;

            if ( BytesToDump > 0 )
            {
                dump    += StrFormat("%02X ", ( curChar & 0xFF ));
                Bytes[i] = (unsigned char) ( curChar & 0xFF );
                --BytesToDump;
                bytesDumped++;
            }
            else
            {
                dump   += "   ";
                curChar = ' ';
            }

            if ( ( i > 0 ) && ( ( ( i + 1 ) % 4 ) == 0 ) )
            {
                dump += " ";
            }
        }

        for ( j = 0; j < bytesDumped; j++ )
        {
            char ch = Bytes[j] >= 33 && Bytes[j] < 128 ? Bytes[j] : '.';
            dump += StrFormat("%c", ch);
        }
        dump += "\n";
    }

    return dump;
}
inline std::string RawDump (void *memory, int BytesToDump, char * format="%02X,")
{
    std::string   dump;
    int           curChar, i;
    char *pos          = (char *) memory;

    if ( BytesToDump <= 0 )
    {
        return "";
    }


    for ( i = 0; i < BytesToDump; i++ )
    {
        curChar = (char) *pos;
        pos++;
        dump    += StrFormat(format, ( curChar & 0xFF ));
    }
    // remove trailing ,

    dump += "\n";

    return dump;
}

typedef std::vector<uint8_t> raw_message_t;
/**
     * @brief split inputs a comma separated 2 digit hex string, and returns
     * a raw binary message.
     * @param str hex comma separated string to translate into raw form.
     * @param msg reference to unsigned char array in which to store
     * raw binary data.
     */
void ReverseDump (std::string str, raw_message_t & msg)
{
    const std::string::size_type size(str.size( ));
    size_t                       start = 0;
    size_t                       range = 0;

    msg.clear( );

    /* Explanation:
        * Range - Length of the word to be extracted without spaces.
        * start - Start of next word. During initialization, starts at space 0.
        *
        * Runs until it encounters a ' ', then splits the string with a substr() function,
        * as well as making sure that all characters are lower-case (without wasting time
        * to check if they already are, as I feel a char-by-char check for upper-case takes
        * just as much time as lowering them all anyway.
        */
    size_t i;

    for ( i = 0; i < str.size( ); i++ )
    {
        if ( i == 443 )
        {
            i = i;
        }

        if ( str[i] == ',' )
        {
            std::string buf = str.substr(start, range);
            int         uc;
            sscanf(buf.c_str( ), "%2X", &uc);
            msg.push_back((uint8_t) uc);
            start = i + 1;
            range = 0;
        }
        else
        {
            ++range;
        }
    }

    // We always have a trailing , with no data afterward so this is unnecessary
    // msg.push_back( toLower(str.substr(start, range)) );
    // skip trailing \n
}
}
#endif                                                     // HEXDUMP_H
