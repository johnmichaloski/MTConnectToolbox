/*
 * ur_communication.h
 *
 * Copyright 2015 Thomas Timm Andersen
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef UR_COMMUNICATION_H_
#define UR_COMMUNICATION_H_

#include <fcntl.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <vector>

#ifdef _WINDOWS

// #include "winsock2.h"
#include "time.h"
#include "windows.h"
#else
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#endif

#include "URInterface.h"

#if C11
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>
namespace ur_comm = std;
#else
#include <boost/bind.hpp>
#include <boost/date_time.hpp>
#include <boost/thread/thread.hpp>
namespace ur_comm = boost;
#endif

#include "NIST/RCSMutexVar.h"


typedef std::vector<uint8_t> ur_message_t;
class UR_Adapter;

class ur_communication
{
public:
    ur_communication( );
    bool         start ( );
    void         halt ( );
    HRESULT      init (UR_Adapter *, std::string host);
    ur_message_t primary_socket ( );
    RCS::MutexVariable<bool> mConnected;
    bool mKeepalive;
private:
    void         run ( );                                  /// thread
    UR_Adapter *_ur_adapter;
    struct hostent *mServer;
    ur_comm::thread mCommThread;
    std::string mHost;
};

#endif                                                     /* UR_COMMUNICATION_H_ */
