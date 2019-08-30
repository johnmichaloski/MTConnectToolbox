/*
* Copyright (c) 2008, AMT – The Association For Manufacturing Technology (“AMT”)
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the AMT nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* DISCLAIMER OF WARRANTY. ALL MTCONNECT MATERIALS AND SPECIFICATIONS PROVIDED
* BY AMT, MTCONNECT OR ANY PARTICIPANT TO YOU OR ANY PARTY ARE PROVIDED "AS IS"
* AND WITHOUT ANY WARRANTY OF ANY KIND. AMT, MTCONNECT, AND EACH OF THEIR
* RESPECTIVE MEMBERS, OFFICERS, DIRECTORS, AFFILIATES, SPONSORS, AND AGENTS
* (COLLECTIVELY, THE "AMT PARTIES") AND PARTICIPANTS MAKE NO REPRESENTATION OR
* WARRANTY OF ANY KIND WHATSOEVER RELATING TO THESE MATERIALS, INCLUDING, WITHOUT
* LIMITATION, ANY EXPRESS OR IMPLIED WARRANTY OF NONINFRINGEMENT,
* MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. 

* LIMITATION OF LIABILITY. IN NO EVENT SHALL AMT, MTCONNECT, ANY OTHER AMT
* PARTY, OR ANY PARTICIPANT BE LIABLE FOR THE COST OF PROCURING SUBSTITUTE GOODS
* OR SERVICES, LOST PROFITS, LOSS OF USE, LOSS OF DATA OR ANY INCIDENTAL,
* CONSEQUENTIAL, INDIRECT, SPECIAL OR PUNITIVE DAMAGES OR OTHER DIRECT DAMAGES,
* WHETHER UNDER CONTRACT, TORT, WARRANTY OR OTHERWISE, ARISING IN ANY WAY OUT OF
* THIS AGREEMENT, USE OR INABILITY TO USE MTCONNECT MATERIALS, WHETHER OR NOT
* SUCH PARTY HAD ADVANCE NOTICE OF THE POSSIBILITY OF SUCH DAMAGES.
*/
#include "stdafx.H"
//#define BOOST_ALL_NO_LIB

#include "sys/stat.h"
#include "fcntl.h"
#include <string>
#include <vector>
#include <map>


#include "CrclAdapter.h"
#include "CrclAgent.h"
#include "Globals.h"

#include "NIST/StdStringFcn.h"
#include "NIST/Config.h"
#include "NIST/AppEventLog.h"
#include "NIST/Logger.h"
#include "NIST/File.h"
#include "NIST/ProductVersion.h"
#include "NIST/versionno.h"

#include "agent.hpp"

#include "version.h"
#include "dlib/revision.h"
#include "libxml/XmlVersion.h"
#include  <boost/version.hpp>
using namespace std;
//Describes the boost version number in XYYYZZ format such that: (BOOST_VERSION % 100) is the sub-minor version, ((BOOST_VERSION / 100) % 1000) is the minor version, and (BOOST_VERSION / 100000) is the major version.
static std::string GetBoostVersion()
{
 return StrFormat("%d.%d.%d",(BOOST_VERSION / 100000), ((BOOST_VERSION / 100) % 1000), BOOST_VERSION % 100);
}

//#define HANDLECALL
class AgentEx : public Agent
{
public:
	AgentEx() 
	{
	}
protected: 
	CProductVersion version;
#ifdef HANDLECALL
	virtual std::string handleCall(ostream& out,
		const string& path,
		const key_value_map& queries,
		const string& call,
		const string& device)
#else
	virtual std::string handleExtensionCall(const std::string& call, const std::string& device)
#endif
	{ 
		if(call  == "version")
		{
			std::string html;
			std::string v="<versions><version>" + GetVersionString();
			ReplaceAll(v,"\n", "</version>\n<version>");
			v=v.substr(0,v.size()-std::string("<version>").size());
			v=v+"</versions>";
			OutputDebugString(v.c_str());
			html = version.GenerateVersionTable(v);

			return html;
		}
		else if(call  == "documentation")
		{
			std::string html=  version.GenerateXSLTVersion("<versions><version>" +version. GenerateVersionDocument()+"</version></versions>");
			return html;
		}
#ifdef HANDLECALL
		return Agent::handleCall(out, path, queries, call,  device);
#else
		return "";
#endif
	}

