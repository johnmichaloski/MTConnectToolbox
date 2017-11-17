//

// SocketParser.h
//

/*
 * DISCLAIMER:
 * This software was produced by the National Institute of Standards
 * and Technology (NIST), an agency of the U.S. government, and by statute is
 * not subject to copyright in the United States.  Recipients of this software
 * assume all responsibility associated with its operation, modification,
 * maintenance, and subsequent redistribution.
 *
 * See NIST Administration Manual 4.09.07 b and Appendix I.
 */

#pragma once
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include "Timing.h"

using namespace Timing;
typedef std::vector<uint8_t> raw_message_t;

/**
* \brief Class provides a simple file replay parser.
*/
class SocketParser
{
public:

    /**
    * \brief Constructor that turn repeat of file off, set first time and captures current time.
    * Todo: the clock resolution is only second level, and MTConnect easily has millisecond Socket updates.
    */
    SocketParser()
    {
        _bRepeat      = false;
        _bFirstTime   = true;
        _realtimetime = Timing::Now( );                    // COleDateTime::GetCurrentTime();
        _linenum      = 0;
    }

    /**
    * \brief Reset is done before the beginning of a file. Either first or each repeated time.
    */
    void Reset (void)
    {
        _bFirstTime   = true;
        _realtimetime = Timing::Now( );                    // COleDateTime::GetCurrentTime();
        _linenum      = 0;
       // _backEnd.Reset( );
    }

    /**
     * @brief split inputs a comma separated 2 digit hex string, and returns
     * a raw binary message.
     * @param str hex comma separated string to translate into raw form.
     * @param msg reference to unsigned char array in which to store
     * raw binary data.
     */
    void split (std::string str, raw_message_t & msg)
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

    /**
    * \brief Processes a line from the given Socket file.
    * Extracts timestamp (first field until | delimiter) and computes how long from last timestamp.
    * If the repeat flag is set, will repeat Socket file from start, resetting all timestamp value to new current time.
    * \return amount of time to delay in seconds given shdr.
    */
    double ProcessStream ( )
    {
        // Get line from "in" stream into string _buffer until line feed found.
        // If the line feed delimiter is found, it is extracted and discarded
        // buffer is a std::string so there will be no "overrun"
        getline(in, _buffer);

        if ( in.eof( ) )
        {
            if ( !Repeat( ) )
            {
                return -1;
            }
            Reset( );
            in.clear( );                                   // forget we hit the end of file
            in.seekg(0, std::ios::beg);                    // move to the start of the file
            getline(in, _buffer);
        }
        _linenum++;

        // Check for empty buffer - return 0 wait
        if ( _buffer.empty( ) )
        {
            return 0;
        }

        // Now get timestamp - in field 1
        size_t delim = _buffer.find("|");

        if ( delim == std::string::npos )
        {
            return 0;
        }

        // strip out timestamp from buffer
        std::string timestamp = _buffer.substr(0, delim);

        if ( !timestamp.empty( ) )
        {
            _lasttime    = _currenttime;
            _currenttime = Timing::GetDateTime(timestamp);

            if ( _bFirstTime )
            {
                _duration   = std::chrono::seconds(0);
                _bFirstTime = false;
            }
            else
            {
                _duration      = _currenttime-_lasttime;
                _realtimetime += std::chrono::duration_cast<std::chrono::nanoseconds>(_duration);
            }

            // in shdr this returns a shdr string with new timestamp.

            _timestamp = Timing::GetDateTimeString(_realtimetime);

            // For binary hex dump need to translate ascii hex into raw binary.
            // also need to add timestamp for timing and remove trailing \n
            split(_buffer.substr(delim + 1), _msg);
        }
        else
        {
            _duration = std::chrono::seconds(0);
        }

        return static_cast<double>(
            std::chrono::duration_cast<std::chrono::milliseconds>(_duration).count( ) );
     }

    /**
    * \brief Attempts to open Socket file for echoing, throw execption if unable to open file.
    * \param filename is the Socket filename including path
    */
    void Init (std::string filename)
    {
        // FIXME: blank lines and other stuff not handled well.
        _binaryfilename = filename;

        // open file
        in.open(_binaryfilename.c_str( ));

        // make sure file opens ok, that is exists and is readable
        if ( !in )
        {
            throw std::runtime_error(StrFormat("Can't open socket hex file %s", _binaryfilename.c_str( )));
        }
    }

    /**
    * \brief Flag to determine replaying of file. True replay. False one-time play.
    * \return reference to replay flag.
    */
    bool & Repeat ( ) { return _bRepeat; }

    /**
    * \brief Get latest hex buffer from file
    */
    std::string & GetLatestBuffer ( ) { return _buffer; }

    /**
    * \brief Get latest raw binary message from file
    */
    raw_message_t & GetLatestMsg ( ) { return _msg; }

    /**
    * \brief Get latest timestamp read from file.
    * @return string containing timestamp.
    */
    std::string & GetLatestTimestamp ( ) { return _timestamp; }

    /**
    * \brief Get latest Line Number.
    */
    size_t & LineNumber ( ) { return _linenum; }

    //////////////////////////////////////////////////////////////////////
protected:

    std::chrono::high_resolution_clock::time_point _realtimetime;                /**< now + elapsed Socket time */
    std::chrono::high_resolution_clock::time_point _currenttime;                 /**< shdr current timestamps */
    std::chrono::high_resolution_clock::time_point _lasttime;                    /**< last shdr current timestamps */
    std::chrono::duration<double> _duration;                       /**< duration between last and current shdr timestamps */

    std::string _timestamp;                                /**< string with updated timestamp Socket */
    std::string _buffer;                                   /**< string with raw data as hex ascii */
    raw_message_t _msg;                                    /**< buffer with raw binary data */
    std::string _binaryfilename;                           /**< string with Socket filename incl path*/
    bool _bRepeat;                                         /**< repeat Socket flag*/
    bool _bFirstTime;                                      /**< first time flag for deciphering timestamps*/
    std::ifstream in;                                      /**< input filestream*/
    size_t _linenum;                                       /**< current line number in file useful for exceptions*/
 };

