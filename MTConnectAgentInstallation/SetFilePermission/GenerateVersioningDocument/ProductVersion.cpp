//
//
//

#include "StdAfx.h"
#include "ProductVersion.h"
#include <sstream>
#include "StdStringFcn.h"
#define ErrMessage ATLTRACE2


#pragma comment(lib, "Version.lib")

#include <time.h>

// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
std::string currentDateTime() 
{

    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d", &tstruct);

    return buf;
}



CProductVersion::CProductVersion(void)
{
	major =     0;
	minor =     0;
	revision =  0;
	build =     0;
}


CProductVersion::~CProductVersion(void)
{
}

void CProductVersion::GetFileVersion( TCHAR *pszFilePath )
{
    DWORD               dwSize              = 0;
    BYTE                *pbVersionInfo      = NULL;
    VS_FIXEDFILEINFO    *pFileInfo          = NULL;
    UINT                puLenFileInfo       = 0;

    // get the version info for the file requested
    dwSize = GetFileVersionInfoSize( pszFilePath, NULL );
    if ( dwSize == 0 )
    {
        ErrMessage( "Error in GetFileVersionInfoSize: %d\n", GetLastError() );
        return;
    }

    pbVersionInfo = new BYTE[ dwSize ];

    if ( !GetFileVersionInfo( pszFilePath, 0, dwSize, pbVersionInfo ) )
    {
        ErrMessage( "Error in GetFileVersionInfo: %d\n", GetLastError() );
        delete[] pbVersionInfo;
        return;
    }

    if ( !VerQueryValue( pbVersionInfo, TEXT("\\"), (LPVOID*) &pFileInfo, &puLenFileInfo ) )
    {
        ErrMessage( "Error in VerQueryValue: %d\n", GetLastError() );
        delete[] pbVersionInfo;
        return;
    }

    // pFileInfo->dwFileVersionMS is usually zero. However, you should check
    // this if your version numbers seem to be wrong
#if 0
    ErrMessage( "File Version: %d.%d.%d.%d\n",
        ( pFileInfo->dwFileVersionLS >> 24 ) & 0xff,
        ( pFileInfo->dwFileVersionLS >> 16 ) & 0xff,
        ( pFileInfo->dwFileVersionLS >>  8 ) & 0xff,
        ( pFileInfo->dwFileVersionLS >>  0 ) & 0xff
        );

    // pFileInfo->dwProductVersionMS is usually zero. However, you should check
    // this if your version numbers seem to be wrong

    printf( "Product Version: %d.%d.%d.%d\n",
        ( pFileInfo->dwProductVersionLS >> 24 ) & 0xff,
        ( pFileInfo->dwProductVersionLS >> 16 ) & 0xff,
        ( pFileInfo->dwProductVersionLS >>  8 ) & 0xff,
        ( pFileInfo->dwProductVersionLS >>  0 ) & 0xff
        );
#endif

}
void CProductVersion::GetVersionInfo(std::string filename)
{
	GetFileVersion( (TCHAR *) filename.c_str() );
	DWORD  verHandle = NULL;
	UINT   size      = 0;
	LPBYTE lpBuffer  = NULL;
	DWORD  verSize   = GetFileVersionInfoSize( (TCHAR *) filename.c_str(), &verHandle);

	if (verSize != NULL)
	{
		LPSTR verData = new char[verSize];

		if (GetFileVersionInfo( (TCHAR *) filename.c_str(), verHandle, verSize, verData))
		{
			if (VerQueryValue(verData,"\\",(VOID FAR* FAR*)&lpBuffer,&size))
			{
				if (size)
				{
					VS_FIXEDFILEINFO *verInfo = (VS_FIXEDFILEINFO *)lpBuffer;
					if(IsWow64())
					{
						// 64 bit build
						major =     (verInfo->dwProductVersionMS >> 16) & 0xff;
						minor =     (verInfo->dwProductVersionMS >>  0) & 0xff;
						revision =  (verInfo->dwProductVersionLS >> 16) & 0xff;
						build =     (verInfo->dwProductVersionLS >>  0) & 0xff;
					} 
					else
					{
						// 32 bit build
						major =     HIWORD(verInfo->dwProductVersionMS);
						minor =     LOWORD(verInfo->dwProductVersionMS);
						revision =  HIWORD(verInfo->dwProductVersionLS);
						build =     LOWORD(verInfo->dwProductVersionLS);
					}
				}
			}
		}
	}
}

typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
LPFN_ISWOW64PROCESS fnIsWow64Process;

BOOL CProductVersion::IsWow64()
{
    BOOL bIsWow64 = FALSE;

    //IsWow64Process is not available on all supported versions of Windows.
    //Use GetModuleHandle to get a handle to the DLL that contains the function
    //and GetProcAddress to get a pointer to the function if available.

    fnIsWow64Process = (LPFN_ISWOW64PROCESS) GetProcAddress(
        GetModuleHandle(TEXT("kernel32")),"IsWow64Process");

    if(NULL != fnIsWow64Process)
    {
        if (!fnIsWow64Process(GetCurrentProcess(),&bIsWow64))
        {
            // Handle error...
        }
    }
    return bIsWow64;
}

void CProductVersion::DumpVersion()
{
	    ErrMessage( "Product Version: %d.%d.%d.%d\n",
        major,
        minor,
        revision,
        build
        );
}

std::string CProductVersion::GenerateVersionDocument()
{
	std::stringstream s;
    s << "<HTML>\n";
	s << "<HEAD><style type=\"text/css\" title=\"layout\" media=\"screen\">\n";
	s << GetCssStyle2();
	s << "\"style.css\"); </style> </HEAD><BODY>";
	//s << "<TABLE  width=\"100%\"> \n";
	//s << "<TR align=\"center\" valign=\"middle\" style=\"font-size: 200%\"> <TD>MTConnect Agent <BR>VERSION DESCRIPTION DOCUMENT </TD> </TR> \n";
	//s << "</TABLE> \n";
	s << "<P align=\"center\" valign=\"middle\" style=\"font-size: 200%\"> MTConnect Agent <BR>VERSION DESCRIPTION DOCUMENT  </P> \n";
	s << "<div style=\"width:100%;height:300px;\"></div> \n";
	s << "RELEASE DATE:  \n";
	s << currentDateTime() + "\n";
    s << "<H1 class=\"break\" align=\"center\">INTRODUCTION </H1>";
	s << "<H2> PURPOSE </H2> \n";
    s << "This document describes the software configuration for the MTConnect Agent. ";
    s << "The  MTConnect Agent is based on C++ Open Source technology and is developed by the MTConnect Institute, but can be modified and redistributed by 3rd parties. ";
	s << "The Agent software version -major, minor, revision, build - of the agent is ebmedded in the exe and is extracted and described in this document.";

    s << "<H2>SCOPE </H2>";
	s << "This document applies to the Product Acceptance Software of the Agent software.\n";
	s << "The agent is installed as an exe with the install version number emebedded in the name.\n";
	s << "The agent should install and run automatically (if installed correctly) as a Windows service.\n";
	s << "In the task manager the agent.exe is the executable file to look for.\n";
	s << "In the Windows Service Control Manager (SCM) the agent is launched automatically, and is given the service name on installation.\n";
 	s << "The default agent service name is 'MTConnectAgent'.\n";
 	s << "<BR>If running correctly, using the configured combination http and port, the agent should display XML describing the device:<BR>\n";
	s << "<PRE>\n";
	s << "http://127.0.0.1:5000/current";
	s << "</PRE>\n";

	s << "<H2>SECURITY REQUIREMENTS </H2>\n";
	s << "An administrator or equivalent must install and configure the agent.\n";
	
	s << "<H2>HARDWARE DESCRIPTION </H2>\n";
	s << "This agent distribution is PCs with 32 and 64 bit Microsoft Windows OS with Intel x86 chipsets.\n";
	s << "This agent will need a Ethernet Network Adapter to communicate device data to clients and to communicate with device Adapters.\n";

	s << "<H1 align=\"center\">SOFTWARE DESCRIPTION </H1>\n";
	s << "<H2>SYSTEM SOFTWARE</H2>\n";
	s << "It has been validated to work on Windows 2000, XP and 7 platforms on Intel x86 chipsets.\n";
	s << "Depending on the underlying architecture, either a 64 or 32 bit agent.exe will be installed.\n";
	s << "<BR>Windows NT and Linux require a different distribution.\n";
	s << "<H2>APPLICATION SOFTWARE</H2>\n";
	s << "<TABLE>\n";
	s << "<TR><TH>Product Name</TH><TH>Supplier</TH><TH>Version</TH></TR>\n";
	s << "<TR><TD>MTConnect Agent</TD><TD>MTConnect Institute<BR> Mods NIST</TD>";
    s<< StdStringFormat("<TD>%d.%d.%d.%d</TD>\n", major, minor, revision, build);
	s << "</TABLE>\n";
    s << "</BODY></HTML>\n";
	s << "<H1 align=\"center\"> SOFTWARE SUPPORT INFORMATION </H1>\n";
	s << " See https://github.com/mtconnect/cppagent/blob/master/README.md for more documentation on MTConnect Agent.\n";
	s << "<H2>SOFTWARE DOCUMENTATION</H2>\n";
	s << "<UL><LI><B>Document Title</B>  MTConnect Standard</LI>\n";
	s << "<LI><B>Vendor </B> MTConnect Institute</LI>\n";
	s << "<LI><B>Electronic Copy </B> https://github.com/mtconnect/standard</LI>\n";
	s << "<LI><B>Help/Bug/Issue</B> https://github.com/mtconnect/cppagent/issues/new</LI>\n";
	s << "</UL>\n";
	s << "<H2>SYSTEM SUPPORT DOCUMENTATION</H2>\n";
	s << "None.\n";
	s << "<H1 align=\"center\">INSTALLAION SOFTWARE INFORMATION </H1>\n";
	s << "Note, the installation describes installation of the MTConnect Agent on a Windows platform only. It is assumed to be a Windows 2000, XP, or 7/8 installation. \n";
	s << "It is known that this installation will not work on Windows NT system, and has not been tried on a Windows 95 or 98 or ME system. \n";
	s << "Before you process, installation of the MTConnect Agent will require administrator priviledges on a Windows 7 platform. \n";
	s << "<OL>\n";
	s << "<LI> You will need to double click the agent install software, MTConnectAgentInstallxx.xx.exe, where xx.xx is the distributed version of the agent.\n" ;
	s <<" The installation uses the NSIS software package. NSIS stands for the Nullsoft Scriptable Install System (NSIS) and is a script-driven installation system for Microsoft Windows with minimal overhead backed by Nullsoft, the creators of Winamp.\n";
	s <<" NSIS is free software released under a combination of free software licenses, primarily the zlib license.\n";
	s << "The Install is divided into Pages, with each Page either prompting the user for intput, or being informative, or both. \n";
	s << "</LI> <LI> The first screen that will appear is a Welcome page. Click Next. At any time you can stop the install by clicking the Cancel button.  \n";
	s << "</LI><LI> The second screen is the License Agreement page. This outlines the terms of agreement for using the software.\n" ;
	s << "In effect there is no copyright. The License Agreement allows the right to distribute copies and modified versions of the Agent and but does NOT require that the same rights be preserved in modified versions of the work. \n";
	s << "</LI><LI> The third screen is the Installation Directory page. The user is prompted to select a installation directory. \n";
	s << "The default directory is C:\\Program Files\\MTConnect\\MTConnectAgent for either 32 or 64 bit installs. The Instsall program will determine the underlying architecture. \n";
	s << "Note, unlike some application exe 32 bit agents will  not run on 64 bit machines. \n";
	s << " Two agents could be installed at the same time, as long as they have different service names and installation diretories. \n";
	s << " The installation over an existing directory is not guaranteed to overwrite older versions (even though it should). \n";
	s << "</LI><LI> After the user clicks Next, the fourth page details the installation of all the file is done, but has not been configured. \n";
	s <<" Again, the installation program determines whether to use a 32 or 64 bit installation exe. \n";
	s << "</LI><LI> The  fifth page prompts the user to configure the Agent specific parameters. \n";
	s << "The User specifies service name, http port, delay, and debug level, which are agent specific parameters. \n";
	s << "Note, You have the option of starting the agent service by checking the start agent checkbox. If this is your first agent installatino, it may be preferable to uncheck the box, and manually check the installation parameters. \n";
	s << "The user clicks Next and now the adapter configuration installation will then be executed to configure the remaining portions of agent.cfg and devices.xml in the installation directory. \n";
	s << "</LI><LI> A flash screen alerts the user that the Adapter Wizard is going to run. \n";
	s << "Now, adapters will be configured. 	Adapter configuration is a NSIS install program that DOES NO INSTALLING. Instead, it prompts the users to specify the set of Adatpers that provide SHDR input to the Agent, and then creates a Devices.xml file and the Adapter configuration part of the Agent.cfg file. \n";
	s << " Advanced users should read https://github.com/mtconnect/adapter/blob/master/MTC_SHDR.docx for information on the SHDR protocol. Note, SHDR is not an MTConnect standard, but is widely used as a back-end for device communication. \n";
	s << "<OL type=a> <LI> A Welcome page offers the user a Adapter configuration greeting. Clicking Next is all that is needed. \n";
	s << "</LI><LI> The second Adapter configuration page, queries the user to select a vendor, machine type and version. \n";
	s << "You must select an MTConnect to match your machine tool vendor CNC, and for which MTConnect templates are available. \n";
	s << "Device descriptions are currently organized into MachineTools, Turning, or Robots. There are not many models, and it is unclear if this is the correct model for specifying MTConnect Adapters. \n";
	s << "</LI><LI> The next Adapter configuration screen asks the necessary information  to configure an Adapter: Device Name, Type, IP and Port. \n";
	s << "The manufacturer and model have already been selected. \n";
	s << "When the users clicks Apply, an adapter.cfg entry and the devices.xml piece are saved, for later file writing. \n";

	s << "The user clicks Apply button to save the configuration, and will take you back a slide to append a new adapter to your configuration. \n";
	s << "The user clicks Done button completes the Adapter configuration process, writes the configuration files and returns to the Agent installation wizard. \n";
	s << "</LI></OL> \n";

	s << "<LI> The  final Agent installation page is the Finish Page, which signals the end of the installation process. \n";
	s << "The user clicks Finish, and if the user selected start the agent, the agent service will be started. \n";
	s << "</LI></OL>\n";
	s << "There are several steps to insure that the Agent installed correctly. \n";
	s << "<OL><LI> First, you will need to double check that the MTConnectAgent installed correctly in the Windows Service Manager (SCM). \n";
	s << " To check the service, right click ont he My Computer and select Manage. You may need to provide administrator credentials, so be prepared. \n";
	s << "Assuming you can manage My Computer, double click 'Services and Applications' and then double click 'Services'. \n";
	s << "At this point, you should see a list of all the services in your Windows platform. You will need to scroll down to the service name (or default name) provided during the Agent installation. \n";
	s << "We will assume the default name, 'MTConnectAgent', and scroll down to this entry, double click the name and a dialog popup describing the service parameters should appear. \n";
	s << "In the popup, you should be on 'General' tab and the see the following: \n";
	s << "<OL type=i> \n";
	s << "<LI> Service Name should say MTConnectAgent (default) or the service name provided when installing the agent. </LI> \n";
	s << "<LI> Startup type as automatic </LI> \n";
	s << "<LI>Path to executable as (the default assuming you didnt change the install directory) C:\\Program Files\\MTConnect\\MTConnectAgent\\agent.exe \n";
	s << "<LI> Service Status should be stopped. If you had checked the service startup it should say started. \n";
	s << "<LI> The tabs Logon, Recovery and Dependencies can be ignored. \n";
	s << "</OL> \n";
	s << "<LI>If you see all the correct parameters, you can then start the Agent, by clicking the Start button. A popup dialog should appear and within a minute or so, the service should be started. \n";
	s << "Close the Properties popup, and iF the agent status has not changed to 'Started' problems occured. \n";
	s << "Assuming the Agent service started, you can now see XML data from your MTConnect Adapter(s) (really devices). \n";
	s << "To see the output, open a web browser, (such as Internet Explorer), and type into the address bar: \n";
	s << "<PRE> http://127.0.0.1:5000/current</PRE> \n";
	s << "You should see data, as opposed to 'Unable to connect' or 'This page cannot be displayed' in the web browser. \n";
	s << "If you dont see data the service is not running, or you configured the HTTP port in the agent to something besides 5000. \n";
	s << "If you see data, but all the data is UNAVAILABLE, this is also most likely bad. \n";
	s << "You see all UNAVAILABLE data either: \n";
	s << "<OL type=i> \n";
	s << "<LI> No adapters were configured. </LI> \n";
	s << "<LI> The adapters is down, and no data is available. To check this, telnet to ip/port of the adapter, you should see output. </LI> \n";
	s << "<LI> The adapters data blocked by the firewall. Firewall can be on either the host or the remote adapter computer.  To check this, telnet to ip/port of the adapter, you should see output. \n";
	s << "If you see data, the firewall is most likely preventing the agent from remote access. \n";
	s << "If you don't see data, (and you can 'ping' the remote computer, a firewall on the remote platform may be preventing an adapter from outputting SHDR to socket port 7878 (the default). \n";

	s << "</OL> \n";
	s << "<LI> If you see data, then the agent has installed and been configured correctly. Now, just monitor the agent for a couple days to insure that it is booting correctly and operating properly. \n";
	s << "</OL> \n";
	s << "<H1 align=\"center\">UNINSTALLAION SOFTWARE INFORMATION </H1>\n";
	s << "There are two methods for uninstalling the Agent. Either use: <OL><LI>The Microsoft Windows Control Panel, \n";
	s << "or <LI>navigate to the install directory (default C:\\Program Files\\MTConnect\\MTConnectAgent) and click uninstall.exe. \n";
	s << "</OL><P>The control panel uninstall procedure wil be described (although once the uninstall program is activated, the procedure is the same.) \n";
	s << "<P>Uninstall uses the Windows mechanism: Start->Control Panale -> Programs and Features to fetch all the installed programs on the platform.\n";
	s << "Scroll down and find the MTConnectAgentxx.xx installation. Double click uninstall, YOU WILL NEED TO HAVE ADMINISTRATOR priviledges to uninstall on windows 7 machines. \n";
	s << "Three uninstall pages will diplay: Click Next, then Uninstall and finally Finish, and the Agent will be removed. \n";
	s << "<P>Uninstalling using the control panel is preferrable, but often some quirks appear. It may be necessary to manually top the MTConnectAgent service before uninstalling. \n";
	s << "Also, the Control Panel programs and features may not remove the MTConnectAgentxx.xx entry even though it has uninstalled the agent. A refresh may take care of this. \n";
	return s.str();
}
std::string CProductVersion::GetCssStyle2()
{
	std::stringstream s;	s << "	BODY\n";
	s << "{\n";
	s << "	FONT-FAMILY: \"Verdana\", sans-serif;\n";
	s << "counter-reset:section;\n";
	s << "}\n";
	s << "	h1{     font-size:20px;}\n";
	s << "	h2{     font-size:16px;}\n";
	s << "	h3{     font-size:12px;}\n";

	s << "    h1 {counter-reset:subsection;}\n";
	s << "    h1:before\n";
	s << "   {\n";
	s << "    counter-increment:section;\n";
	s << "    content:counter(section) \". \";\n";
	s << "       font-weight:bold;\n";
	s << "    }\n";
	s << "    h2:before\n";
	s << "   {\n";
	s << "    counter-increment:subsection;\n";
	s << "   content:counter(section) \".\" counter(subsection) \" \";\n";
	s << "   }\n";
	s << "  .break { page-break-before: always; }\n";
	return s.str();

}
std::string CProductVersion::GetCssStyle()
{
	std::stringstream s;
	s << "	BODY\n";
	s << "{\n";
	s << "counter-reset:section;\n";
	s << "	BACKGROUND-COLOR: white;\n";
	s << "	FONT-FAMILY: \"Verdana\", sans-serif;\n";
	s << "	FONT-SIZE: 100%;\n";
	s << "	MARGIN-LEFT: 0px;\n";
	s << "	MARGIN-TOP: 0px\n";
	s << "}\n";
	s << "P\n";
	s << "{\n";
	s << "	FONT-FAMILY: \"Verdana\", sans-serif;\n";
	s << "	FONT-SIZE: 70%;\n";
	s << "	LINE-HEIGHT: 12pt;\n";
	s << "	MARGIN-BOTTOM: 0px;\n";
	s << "	MARGIN-LEFT: 10px;\n";
	s << "	MARGIN-TOP: 10px\n";
	s << "}\n";
	s << ".note\n";
	s << "{\n";
	s << "	BACKGROUND-COLOR:  #ffffff;\n";
	s << "	COLOR: #336699;\n";
	s << "	FONT-FAMILY: \"Verdana\", sans-serif;\n";
	s << "	FONT-SIZE: 100%;\n";
	s << "	MARGIN-BOTTOM: 0px;\n";
	s << "	MARGIN-LEFT: 0px;\n";
	s << "	MARGIN-TOP: 0px;\n";
	s << "	PADDING-RIGHT: 10px\n";
	s << "}\n";
	s << ".infotable\n";
	s << "{\n";
	s << "	BACKGROUND-COLOR: #f0f0e0;\n";
	s << "	BORDER-BOTTOM: #ffffff 0px solid;\n";
	s << "	BORDER-COLLAPSE: collapse;\n";
	s << "	BORDER-LEFT: #ffffff 0px solid;\n";
	s << "	BORDER-RIGHT: #ffffff 0px solid;\n";
	s << "	BORDER-TOP: #ffffff 0px solid;\n";
	s << "	FONT-SIZE: 70%;\n";
	s << "	MARGIN-LEFT: 10px\n";
	s << "}\n";
	s << ".issuetable\n";
	s << "{\n";
	s << "	BACKGROUND-COLOR: #ffffe8;\n";
	s << "	BORDER-COLLAPSE: collapse;\n";
	s << "	COLOR: #000000;\n";
	s << "	FONT-SIZE: 100%;\n";
	s << "	MARGIN-BOTTOM: 10px;\n";
	s << "	MARGIN-LEFT: 13px;\n";
	s << "	MARGIN-TOP: 0px\n";
	s << "}\n";
	s << ".issuetitle\n";
	s << "{\n";
	s << "	BACKGROUND-COLOR: #ffffff;\n";
	s << "	BORDER-BOTTOM: #dcdcdc 1px solid;\n";
	s << "	BORDER-TOP: #dcdcdc 1px;\n";
	s << "	COLOR: #003366;\n";
	s << "	FONT-WEIGHT: normal\n";
	s << "}\n";
	s << ".header\n";
	s << "{\n";
	s << "	BACKGROUND-COLOR: #cecf9c;\n";
	s << "	BORDER-BOTTOM: #ffffff 1px solid;\n";
	s << "	BORDER-LEFT: #ffffff 1px solid;\n";
	s << "	BORDER-RIGHT: #ffffff 1px solid;\n";
	s << "	BORDER-TOP: #ffffff 1px solid;\n";
	s << "	COLOR: #000000;\n";
	s << "	FONT-WEIGHT: bold\n";
	s << "}\n";
	s << ".issuehdr\n";
	s << "{\n";
	s << "	BACKGROUND-COLOR: #E0EBF5;\n";
	s << "	BORDER-BOTTOM: #dcdcdc 1px solid;\n";
	s << "	BORDER-TOP: #dcdcdc 1px solid;\n";
	s << "	COLOR: #000000;\n";
	s << "	FONT-WEIGHT: normal\n";
	s << "}\n";
	s << ".issuenone\n";
	s << "{\n";
	s << "	BACKGROUND-COLOR: #ffffff;\n";
	s << "	BORDER-BOTTOM: 0px;\n";
	s << "	BORDER-LEFT: 0px;\n";
	s << "	BORDER-RIGHT: 0px;\n";
	s << "	BORDER-TOP: 0px;\n";
	s << "	COLOR: #000000;\n";
	s << "	FONT-WEIGHT: normal\n";
	s << "}\n";
	s << ".content\n";
	s << "{\n";
	s << "	BACKGROUND-COLOR: #e7e7ce;\n";
	s << "	BORDER-BOTTOM: #ffffff 1px solid;\n";
	s << "	BORDER-LEFT: #ffffff 1px solid;\n";
	s << "	BORDER-RIGHT: #ffffff 1px solid;\n";
	s << "	BORDER-TOP: #ffffff 1px solid;\n";
	s << "	PADDING-LEFT: 3px\n";
	s << "}\n";
	s << ".issuecontent\n";
	s << "{\n";
	s << "	BACKGROUND-COLOR: #ffffff;\n";
	s << "	BORDER-BOTTOM: #dcdcdc 1px solid;\n";
	s << "	BORDER-TOP: #dcdcdc 1px solid;\n";
	s << "	PADDING-LEFT: 3px\n";
	s << "}\n";
	s << "A:link\n";
	s << "{\n";
	s << "	COLOR: #cc6633;\n";
	s << "	TEXT-DECORATION: underline\n";
	s << "}\n";
	s << "A:visited\n";
	s << "{\n";
	s << "	COLOR: #cc6633;\n";
	s << "}\n";
	s << "A:active\n";
	s << "{\n";
	s << "	COLOR: #cc6633;\n";
	s << "}\n";
	s << "A:hover\n";
	s << "{\n";
	s << "	COLOR: #cc3300;\n";
	s << "	TEXT-DECORATION: underline\n";
	s << "}\n";
	//   body {counter-reset:section;}
	s << "    h1 {counter-reset:subsection;}\n";
	s << "    h1:before\n";
 	s << "   {\n";
	s << "    counter-increment:section;\n";
	s << "    content:counter(section) \". \";\n";
 	s << "       font-weight:bold;\n";
	s << "    }\n";
	s << "    h2:before\n";
 	s << "   {\n";
	s << "    counter-increment:subsection;\n";
 	s << "   content:counter(section) \".\" counter(subsection) \" \";\n";
 	s << "   }\n";
#if 0
	s << "H1\n";
	s << "{\n";
	s << "	BACKGROUND-COLOR: #003366;\n";
	s << "	BORDER-BOTTOM: #336699 6px solid;\n";
	s << "	COLOR: #ffffff;\n";
	s << "	FONT-SIZE: 130%;\n";
	s << "	FONT-WEIGHT: normal;\n";
	s << "	MARGIN: 0em 0em 0em -20px;\n";
	s << "	PADDING-BOTTOM: 8px;\n";
	s << "	PADDING-LEFT: 30px;\n";
	s << "	PADDING-TOP: 16px\n";
	s << "}\n";
	s << "H2\n";
	s << "{\n";
	s << "	COLOR: #000000;\n";
	s << "	FONT-SIZE: 80%;\n";
	s << "	FONT-WEIGHT: bold;\n";
	s << "	MARGIN-BOTTOM: 3px;\n";
	s << "	MARGIN-LEFT: 10px;\n";
	s << "	MARGIN-TOP: 20px;\n";
	s << "	PADDING-LEFT: 0px\n";
	s << "}\n";
	s << "H3\n";
	s << "{\n";
	s << "	COLOR: #000000;\n";
	s << "	FONT-SIZE: 80%;\n";
	s << "	FONT-WEIGHT: bold;\n";
	s << "	MARGIN-BOTTOM: -5px;\n";
	s << "	MARGIN-LEFT: 10px;\n";
	s << "	MARGIN-TOP: 20px\n";
	s << "}\n";
	s << "H4\n";
	s << "{\n";
	s << "	COLOR: #000000;\n";
	s << "	FONT-SIZE: 70%;\n";
	s << "	FONT-WEIGHT: bold;\n";
	s << "	MARGIN-BOTTOM: 0px;\n";
	s << "	MARGIN-TOP: 15px;\n";
	s << "	PADDING-BOTTOM: 0px\n";
	s << "}\n";
#endif
	s << "UL\n";
	s << "{\n";
	s << "	COLOR: #000000;\n";
	s << "	FONT-SIZE: 70%;\n";
	s << "	LIST-STYLE: square;\n";
	s << "	MARGIN-BOTTOM: 0pt;\n";
	s << "	MARGIN-TOP: 0pt\n";
	s << "}\n";
	s << "OL\n";
	s << "{\n";
	s << "	COLOR: #000000;\n";
	s << "	FONT-SIZE: 70%;\n";
	s << "	LIST-STYLE: square;\n";
	s << "	MARGIN-BOTTOM: 0pt;\n";
	s << "	MARGIN-TOP: 0pt\n";
	s << "}\n";
	s << "LI\n";
	s << "{\n";
	s << "	LIST-STYLE: square;\n";
	s << "	MARGIN-LEFT: 0px\n";
	s << "}\n";
	s << ".expandable\n";
	s << "{\n";
	s << "	CURSOR: hand\n";
	s << "}\n";
	s << ".expanded\n";
	s << "{\n";
	s << "	color: black\n";
	s << "}\n";
	s << ".collapsed\n";
	s << "{\n";
	s << "	DISPLAY: none\n";
	s << "}\n";
	s << ".foot\n";
	s << "{\n";
	s << "BACKGROUND-COLOR: #ffffff;\n";
	s << "BORDER-BOTTOM: #cecf9c 1px solid;\n";
	s << "BORDER-TOP: #cecf9c 2px solid\n";
	s << "}\n";
	s << ".settings\n";
	s << "{\n";
	s << "MARGIN-LEFT: 25PX;\n";
	s << "}\n";
	s << ".help\n";
	s << "{\n";
	s << "TEXT-ALIGN: right;\n";
	s << "margin-right: 10px;\n";
	s << "}\n";
	s << "table {\n";
	s << " 	BACKGROUND-COLOR: #f0f0e0;\n";
	s << "	BORDER-BOTTOM: #ffffff 0px solid;\n";
	s << "	BORDER-COLLAPSE: collapse;\n";
	s << "	BORDER-LEFT: #ffffff 0px solid;\n";
	s << "	BORDER-RIGHT: #ffffff 0px solid;\n";
	s << "	BORDER-TOP: #ffffff 0px solid;\n";
	s << "	FONT-SIZE: 90%;\n";
	s << "	MARGIN-LEFT: 10px\n";
	s << "  }\n";
	s << "td {\n";
	s << "	BACKGROUND-COLOR: #e7e7ce;\n";
	s << "	BORDER-BOTTOM: #ffffff 1px solid;\n";
	s << "	BORDER-LEFT: #ffffff 1px solid;\n";
	s << "	BORDER-RIGHT: #ffffff 1px solid;\n";
	s << "	BORDER-TOP: #ffffff 1px solid;\n";
	s << "	PADDING-LEFT: 3px\n";
	s << "  }\n";
	s << "th {\n";
	s << "	BACKGROUND-COLOR: #cecf9c;\n";
	s << "	BORDER-BOTTOM: #ffffff 1px solid;\n";
	s << "	BORDER-LEFT: #ffffff 1px solid;\n";
	s << "	BORDER-RIGHT: #ffffff 1px solid;\n";
	s << "	BORDER-TOP: #ffffff 1px solid;\n";
	s << "	COLOR: #000000;\n";
	s << "	FONT-WEIGHT: bold\n";
	s << "  }\n";
	s << "tr.rowhighlight {\n";
	s << "	FONT-WEIGHT: bold;\n";
	s << "	BACKGROUND-COLOR: #ffffff;\n";
	s << "	BORDER-BOTTOM: #ffffff 1px solid;\n";
	s << "	BORDER-LEFT: #ffffff 1px solid;\n";
	s << "	BORDER-RIGHT: #ffffff 1px solid;\n";
	s << "	BORDER-TOP: #ffffff 1px solid;\n";
	s << "	PADDING-LEFT: 3px\n";
	s << "  }\n";
	s << "  td.rowhighlight {\n";
	s << "	FONT-WEIGHT: bold;\n";
	s << "	BACKGROUND-COLOR: #ffffff;\n";
	s << "	BORDER-BOTTOM: #ffffff 1px solid;\n";
	s << "	BORDER-LEFT: #ffffff 1px solid;\n";
	s << "	BORDER-RIGHT: #ffffff 1px solid;\n";
	s << "	BORDER-TOP: #ffffff 1px solid;\n";
	s << "	PADDING-LEFT: 3px\n";
	s << "  }\n";
	return s.str();

}