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
    inline std::string RawDump (void *memory, int BytesToDump, char *format = "%02X,")
    {
        std::string dump;
        int         curChar, i;
        char *      pos = (char *) memory;

        if ( BytesToDump <= 0 )
        {
            return "";
        }

        for ( i = 0; i < BytesToDump; i++ )
        {
            curChar = (char) *pos;
            pos++;
            dump += StrFormat(format, ( curChar & 0xFF ));
        }

        // remove trailing ,

        dump += "\n";

        return dump;
    }
}
#endif                                                     // HEXDUMP_H