	std::string GetVersionString()
	{
		std::string str;
		str+=StrFormat("MTConnect Core Agent Version %d.%d.%d.%d - built on " __TIMESTAMP__ "\n", 
			AGENT_VERSION_MAJOR, AGENT_VERSION_MINOR, AGENT_VERSION_PATCH,AGENT_VERSION_BUILD);
#ifdef WIN64
		std::string compilearch =  "64 bit" ;
#else
		std::string compilearch =   "32 bit";
#endif
		str+=StrFormat("%s Agent Extensions  %s Platform Version %s - built on  " __TIMESTAMP__ "\n", Globals.mServerName.c_str(), compilearch.c_str(), STRPRODUCTVER);
		str+=StrFormat("%s Agent MSI Install Version %s  \n", Globals.mServerName.c_str(), version.GetProductInstallVersion("C:\\Users\\michalos\\Documents\\GitHub\\Agents\\NikonMTConnectAgent\\NikonSetupX64\\NikonSetupX64.vdproj").c_str());
		str+=StrFormat("Windows Version : %s\n", version.GetOSDisplayString().c_str());
		str+=StrFormat("Microsoft Visual C++ Version %s\n", version.GetMSVCVersion(_MSC_VER));
		//		str+=StrFormat("Dlib version %d.%d dated %s \n", DLIB_MAJOR_VERSION , DLIB_MINOR_VERSION,DLIB_DATE);
		//str+=StrFormat("Dlib version %d.%d\n", 17,47);
		str+=StrFormat("XML Lib version %s \n", LIBXML_DOTTED_VERSION );
		str+=StrFormat("Boost libraries version - %s \n", GetBoostVersion().c_str());
		str+=StrFormat("%s Devices = %s\n", Globals.mServerName.c_str(), GetAllDevices().c_str());
		return str;

	}
	// This is here becuase the mDeviceMap variable member of Agent is protected, and its easier to just access here than in CProductVersion
	std::string GetAllDevices()
	{
		std::string d;
		for(std::map<std::string, Device *>::iterator it= mDeviceMap.begin(); it!=mDeviceMap.end(); it++)
		{
			if(it!=mDeviceMap.begin()) d+=",";
			d+=(*it).first ;
		}
		return d;
	}
};

#ifdef _WINDOWS

//Handles Win32 exceptions (C structured exceptions) as C++ typed exceptions
static void trans_func( unsigned int u, EXCEPTION_POINTERS* pExp )
{
	std::stringstream outbuf;
	OutputDebugString( outbuf.str().c_str() );
	throw std::exception();
}
#endif
//int main(int aArgc, const char *aArgv[])
//{
//#ifdef _WINDOWS
//	_set_se_translator( trans_func );   //Handles Win32 exceptions (C structured exceptions) as C++ typed exceptions
//	// MICHALOSKI ADDED
//	TCHAR buf[1000];
//	GetModuleFileName(NULL, buf, 1000);
//	std::string path(buf);
//	path=path.substr( 0, path.find_last_of( '\\' ) +1 );
//	SetCurrentDirectory(path.c_str());
//#endif
//	AgentConfigurationEx config;
//	config.setAgent(new AgentEx());
//
//
//	Adapter::enummapping["execution.READY"]="IDLE";
//	Adapter::enummapping["execution.ACTIVE"]="EXECUTING";
//	Adapter::enummapping["execution.INTERRUPTED"]="PAUSED";
//	Adapter::enummapping["execution.STOPPED"]="PAUSED";
//	Adapter::keymapping["mode"]="controllermode";
//	Adapter::keymapping["Fovr"]="path_feedrateovr";
//	Adapter::keymapping["Fact"]="path_feedratefrt";
//
//
//	return config.main(aArgc, (const char **) aArgv);
//}
HRESULT FailWithMsg(HRESULT hr, tstring errmsg)
{
	logFatal(errmsg.c_str());
	return E_FAIL;
}
HRESULT WarnWithMsg(HRESULT hr, tstring errmsg)
{
	logWarn(errmsg.c_str());
	return E_FAIL;
}
#include "CrclInterface.h"
#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/XMLGrammarPoolImpl.hpp>
int _tmain(int argc, _TCHAR* argv[])
{
	try {
#if 0
		xercesc::XMLPlatformUtils::Initialize();
		CrclInterface crcl;
		std::string text = crcl.CRCLGetStatusCmd();
		xercesc::XMLPlatformUtils::Terminate ();
#endif
		GLogger.Timestamping()=true;
		GLogger.DebugString()="CrclAgent";
		GLogger.OutputConsole()=true;
		GLogger.Open(File.ExeDirectory()+"Debug.txt");
		GLogger.DebugLevel()=5;
		logFatal("Start CRCL Agent\n");
		//	EventLogger.Register();
#ifdef _WINDOWS
		_set_se_translator( trans_func );   //Handles Win32 exceptions (C structured exceptions) as C++ typed exceptions
		// MICHALOSKI ADDED
		TCHAR buf[1000];
		GetModuleFileName(NULL, buf, 1000);
		std::string path(buf);
		path=path.substr( 0, path.find_last_of( '\\' ) +1 );
		SetCurrentDirectory(path.c_str());
#endif
		// check for 		inifile=::ExeDirectory() + "Config.ini";	
		AgentConfigurationEx config;
		config.setAgent(new AgentEx());
		//config.setAgent(new Agent());
		Globals.Debug = 5;

		return config.main(argc, (const char **) argv);
	}
	catch(...)
	{
		logFatal("Abnormal exception end to  CRCL Agent\n");
	}
	return 0;
}
