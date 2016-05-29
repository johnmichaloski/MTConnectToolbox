//==============================================================================================

//
// DDEapi.cpp
//
// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied 
// or intended.Any questions or comments mailto:John.Michaloski@nist.gov

/*
It uses the DDE execute facility for dealing with Word.  Each call to
an API here causes a DDE conversation to be set up, the execute
request made and the conversation terminated.

Example:
SendExecCmd("WinWord", "[FileOpen(\"c:\\michaloski\\worddoc.doc\")][FilePrint 0][DocClose 2]");

DE is a means for one application (the client) to communicate with another (the server) via messages. The client can send commands and data to the server which will run the commands and process the data. This exchange of messages is called a conversation.

Conversations are about a topic. Think of a topic as the fundamental data grouping used in an application. A workbook in Excel or a form in VB for instance. Topics can be about a specific item, say a particular Excel spreadsheet cell.

Lets look at an example of sending data to a cell in Excel. With this app and Excel running, the first step is to register this application with the DDEML. This is accomplished with a call to the DdeInitialize API when you click the Initialize button. Conversly, the DdeUninitialize function is used to free all DDEML resources when our conversation is complete.

Next we need to describe our conversation. The service or process that we will converse with is "Excel" and the topic is the "book1" workbook. For the item we can specify a cell such as "R1C1" and we can use a value of "test".

After these values are entered into the corresponding textboxes the Poke button can be pressed. This uses the DdeCreateStringHandle function to convert our conversation parameters into memory handles. A call to DdeConnect establishes a conversation with a server application that supports the specified service name and topic and returns a handle to the conversation.

Once a conversation is established, the DdeClientTransaction API transfers the actual data to the server (Excel) application. DdeClientTransaction call uses a Poke transaction type which means "send unsolicited data" to the server application.

The Execute and Request buttons perfom similar functions but use "Execute" and "Request" type transactions. Execute is used to send a command string to the server while Request asks the server for data.


*/
#include "stdafx.h"
#include <windows.h>

#include <stdio.h>
#include <atltime.h>
#include <time.h>

#include "DDEapi.h"

#include "StdStringFcn.h"

/**************************************************************************** 
* Callback function for DDE messages.
*  @parameter wType
*  @parameter wFmt
*  @parameter hConv
*  @parameter hsz1
*  @parameter hsz2
*  @parameter hDDEData
*  @parameter dwData1
*  @parameter dwData2
*  @return HDDEDATA handle to the DDE data
*****************************************************************************/
HDDEDATA CALLBACK DDEAdapter::DDEMyCallback(UINT wType, 
                              UINT wFmt, 
                              HCONV hConv,
                              HSZ hsz1, 
                              HSZ hsz2, 
                              HDDEDATA hDDEData, 
                              DWORD dwData1, 
                              DWORD dwData2)
{
	std::cerr << StdStringFormat( "Got DDE Callback %x\n", wType);
	HDDEDATA ret=(HDDEDATA)true;
   	switch (wType)
	{
	case XTYP_CONNECT:
		ret = (HDDEDATA)true;
		break;
	case XTYP_REQUEST:
		//	We use the DdeCmpStringHandles instead of the strcmp for two reasons: 1st we already have the strings in HSZ
		//	format, 2nd this way it's case insensitive as the DDE protocol is supposed to be.
		std::cerr<< StdStringFormat("XTYP_REQUEST\n");
		//if (DdeCmpStringHandles(hszTopic, ddeAdapter.DDETopicEvaluate) == 0)

		break;
	case XTYP_ERROR:
		std::cerr<< StdStringFormat("Error=%x\n",  XTYP_ERROR);
		break;
	case     XTYP_ADVDATA :
		{
			//std::cerr << StdStringFormat("XTYP_ADVDATA callback  %s=%s\n", szItem,szResult );
		}
		break;
	case     XTYP_ADVREQ          :
		std::cerr<< StdStringFormat("XTYP_ADVREQ\n");
		break;		
	case     XTYP_ADVSTART           :
		std::cerr<< StdStringFormat("XTYP_ADVSTART\n");
		break;		
	case     XTYP_ADVSTOP           :
		std::cerr<< StdStringFormat("XTYP_ADVSTOP\n");
		break;		
	case     XTYP_EXECUTE           :
		std::cerr<< StdStringFormat("XTYP_ADVDATA\n");
		break;	
	case     XTYP_CONNECT_CONFIRM    :
		std::cerr<< StdStringFormat("XTYP_CONNECT_CONFIRM\n");
		break;
	case     XTYP_XACT_COMPLETE     :
		std::cerr<< StdStringFormat("XTYP_XACT_COMPLETE\n");
		break;
	case     XTYP_REGISTER           :
		std::cerr<< StdStringFormat("XTYP_REGISTER\n");
		break;
	case     XTYP_DISCONNECT         :
		std::cerr<< StdStringFormat("XTYP_DISCONNECT\n");
		break;
	case     XTYP_UNREGISTER         :
		std::cerr<< StdStringFormat("XTYP_UNREGISTER\n");
		break;
	case     XTYP_WILDCONNECT        :
		std::cerr<< StdStringFormat("XTYP_WILDCONNECT\n");
		break;
	case XTYP_POKE:
		//			ret = DDE_FACK;					// Ritorna FACK se il messaggio è stato processato
		ret = DDE_FNOTPROCESSED;
		break;
	}

	return ret;
}

