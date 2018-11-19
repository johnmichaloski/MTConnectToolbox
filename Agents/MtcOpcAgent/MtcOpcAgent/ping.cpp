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
		GLogger.Info(StdStringFormat("usage: %s IP address\n", host));
		return 1;
	}

	hIcmpFile = IcmpCreateFile();
	if (hIcmpFile == INVALID_HANDLE_VALUE) {
		GLogger.Info( StdStringFormat("\tUnable to open handle.\n"));
		GLogger.Info(StdStringFormat("IcmpCreatefile returned error: %ld\n", GetLastError() ));
		return 1;
	}    

	ReplySize = sizeof(ICMP_ECHO_REPLY) + sizeof(SendData);
	ReplyBuffer = (VOID*) malloc(ReplySize);
	if (ReplyBuffer == NULL) {
		GLogger.Info(StdStringFormat("\tUnable to allocate memory\n"));
		return 1;
	}    


	dwRetVal = IcmpSendEcho(hIcmpFile, ipaddr, SendData, sizeof(SendData), 
		NULL, ReplyBuffer, ReplySize, nTimeout);
	if (dwRetVal != 0) 
	{
		PICMP_ECHO_REPLY pEchoReply = (PICMP_ECHO_REPLY)ReplyBuffer;
		struct in_addr ReplyAddr;
		ReplyAddr.S_un.S_addr = pEchoReply->Address;
		GLogger.Info(StdStringFormat("\tSent icmp message to %s\n", host));
		if (dwRetVal > 1) 
		{
			GLogger.Info(StdStringFormat("\tReceived %ld icmp message responses\n", dwRetVal));
			GLogger.Info(StdStringFormat("\tInformation from the first response:\n")) ;
		}    
		else 
		{    
			GLogger.Info(StdStringFormat("\tReceived %ld icmp message response\n", dwRetVal));
			GLogger.Info(StdStringFormat("\tInformation from this response:\n")); 
		}    
		GLogger.Info(StdStringFormat("\t  Received from %s\n", inet_ntoa( ReplyAddr ) ));
		GLogger.Info(StdStringFormat("\t  Status = %ld\n", 
			pEchoReply->Status));
		GLogger.Info(StdStringFormat("\t  Roundtrip time = %ld milliseconds\n", 
			pEchoReply->RoundTripTime));
	}
	else 
	{
		GLogger.Info(StdStringFormat("\tCall to IcmpSendEcho failed.\n"));
		GLogger.Info(StdStringFormat("\tIcmpSendEcho returned error: %ld\n", GetLastError() ));
		return 1;
	}
	return 0;
} 