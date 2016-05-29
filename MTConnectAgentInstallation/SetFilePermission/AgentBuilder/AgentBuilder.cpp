// AgentBuilder.cpp : main source file for AgentBuilder.exe
//


#include "stdafx.h"
#include "resource.h"
#include "MainDlg.h"
#include "Utils.h"
#include "StdStringFcn.h"
#include "Yaml.h"
#include "Outdebug.h"
#include "ATLUtils.h"
#include <boost/program_options.hpp>
using namespace  boost::program_options;

CAppModule _Module;

#pragma comment(lib, "comsuppw.lib")
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Iphlpapi.lib")

#if defined(WIN64) && defined( _DEBUG) 
#pragma message( "DEBUG x64" )
#pragma comment(lib, "libboost_system-vc100-mt-sgd-1_54.lib")
#pragma comment(lib, "libboost_thread-vc100-mt-sgd-1_54.lib")
#pragma comment(lib, "libboost_exception-vc100-mt-sgd-1_54.lib")
#elif !defined( _DEBUG) && defined(WIN64)
#pragma message( "RELEASE x64" )
#pragma comment(lib, "libboost_thread-vc100-mt-s-1_54.lib")
#pragma comment(lib, "libboost_system-vc100-mt-s-1_54.lib")
#pragma comment(lib, "libboost_exception-vc100-mt-s-1_54.lib")
#elif defined(_DEBUG) && defined(WIN32)
#pragma message( "DEBUG x32" )
#pragma comment(lib, "libboost_thread-vc100-mt-sgd-1_54.lib")
#pragma comment(lib, "libboost_system-vc100-mt-sgd-1_54.lib")
#pragma comment(lib, "libboost_exception-vc100-mt-sgd-1_54.lib")
#pragma comment(lib, "libboost_program_options-vc100-mt-sgd-1_54")
#elif !defined( _DEBUG) && defined(WIN32)
#pragma message( "RELEASE x32" )
#pragma comment(lib, "libboost_thread-vc100-mt-s-1_54.lib")
#pragma comment(lib, "libboost_system-vc100-mt-s-1_54.lib")
#pragma comment(lib, "libboost_exception-vc100-mt-s-1_54.lib")
#pragma comment(lib, "libboost_program_options-vc100-mt-s-1_54")
#endif

namespace boost 
{
	void throw_exception(std::exception const & e)
	{
		throw e;
	}
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int /*nCmdShow*/)
{
	HRESULT hRes = ::OleInitialize(NULL);

	//HRESULT hRes = ::CoInitialize(NULL);
// If you are running on NT 4.0 or higher you can use the following call instead to 
// make the EXE free threaded. This means that calls come in on a random RPC thread.
//	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	ATLASSERT(SUCCEEDED(hRes));

	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(NULL, 0, 0, 0L);

	AtlInitCommonControls(ICC_BAR_CLASSES | ICC_DATE_CLASSES);	// add flags to support other controls

	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

	AtlAxWinInit();

	int nRet = 0;
	try
	{	CMainDlg dlgMain;

	//DebugFileOut debugfile(CString((::ExeDirectory() + "debug.txt").c_str()));
	//	basic_debugbuf debugbuf(2048);
	//	std::cout.rdbuf(&debugfile);
	//	cout<< "Test Me" << endl;
	//
	//
	//	Yaml storage ;
	//	ParseIniFile("C:\\Program Files\\MTConnect\\Agent\\Agent.cfg", storage );

	// BLOCK: Run application


	std::string cfgfilename;
	std::string destinationfolder("C:\\Program Files\\MTConnect\\Agent\\");
	options_description desc("Allowed options");
		AtlTrace(StdStringFormat("AgentCfg=%s\n",lpstrCmdLine).c_str() );
	desc.add_options()
		("-f", value<std::string>(), "ConfigFile")
		("-p", value<std::string>(), "HTTP Port")
		("-d", value<std::string>(), "Destination folder")
		("-restart", "Restart Agent Service")
		;
	if(__argc > 1)
	{
		try {
			variables_map vm;
			std::vector<std::string> args = split_winmain(lpstrCmdLine);
			store(command_line_parser(args).options(desc).run(), vm);
			if (vm.count("-f")) { cfgfilename= vm["-f"].as<std::string>();}
			if (vm.count("-p")) { dlgMain.HttpPort() = vm["-p"].as<std::string>();}
			if (vm.count("-d")) { destinationfolder= vm["-d"].as<std::string>();}

		}
		catch(std::exception e)
		{
			AtlTrace(e.what());
		}
		AtlTrace(StdStringFormat("AgentCfg CfgFile=%s\n",cfgfilename.c_str()).c_str() );
		AtlTrace(StdStringFormat("AgentCfg Destinationfolder=%s\n",destinationfolder.c_str()).c_str() );
	// FIXME: error checking
		std::string contents;
		ReplaceAll(cfgfilename,"\"", "");
		ReplaceAll(destinationfolder,"\"", "");
		ReadFile(cfgfilename, contents);
		AtlTrace(contents.c_str());
		dlgMain.UpdateLists(contents);
		dlgMain.WriteCfgFile(destinationfolder);
	}

	else
	{

		nRet = dlgMain.DoModal();
	}
	}
	catch(...)
	{

		OutputDebugString("_tWinMain Exception \n ");
	}

	_Module.Term();
	::CoUninitialize();

	return nRet;
}