//HDDEDATA CALLBACK DDEAdapter::DDECallback(UINT uType, UINT uFmt, HCONV hconv, HSZ hszTopic, HSZ hszItem, HDDEDATA hdata, DWORD dwData1, DWORD dwData2)
//{
//	HDDEDATA ret = 0;
//	OutputDebugString("Got DDE Callback\n");
//	std::cerr << "Got DDE Callback\n";
//
//	switch (uType)
//	{
//		//
//		//		Accetta connessioni DDE
//		//
//	case XTYP_CONNECT:
//		ret = (HDDEDATA)true;
//		break;
//	case XTYP_REQUEST:
//		//	We use the DdeCmpStringHandles instead of the strcmp for two reasons: 1st we already have the strings in HSZ
//		//	format, 2nd this way it's case insensitive as the DDE protocol is supposed to be.
//		if (DdeCmpStringHandles(hszTopic, ddeAdapter.DDETopicEvaluate) == 0)
//		{
//			ret = DdeCreateDataHandle(ddeAdapter.dwDDEInst, (BYTE*)&ddeAdapter.Message[0], strlen(ddeAdapter.Message)+1, 0, hszItem, CF_TEXT, 0);
//			//				if (ret == 0)
//			//				  ret = DdeCreateDataHandle(idInst, (BYTE*)"\1Unknown Error...", 18, 0, hszItem, CF_TEXT, 0);
//		}
//		break;
//	case XTYP_ERROR:
//		std::cerr<< StdStringFormat("Error=%s\n",  hszTopic);
//		break;
//	case     XTYP_ADVDATA :
//		{
//			if(uFmt==CF_TEXT)
//			{
//				char szResult[255];
//				char szItem[255];
//
//				DdeGetData(hdata, (unsigned char *)szResult, 255, 0);
//				DdeQueryString(ddeAdapter.dwDDEInst,hszItem,szItem,256,CP_WINANSI );
//				ddeAdapter.values[szItem]=szResult;
//				std::cerr <<StdStringFormat("XTYP_ADVDATA callback  %s=%s\n", szItem,szResult );
//			}
//		}
//		break;
//	case     XTYP_ADVREQ          :
//		std::cerr<< StdStringFormat("XTYP_ADVREQ\n");
//		break;		
//	case     XTYP_ADVSTART           :
//		std::cerr<< StdStringFormat("XTYP_ADVSTART\n");
//		break;		
//	case     XTYP_ADVSTOP           :
//		std::cerr<< StdStringFormat("XTYP_ADVSTOP\n");
//		break;		
//	case     XTYP_EXECUTE           :
//		std::cerr<< StdStringFormat("XTYP_ADVDATA\n");
//		break;	
//	case     XTYP_CONNECT_CONFIRM    :
//		std::cerr<< StdStringFormat("XTYP_CONNECT_CONFIRM\n");
//		break;
//	case     XTYP_XACT_COMPLETE     :
//		std::cerr<< StdStringFormat("XTYP_XACT_COMPLETE\n");
//		break;
//	case     XTYP_REGISTER           :
//		std::cerr<< StdStringFormat("XTYP_REGISTER\n");
//		break;
//	case     XTYP_DISCONNECT         :
//		std::cerr<< StdStringFormat("XTYP_DISCONNECT\n");
//		break;
//	case     XTYP_UNREGISTER         :
//		std::cerr<< StdStringFormat("XTYP_UNREGISTER\n");
//		break;
//	case     XTYP_WILDCONNECT        :
//		std::cerr<< StdStringFormat("XTYP_WILDCONNECT\n");
//		break;
//	case XTYP_POKE:
//		//			ret = DDE_FACK;					// Ritorna FACK se il messaggio è stato processato
//		ret = DDE_FNOTPROCESSED;
//		break;
//	}
//
//	return ret;
//}

