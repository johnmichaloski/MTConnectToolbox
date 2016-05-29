#include "stdafx.h"
#include <winsock2.h>
#include <iphlpapi.h>
#include <icmpapi.h>
#include <stdio.h>
#include "Logger.h"

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

int __cdecl Ping(const char *host, int nTimeout)  
{

	// Declare and initialize variables

	HANDLE hIcmpFile;
	unsigned long ipaddr = INADDR_NONE;
	DWORD dwRetVal = 0;
	char SendData[32] = "Data Buffer";
	LPVOID ReplyBuffer = NULL;
	DWORD ReplySize = 0;

	ipaddr = inet_addr(host);
	if (ipaddr == INADDR_NONE) {
		GLogger.LogMessage(StdStringFormat("usage: %s IP address\n", host), INFO);
		return 1;
	}

	hIcmpFile = IcmpCreateFile();
	if (hIcmpFile == INVALID_HANDLE_VALUE) {
		GLogger.LogMessage( StdStringFormat("\tUnable to open handle.\n"), INFO);
		GLogger.LogMessage(StdStringFormat("IcmpCreatefile returned error: %ld\n", GetLastError() ), INFO);
		return 1;
	}    

	ReplySize = sizeof(ICMP_ECHO_REPLY) + sizeof(SendData);
	ReplyBuffer = (VOID*) malloc(ReplySize);
	if (ReplyBuffer == NULL) {
		GLogger.LogMessage(StdStringFormat("\tUnable to allocate memory\n"), INFO);
		return 1;
	}    


	dwRetVal = IcmpSendEcho(hIcmpFile, ipaddr, SendData, sizeof(SendData), 
		NULL, ReplyBuffer, ReplySize, nTimeout);
	if (dwRetVal != 0) 
	{
		PICMP_ECHO_REPLY pEchoReply = (PICMP_ECHO_REPLY)ReplyBuffer;
		struct in_addr ReplyAddr;
		ReplyAddr.S_un.S_addr = pEchoReply->Address;
		GLogger.LogMessage(StdStringFormat("\tSent icmp message to %s\n", host), INFO);
		if (dwRetVal > 1) 
		{
			GLogger.LogMessage(StdStringFormat("\tReceived %ld icmp message responses\n", dwRetVal), INFO);
			GLogger.LogMessage(StdStringFormat("\tInformation from the first response:\n"), INFO) ;
		}    
		else 
		{    
			GLogger.LogMessage(StdStringFormat("\tReceived %ld icmp message response\n", dwRetVal), INFO);
			GLogger.LogMessage(StdStringFormat("\tInformation from this response:\n"), INFO); 
		}    
		GLogger.LogMessage(StdStringFormat("\t  Received from %s\n", inet_ntoa( ReplyAddr ) ), INFO);
		GLogger.LogMessage(StdStringFormat("\t  Status = %ld\n", 
			pEchoReply->Status), INFO);
		GLogger.LogMessage(StdStringFormat("\t  Roundtrip time = %ld milliseconds\n", 
			pEchoReply->RoundTripTime), INFO);
	}
	else 
	{
		GLogger.LogMessage(StdStringFormat("\tCall to IcmpSendEcho failed.\n"), INFO);
		GLogger.LogMessage(StdStringFormat("\tIcmpSendEcho returned error: %ld\n", GetLastError() ), INFO);
		return 1;
	}
	return 0;
} 