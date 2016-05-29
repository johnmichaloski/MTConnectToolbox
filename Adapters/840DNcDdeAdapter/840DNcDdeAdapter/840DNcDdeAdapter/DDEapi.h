//
//  DDEapi.h
//
// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied 
// or intended.Any questions or comments mailto:John.Michaloski@nist.gov

/*
    Exported DDE Capabilities
*/
#pragma once

//#include <atlstr.h>
//HRESULT printFile(CString filename, CString filepath, CString filetitle);
#include <string>
#include <map>
#include <vector>
#include <ddeml.h>

class DDEAdapter
{
public:
	DDEAdapter();
	int Init();
	void Quit();
	int Connect(LPSTR app, LPSTR topic);  // conversation
	void Disconnect();
	bool IsConnected();

	int GetData(LPSTR lpszDataName,std::string & data);
	 
	//HDDEDATA CALLBACK DDECallback(UINT uType, UINT uFmt, HCONV hconv, HSZ hszTopic, HSZ hszItem, HDDEDATA hdata, DWORD dwData1, DWORD dwData2);
	static HDDEDATA CALLBACK DDEMyCallback(UINT uType, UINT uFmt, HCONV hconv, HSZ hszTopic, HSZ hszItem, HDDEDATA hdata, DWORD dwData1, DWORD dwData2);

	//int ExecCmd(LPSTR app, LPSTR lpszCmd);
	PTSTR GetTextData( HDDEDATA hData) ;
	std::string DDERequest(std::string item);
	void DDEAyncExecute(DWORD idInst, char* szCommand);
	void DDEPoke(DWORD idInst, char* szItem, char* szData);
	std::string DDEErrorMessage(UINT err);

	std::string DdeAyncRequest(std::string item);

	int DDEAdapter::TryRequest(std::string item, int format, int timeout,  std::string &data);


	//////////////////
	std::map<std::string,std::string> values;
	HCONV hConv;
	DWORD dwDDEInst ;
	HINSTANCE hInst;
	char Message[256];
	std::string szItem1;
	HSZ DDEServerName;
	HSZ DDETopicEvaluate;
	HSZ hszTopic;
	HSZ hszApp;
	HSZ hszItem ;
	DWORD hTransaction;
	std::vector<HCONVLIST> vars;


//#	define ServerName		"ncdde|"
//#	define TopicName		"machineswitch"

};

__declspec(selectany)  	  DDEAdapter ddeAdapter;