//================================================================

DDEAdapter:: DDEAdapter()
{
	dwDDEInst = 0;
	hConv=NULL;
	hTransaction=0;
}
int DDEAdapter::Init()
{
	UINT ui;
	dwDDEInst = NULL;
	
	ui = DdeInitialize(&dwDDEInst,
//		(PFNCALLBACK)  &boost::bind<HDDEDATA>(&DDEAdapter::DDECallback,this,_1, _2, _3, _4, _5, _6, _7, _8),
		(PFNCALLBACK) DDEMyCallback,
		APPCMD_CLIENTONLY,
		0l);

	if (ui != DMLERR_NO_ERROR) 
	{
		dwDDEInst = NULL;
		throw this->DDEErrorMessage(ui); 
	}
	return DMLERR_NO_ERROR;
}
void DDEAdapter:: Quit()
{
	DdeFreeStringHandle(dwDDEInst, hszTopic);
	DdeFreeStringHandle(dwDDEInst, hszApp);

	//DdeNameService(dwDDEInst, DDEServerName, 0, DNS_UNREGISTER);
	DdeUninitialize(dwDDEInst);
	 hConv = NULL;
	 dwDDEInst=NULL;
}
bool DDEAdapter::IsConnected()
{
	return( hConv != NULL);
}


// Initiate a conversation the the PROGMAN service
// on the APP topic.
int  DDEAdapter::Connect(LPSTR app, LPSTR topic)
{
	// Must be initialized
	if(dwDDEInst == 0)
		throw this->DDEErrorMessage(DMLERR_DLL_NOT_INITIALIZED);

	hszApp = DdeCreateStringHandle(dwDDEInst, app, CP_WINANSI);
	hszTopic = DdeCreateStringHandle(dwDDEInst, topic, CP_WINANSI);
	//std::cerr <<StdStringFormat("hszApp = %x\n", hszApp);
	//std::cerr <<StdStringFormat("hszTopic = %x\n", hszTopic);

	hConv = DdeConnect(dwDDEInst,hszApp,hszTopic,NULL);
	UINT err = DdeGetLastError(dwDDEInst);

	if(err != DMLERR_NO_ERROR)
	{
		hConv=NULL;
		throw this->DDEErrorMessage(err);
	}
	return DMLERR_NO_ERROR;
}
void  DDEAdapter::Disconnect()
{
	if(!hConv) return;
	if(hTransaction) 
		DdeAbandonTransaction(dwDDEInst, hConv, hTransaction);
	hTransaction=0;
	DdeDisconnect(hConv);
	hConv=0;
}
/**************************************************************************** 
* DDE method to request data on a DDE conversation.
*  @parameter dwDDEInst 
*  @parameter hConv  handle to a DDE conversation
*  @parameter lpszQuery  DDE query command as type long pointer to a zero-terminated string 
*  @return the string retrieved from the DDE query.
*****************************************************************************/

std::string DDEAdapter::DDERequest(std::string item)
{
	char szResult[255]={0};
	// Error checking
	if(dwDDEInst==NULL || hConv==NULL)
		throw DDEErrorMessage(DMLERR_NO_CONV_ESTABLISHED);

	HSZ hszRequest = DdeCreateStringHandleA(dwDDEInst, item.c_str(), CP_WINANSI);
	OutputDebugString(StdStringFormat("Get data %s\n", item.c_str()).c_str());
	HDDEDATA recData = DdeClientTransaction(NULL, 0, hConv, hszRequest, CF_TEXT, XTYP_REQUEST, 1000, NULL);

	if(recData!=NULL)
	{
		DdeGetData(recData, (unsigned char *)szResult, 255, 0);

	}
	else 
	{
		std::cerr<< DDEErrorMessage(DdeGetLastError(dwDDEInst)) << std::endl;
		return DDEErrorMessage(DdeGetLastError(dwDDEInst));
	}
	//char *data = (char*) byte;
	//std::string retStr = data;
	DdeFreeStringHandle(dwDDEInst, hszRequest);
	DdeFreeDataHandle(recData);
	return szResult;
}
std::string DDEAdapter::DdeAyncRequest(std::string item)
{
	if(dwDDEInst==NULL || hConv==NULL)
		return "";
	HSZ hszItem = DdeCreateStringHandle(dwDDEInst, item.c_str(), 0);
    hTransaction=(DWORD)DdeClientTransaction(NULL,0,hConv,hszItem,CF_TEXT,XTYP_ADVSTART, 5000, NULL);
	DdeFreeStringHandle(dwDDEInst, hszItem);
	return "";
}

void DDEAdapter::DDEAyncExecute(DWORD idInst,  char* szCommand)
{
	if(dwDDEInst==NULL || hConv==NULL)
		throw std::exception("NO instance or conversation") ;

	HDDEDATA hData = DdeCreateDataHandle(idInst, (LPBYTE)szCommand,
		lstrlen(szCommand)+1, 0, NULL, CF_TEXT, 0);
	if (hData==NULL)   
	{
		std::cerr<< StdStringFormat("Command failed: %s\n", szCommand);
	}
	else   
	{
		hTransaction=(DWORD) DdeClientTransaction((LPBYTE)hData, 0xFFFFFFFF, hConv, 0L, 0,
			XTYP_EXECUTE, TIMEOUT_ASYNC, NULL);
	}
}
void DDEAdapter::DDEPoke(DWORD idInst, char* szItem, char* szData)
{
	if(dwDDEInst==NULL || hConv==NULL)
		throw std::exception("No Instance or conversation");

	HSZ hszItem = DdeCreateStringHandle(idInst, szItem, 0);
	DdeClientTransaction((LPBYTE)szData, (DWORD)(lstrlen(szData)+1),
		hConv, hszItem, CF_TEXT,
		XTYP_POKE, 3000, NULL);
	DdeFreeStringHandle(idInst, hszItem);
}
std::string DDEAdapter::DDEErrorMessage(UINT err)
{
	switch(err)
	{
	case DMLERR_ADVACKTIMEOUT: return "A request for a synchronous advise transaction has timed out.\n";
	case DMLERR_BUSY: return "The response to the transaction caused the DDE_FBUSY flag to be set.\n";
	case DMLERR_DATAACKTIMEOUT: return "A request for a synchronous data transaction has timed out.\n";
	case DMLERR_DLL_NOT_INITIALIZED	: return "A DDEML function was called without first calling the DdeInitialize function, or an invalid instance identifier was passed to a DDEML function.\n";
	case DMLERR_DLL_USAGE: return "An application initialized as APPCLASS_MONITOR has attempted to perform a Dynamic Data Exchange (DDE) transaction, or an application initialized as APPCMD_CLIENTONLY has attempted to perform server transactions.\n";
	case DMLERR_EXECACKTIMEOUT: return "A request for a synchronous execute transaction has timed out.\n";
	case DMLERR_INVALIDPARAMETER: return "A parameter failed to be validated by the DDEML. Some of the possible causes follow:"
									  "The application used a data handle initialized with a different item name handle than was required by the transaction."
									  "The application used a data handle that was initialized with a different clipboard data format than was required by the transaction."
									  "The application used a client-side conversation handle with a server-side function or vice versa."
									  "The application used a freed data handle or string handle."
									  "More than one instance of the application used the same object.\n";
	case DMLERR_LOW_MEMORY: return "A DDEML application has created a prolonged race condition (in which the server application outruns the client), causing large amounts of memory to be consumed.\n";
	case DMLERR_MEMORY_ERROR: return "A memory allocation has failed.\n";
	case DMLERR_NO_CONV_ESTABLISHED: return "A client's attempt to establish a conversation has failed.\n";
	case DMLERR_NOTPROCESSED: return "A transaction has failed.\n";
	case DMLERR_POKEACKTIMEOUT: return "A request for a synchronous poke transaction has timed out.\n";
	case DMLERR_POSTMSG_FAILED: return "An internal call to the PostMessage function has failed.\n";
	case DMLERR_REENTRANCY: return "An application instance with a synchronous transaction already in progress attempted to initiate another synchronous transaction, or the DdeEnableCallback function was called from within a DDEML callback function.\n";
	case DMLERR_SERVER_DIED: return "A server-side transaction was attempted on a conversation terminated by the client, or the server terminated before completing a transaction.\n";
	case DMLERR_SYS_ERROR: return "An internal error has occurred in the DDEML.\n";
	case DMLERR_UNADVACKTIMEOUT: return "A request to end an advise transaction has timed out.\n";
	case DMLERR_UNFOUND_QUEUE_ID: return "An invalid transaction identifier was passed to a DDEML function. Once the application has returned from an XTYP_XACT_COMPLETE callback, the transaction identifier for that callback function is no longer valid.\n";
	default: return "";
	}
	return "";
}
/**************************************************************************** 
* Send an execute request to the Desired DDE Server
*  @parameter app  - string describing application, e.g., "\\ip addr\\ncdde|machineswitch "
*  @return int,  zero failed. Otherwise success.
*****************************************************************************/
int DDEAdapter::GetData(LPSTR lpszDataName, std::string & data)
{
//	HDDEDATA hExecData;
	int retval = DMLERR_NO_ERROR;
	data.clear();
	try {
		// Must be initialized
		if(dwDDEInst == 0)
			throw this->DDEErrorMessage(DMLERR_DLL_NOT_INITIALIZED);
		//Check that conversation connected
		if (!hConv ) 
			throw this->DDEErrorMessage(DMLERR_NO_CONV_ESTABLISHED);

		hszItem = DdeCreateStringHandle(dwDDEInst,    lpszDataName, CP_WINANSI);
		data=DDERequest(lpszDataName);
	}
	catch(...)
	{

	}
	if(data.size() < 1)
		retval = DMLERR_FIRST;
	return retval;
}
/**************************************************************************** 
* Send an execute request to the Desired DDE Server
*  @parameter app  - string describing application, e.g., "winword"
*  @parameter lpszCmd, string describing DDE command e.g., "[FileOpen(\"test.doc")][CloseDoc]"
*  @return int,  zero failed. Otherwise success.
*****************************************************************************/
//int DDEAdapter::ExecCmd(LPSTR app, LPSTR lpszCmd)
//{
//	UINT ui;
//	HSZ hszTopic;
//	HSZ hszApp;
//	HCONV hConv;
//	HDDEDATA hExecData;
//	LPDWORD pdwResult;
//	int retval = TRUE;
//	char * szProgData;
//
//
//	//				   
//	// Initialize DDEML
//	//
//	ui = DdeInitialize(&dwDDEInst,
//		DDECallback,
//		CBF_FAIL_ALLSVRXACTIONS,
//		0l);
//
//	if (ui != DMLERR_NO_ERROR) {
//		return FALSE;
//	}
//
//	// Initiate a conversation the the PROGMAN service
//	// on the APP topic.
//	//
//
//	hszApp = DdeCreateStringHandle(dwDDEInst,
//		app,
//		CP_WINANSI);
//
//	hszTopic = DdeCreateStringHandle(dwDDEInst,
//		"System",
//		CP_WINANSI);
//
//	hConv = DdeConnect(dwDDEInst,
//		hszApp,
//		hszTopic,
//		NULL);
//
//	//
//	//Check that conversation connected
//	//
//	if (!hConv ) {
//		retval = FALSE;
//		goto done;
//	}
//
//	//
//	// Create a data handle for the exec string
//	//
//	hExecData = DdeCreateDataHandle(dwDDEInst,
//		(unsigned char *) lpszCmd,
//		lstrlen(lpszCmd)+1,
//		0,
//		NULL,
//		0,
//		0);
//
//	//
//	// Send the execute request - just synchronize so we don't go too fast
//	//
//	done=0;
//	DdeClientTransaction((unsigned char *)hExecData,
//		(DWORD)-1,
//		hConv,
//		NULL,
//		0,
//		XTYP_EXECUTE,
//		1000, // ms timeout
//		NULL);
//
//	szProgData=getRequestData(dwDDEInst, hConv, "Topics");
//	// Could test to look for file and what while a topic
//	//	printf("Topics = %s\n", szProgData);
//	free(szProgData);
//
//	szProgData=getRequestData(dwDDEInst, hConv, "SysItems");
//	free(szProgData);
//	szProgData=getRequestData(dwDDEInst, hConv, "Formats");
//	free(szProgData);
//
//	//
//	// Done with the conversation now.
//	//
//
//	DdeDisconnect(hConv);
//
//	//
//	// Done with DDEML
//	//
//
//	DdeUninitialize(dwDDEInst);
//
//done:
//	//
//	// Free the HSZ now
//	//
//
//	DdeFreeStringHandle(dwDDEInst, hszTopic);
//	DdeFreeStringHandle(dwDDEInst, hszApp);
//
//	return retval;
//}

/**************************************************************************** 
*
*
*  FUNCTION   : GetTextData() 
* 
*                                                                          * 
*  PURPOSE    : Allocates and returns a pointer to the data contained in   * 
*               hData.  This assumes that hData points to text data and    *  
*               will properly handle huge text data by leaving out the     *  
*               middle of the string and placing the size of the string    *  
*               into this string portion.                                  *  
*                                                                          *  
*  RETURNS    : A pointer to the allocated string.                         *  
*                                                                          
*****************************************************************************/
PTSTR DDEAdapter::GetTextData( HDDEDATA hData) 
{     
	PTSTR psz;     
	DWORD cb;  
#define MAXCCH  1024      
	if (hData == 0) {
		return(NULL);     
	}      
	cb = DdeGetData(hData, NULL, 0, 0);    
	if (!hData || !cb)        
		return NULL;      
	if (cb > MAXCCH) {  
		// possibly HUGE object!         
		psz = (char *) malloc(MAXCCH * sizeof(TCHAR));         
		DdeGetData(hData, (PBYTE)psz, MAXCCH - 46 * sizeof(TCHAR), 0L);
		wsprintf(&psz[MAXCCH - 46], TEXT("<---Size=%ld"), cb);     
	} else {        
		psz = (char *) malloc((DWORD)cb);    
		DdeGetData(hData, (LPBYTE)psz, cb, 0L);  
	}    
	return psz; 
#undef MAXCCH 
}  


int DDEAdapter::TryRequest(std::string item, int format, int timeout,  std::string &data)
{
	DWORD returnFlags= 0;
	data.clear();        

	// Must be initialized
	if(dwDDEInst == 0)
		throw this->DDEErrorMessage(DMLERR_DLL_NOT_INITIALIZED);
	//Check that conversation connected
	if (!hConv ) 
		throw this->DDEErrorMessage(DMLERR_NO_CONV_ESTABLISHED);

	// Create a string handle for the item name.
	HSZ itemHandle = DdeCreateStringHandle(dwDDEInst, item.c_str(), CP_WINANSI);

	// Request the data from the server.
	HDDEDATA dataHandle = DdeClientTransaction(
		NULL,
		0,
		hConv,
		itemHandle,
		format,
		XTYP_REQUEST,
		timeout,
		&returnFlags);

	// Free the string handle created earlier.
	DdeFreeStringHandle(dwDDEInst, itemHandle);

	// If the data handle is null then the server did not process the request.
	if (dataHandle == NULL)
	{
		data = this->DDEErrorMessage(DdeGetLastError(dwDDEInst));
		return DdeGetLastError(dwDDEInst);
	}

	// Get the data from the data handle.
	int length = DdeGetData(dataHandle, NULL, 0, 0);
	//data = new byte[length];
	BYTE * buffer=(BYTE * ) calloc(length , sizeof(BYTE)); 
	length = DdeGetData(dataHandle, (BYTE *) buffer,  length, 0);

	// Free the data handle created by the server.
	DdeFreeDataHandle(dataHandle);
	if(length) data=(char *) buffer;
	return DMLERR_NO_ERROR;
}

