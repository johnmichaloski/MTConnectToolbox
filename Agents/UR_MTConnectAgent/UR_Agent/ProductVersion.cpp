
//
// ProductVersion.cpp
//

#include "StdAfx.h"
#include "ProductVersion.h"
#include "NIST/StdStringFcn.h"
#include <atltrace.h>
#include <sstream>
#define ErrMessage    ATLTRACE

#pragma comment(lib, "Version.lib")

#include <time.h>

// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
std::string currentDateTime ( )
{
    time_t    now = time(0);
    struct tm tstruct;
    char      buf[80];

    tstruct = *localtime(&now);
    strftime(buf, sizeof( buf ), "%Y-%m-%d", &tstruct);

    return buf;
}
std::string CProductVersion::GetWindowsVersionName ( )
{
    OSVERSIONINFO osver;

    osver.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
    GetVersionEx(&osver);

    if ( osver.dwPlatformId == 2 )
    {
        if ( ( osver.dwMajorVersion == 5 ) && ( osver.dwMinorVersion == 1 ) )
        {
            return "Windows Server 2003 or Windows XP";
        }

        if ( ( osver.dwMajorVersion == 5 ) && ( osver.dwMinorVersion == 2 ) )
        {
            return "Windows Server 2003 with SP1 or Windows XP with SP2";
        }

        if ( ( osver.dwMajorVersion == 6 ) && ( osver.dwMinorVersion == 0 ) )
        {
            return "Windows Vista";
        }

        if ( ( osver.dwMajorVersion == 6 ) && ( osver.dwMinorVersion == 1 ) )
        {
            return "Windows 2008 R2 or Windows 7";
        }

        if ( ( osver.dwMajorVersion == 6 ) && ( osver.dwMinorVersion == 2 ) )
        {
            return "Windows 8";
        }
    }

    return "";
}
CProductVersion::CProductVersion(void)
{
    major    = 0;
    minor    = 0;
    revision = 0;
    build    = 0;
}
CProductVersion::~CProductVersion(void) { }
std::string CProductVersion::GenerateVersionTable (std::string versionstr)
{
    std::string html;

    html += "<?xml version=\"1.0\"?> \n";
    html += "<?xml-stylesheet type=\"text/xsl\" href=\"#stylesheet\"?>\n";
    html += "<!DOCTYPE doc [\n";
    html += "<!ATTLIST xsl:stylesheet\n";
    html += "  id    ID    #REQUIRED>\n";
    html += "]>\n";
    html += "<doc>\n";
    html += "<xsl:stylesheet version=\"1.0\"\n";
    html += "                id=\"stylesheet\"\n";
    html
         += "                xmlns:xsl=\"http://www.w3.org/1999/XSL/Transform\">\n";
    html += "  <xsl:output method=\"html\"/>\n";
    html += "  <xsl:template match=\"xsl:stylesheet\" />\n";
    html += "  <xsl:template match=\"versions\">\n";
    html += "    <html>\n";
    html += "      <head><title>title of embedded stylesheet</title></head>\n";

    html += "      <body><h1>Version Information </h1><table border=\"1\"><tr> "
            "<th>Version</th></tr>\n";
    html += "        <xsl:apply-templates/>\n";
    html += "      </table></body>\n";
    html += "    </html>\n";
    html += "  </xsl:template>\n";
    html += "  <xsl:template match=\"version\">\n";
    html += "    <tr><td><xsl:value-of select=\".\"/></td></tr>\n";
    html += "  </xsl:template>\n";

    html += "</xsl:stylesheet>\n";
    html += versionstr;
    html += "</doc>\n";

    return html;
}
std::string CProductVersion::GenerateXSLTVersion (std::string versionstr)
{
    std::string html;

    html += "<?xml version=\"1.0\"?> \n";
    html += "<?xml-stylesheet type=\"text/xsl\" href=\"#stylesheet\"?>\n";
    html += "<!DOCTYPE doc [\n";
    html += "<!ATTLIST xsl:stylesheet\n";
    html += "  id    ID    #REQUIRED>\n";
    html += "]>\n";
    html += "<doc>\n";
    html += "<xsl:stylesheet version=\"1.0\"\n";
    html += "                id=\"stylesheet\"\n";
    html
         += "                xmlns:xsl=\"http://www.w3.org/1999/XSL/Transform\">\n";
    html += "  <xsl:output method=\"html\"/>\n";
    html += "  <xsl:template match=\"xsl:stylesheet\" />\n";
    html += "  <xsl:template match=\"versions\">\n";
    html += "    <html>\n";

    //	html+="      <head><title>title of embedded
    // stylesheet</title></head>\n";

    // /	html+="      <body>\n";
    html += "        <xsl:apply-templates/>\n";

    //	html+="      </body>\n";
    html += "    </html>\n";
    html += "  </xsl:template>\n";
    html += "  <xsl:template match=\"version\">\n";
    html += "   <xsl:value-of select=\".\"/>\n";
    html += "  </xsl:template>\n";

    html += "</xsl:stylesheet>\n";
    html += versionstr;
    html += "</doc>\n";

    return html;
}
std::string CProductVersion::GetUR_InstallVersion (std::string configfilename)
{
    std::string contents;

    ReadFile(configfilename, contents);
    std::size_t found = contents.find("\"ProductVersion\"");

    if ( found == std::string::npos )
    {
        return "";
    }
    contents = contents.substr(found);
    found    = contents.find("\n");

    if ( found == std::string::npos )
    {
        return "";
    }
    contents = contents.substr(0, found);
    found    = contents.find(":");

    if ( found == std::string::npos )
    {
        return "";
    }
    contents = contents.substr(found + 1);                 //     = "8:1.0.4"
    // remove trailing "
    return contents.substr(0, contents.size( ) - 1);
}
TCHAR *CProductVersion::GetMSVCVersion (int n)
{
    switch ( n )
    {
    case 1800:
        {
            return TEXT("Visual Studio 2013 - MSVC++ 12.0 ");
        }

    case 1700:
        {
            return TEXT("Visual Studio 2012 - MSVC++ 11.0");
        }

    case 1600:
        {
            return TEXT("Visual Studio 2010 - MSVC++ 10.0");
        }

    case 1500:
        {
            return TEXT("Visual Studio 2008 - MSVC++ 9.0");
        }

    case 1400:
        {
            return TEXT("Visual Studio 2005 - MSVC++ 8.0");
        }

    case 1300:
        {
            return TEXT("Visual Studio 2003 - MSVC++ 7.0");
        }

    case 1200:
        {
            return TEXT("MSVC++ 6.0");
        }

    case 1100:
        return TEXT("MSVC++ 5.0");
    }

    return TEXT("Unknown MSVC");
}
void CProductVersion::GetFileVersion (TCHAR *pszFilePath)
{
    DWORD             dwSize        = 0;
    BYTE *            pbVersionInfo = NULL;
    VS_FIXEDFILEINFO *pFileInfo     = NULL;
    UINT              puLenFileInfo = 0;

    // get the version info for the file requested
    dwSize = GetFileVersionInfoSize(pszFilePath, NULL);

    if ( dwSize == 0 )
    {
        ErrMessage("Error in GetFileVersionInfoSize: %d\n", GetLastError( ));
        return;
    }

    pbVersionInfo = new BYTE[dwSize];

    if ( !GetFileVersionInfo(pszFilePath, 0, dwSize, pbVersionInfo) )
    {
        ErrMessage("Error in GetFileVersionInfo: %d\n", GetLastError( ));
        delete[] pbVersionInfo;
        return;
    }

    if ( !VerQueryValue(pbVersionInfo, TEXT("\\"), (LPVOID *) &pFileInfo,
                        &puLenFileInfo) )
    {
        ErrMessage("Error in VerQueryValue: %d\n", GetLastError( ));
        delete[] pbVersionInfo;
        return;
    }

// pFileInfo->dwFileVersionMS is usually zero. However, you should check
// this if your version numbers seem to be wrong
#if 0
    ErrMessage("File Version: %d.%d.%d.%d\n",
               ( pFileInfo->dwFileVersionLS >> 24 ) & 0xff,
               ( pFileInfo->dwFileVersionLS >> 16 ) & 0xff,
               ( pFileInfo->dwFileVersionLS >> 8 ) & 0xff,
               ( pFileInfo->dwFileVersionLS >> 0 ) & 0xff
               );

    // pFileInfo->dwProductVersionMS is usually zero. However, you should check
    // this if your version numbers seem to be wrong

    printf("Product Version: %d.%d.%d.%d\n",
           ( pFileInfo->dwProductVersionLS >> 24 ) & 0xff,
           ( pFileInfo->dwProductVersionLS >> 16 ) & 0xff,
           ( pFileInfo->dwProductVersionLS >> 8 ) & 0xff,
           ( pFileInfo->dwProductVersionLS >> 0 ) & 0xff
           );
#endif
}
void CProductVersion::GetVersionInfo (std::string filename)
{
    GetFileVersion((TCHAR *) filename.c_str( ));
    DWORD  verHandle = NULL;
    UINT   size      = 0;
    LPBYTE lpBuffer  = NULL;
    DWORD  verSize   = GetFileVersionInfoSize((TCHAR *) filename.c_str( ), &verHandle);

    if ( verSize != NULL )
    {
        LPSTR verData = new char[verSize];

        if ( GetFileVersionInfo((TCHAR *) filename.c_str( ), verHandle, verSize,
                                verData) )
        {
            if ( VerQueryValue(verData, "\\", ( VOID FAR * FAR * ) & lpBuffer, &size) )
            {
                if ( size )
                {
                    VS_FIXEDFILEINFO *verInfo = (VS_FIXEDFILEINFO *) lpBuffer;

                    if ( IsWow64( ) )
                    {
                        // 64 bit build
                        major    = ( verInfo->dwProductVersionMS >> 16 ) & 0xff;
                        minor    = ( verInfo->dwProductVersionMS >> 0 ) & 0xff;
                        revision = ( verInfo->dwProductVersionLS >> 16 ) & 0xff;
                        build    = ( verInfo->dwProductVersionLS >> 0 ) & 0xff;
                    }
                    else
                    {
                        // 32 bit build
                        major    = HIWORD(verInfo->dwProductVersionMS);
                        minor    = LOWORD(verInfo->dwProductVersionMS);
                        revision = HIWORD(verInfo->dwProductVersionLS);
                        build    = LOWORD(verInfo->dwProductVersionLS);
                    }
                }
            }
        }
    }
}
#if 1
typedef BOOL ( WINAPI * LPFN_MyISWOW64PROCESS )(HANDLE, PBOOL);
static LPFN_MyISWOW64PROCESS fnIsWow64Process;

bool CProductVersion::IsWow64 ( )
{
    BOOL bIsWow64 = FALSE;

    // IsWow64Process is not available on all supported versions of Windows.
    // Use GetModuleHandle to get a handle to the DLL that contains the function
    // and GetProcAddress to get a pointer to the function if available.

    fnIsWow64Process = (LPFN_MyISWOW64PROCESS) GetProcAddress(
        GetModuleHandle(TEXT("kernel32")), "IsWow64Process");

    if ( NULL != fnIsWow64Process )
    {
        /**
        A pointer to a value that is set to TRUE if the process is running under
        WOW64.
        If the process is running under 32-bit Windows, the value is set to FALSE.
        If the process is a 64-bit application running under 64-bit Windows, the
        value is also set to FALSE.
        THIS IS NOT WHAT I WANT!
        */
        if ( !fnIsWow64Process(GetCurrentProcess( ), &bIsWow64) )
        {
            OutputDebugString("CProductVersion::IsWow64() Error:  "
                              "fnIsWow64Process(GetCurrentProcess(),&bIsWow64))\n");
        }
        else
        {
            bIsWow64 = TRUE;
        }
    }
    return bIsWow64;
}
#endif
void CProductVersion::DumpVersion ( )
{
    ErrMessage("Product Version: %d.%d.%d.%d\n", major, minor, revision, build);
}
std::string CProductVersion::GenerateVersionDocument ( )
{
    std::stringstream s;

    //   s << "<HTML>\n";
    s << "<HEAD><style type=\"text/css\" title=\"layout\" media=\"screen\">\n";
    s << GetCssStyle2( );
    s << "\"style.css\"); </style> </HEAD><BODY>";

    // s << "<TABLE  width=\"100%\"> \n";
    // s << "<TR align=\"center\" valign=\"middle\" style=\"font-size: 200%\">
    // <TD>MTConnect Agent <BR>VERSION DESCRIPTION DOCUMENT </TD> </TR> \n";
    // s << "</TABLE> \n";
    s << "<P align=\"center\" valign=\"middle\" style=\"font-size: 200%\"> "
         "MTConnect Agent <BR>VERSION DESCRIPTION DOCUMENT </BR> </P> \n";
    s << "<div style=\"width:100%;height:300px;\"></div> \n";
    s << "<P>RELEASE DATE: \n";
    s << currentDateTime( ) + "</P>\n";
    s << "<H1 class=\"break\" align=\"center\">INTRODUCTION </H1>";
    s << "<H2> PURPOSE </H2> \n";
    s << "<P>";
    s << "This document describes the software configuration for the MTConnect "
         "Agent. ";
    s << "The  MTConnect Agent is based on C++ Open Source technology and is "
         "developed by the MTConnect Institute, but can be modified and "
         "redistributed by 3rd parties. ";
    s << "The Agent software version -major, minor, revision, build - of the "
         "agent is ebmedded in the exe and is extracted and described in this "
         "document.";
    s << "</P>";
    s << "<H2>SCOPE </H2>";
    s << "<P>";
    s << "This document applies to the Product Acceptance Software of the Agent "
         "software.\n";
    s << "The agent is installed as an exe with the install version number "
         "emebedded in the name.\n";
    s << "The agent should install and run automatically (if installed "
         "correctly) as a Windows service.\n";
    s << "In the task manager the agent.exe is the executable file to look "
         "for.\n";
    s << "In the Windows Service Control Manager (SCM) the agent is launched "
         "automatically, and is given the service name on installation.\n";
    s << "The default agent service name is 'MTConnectAgent'.\n";
    s << "<BR>If running correctly, using the configured combination http and "
         "port, the agent should display XML describing the device:</BR>\n";
    s << "</P>";
    s << "<PRE>\n";
    s << "http://127.0.0.1:5000/current";
    s << "</PRE>\n";

    s << "<H2>SECURITY REQUIREMENTS </H2>\n";
    s << "<P>";
    s << "An administrator or equivalent must install and configure the agent.\n";
    s << "</P>";

    s << "<H2>HARDWARE DESCRIPTION </H2>\n";
    s << "<P>";
    s << "This agent distribution is PCs with 32 and 64 bit Microsoft Windows OS "
         "with Intel x86 chipsets.\n";
    s << "This agent will need a Ethernet Network Adapter to communicate device "
         "data to clients and to communicate with device Adapters.\n";
    s << "</P>";

    s << "<H1 align=\"center\">SOFTWARE DESCRIPTION </H1>\n";
    s << "<H2>SYSTEM SOFTWARE</H2>\n";
    s << "<P>";
    s << "It has been validated to work on Windows 2000, XP and 7 platforms on "
         "Intel x86 chipsets.\n";
    s << "Depending on the underlying architecture, either a 64 or 32 bit "
         "agent.exe will be installed.\n";
    s << "</P>";
    s << "<BR>Windows NT and Linux require a different distribution.</BR>\n";
    s << "<H2>APPLICATION SOFTWARE</H2>\n";
    s << "<TABLE>\n";
    s << "<TR><TH>Product Name</TH><TH>Supplier</TH><TH>Version</TH></TR>\n";
    s << "<TR><TD>MTConnect Agent</TD><TD>MTConnect Institute<BR> Mods "
         "NIST</BR></TD>";
    s << StdStringFormat("<TD>%d.%d.%d.%d</TD></TR>\n", major, minor, revision,
                         build);
    s << "</TABLE>\n";
    s << "<H1 align=\"center\"> SOFTWARE SUPPORT INFORMATION </H1>\n";
    s << " See https://github.com/mtconnect/cppagent/blob/master/README.md for "
         "more documentation on MTConnect Agent.\n";
    s << "<H2>SOFTWARE DOCUMENTATION</H2>\n";
    s << "<UL><LI><B>Document Title</B>  MTConnect Standard</LI>\n";
    s << "<LI><B>Vendor </B> MTConnect Institute</LI>\n";
    s << "<LI><B>Electronic Copy </B> https://github.com/mtconnect/standard "
         "</LI>\n";
    s << "<LI><B>Help/Bug/Issue</B> "
         "https://github.com/mtconnect/cppagent/issues/new </LI>\n";
    s << "</UL>\n";
    s << "<H2>SYSTEM SUPPORT DOCUMENTATION</H2>\n";
    s << "<P>";
    s << "None.\n";
    s << "</P>";
    s << "<H1 align=\"center\">INSTALLAION SOFTWARE INFORMATION </H1>\n";
    s << "<P>";
    s << "Note, the installation describes installation of the MTConnect Agent "
         "on a Windows platform only. It is assumed to be a Windows 2000, XP, or "
         "7/8 installation. \n";
    s << "It is known that this installation will not work on Windows NT system, "
         "and has not been tried on a Windows 95 or 98 or ME system. \n";
    s << "Before you process, installation of the MTConnect Agent will require "
         "administrator priviledges on a Windows 7 platform. \n";
    s << "</P>";
    s << "<OL>\n";
    s << "<LI> You will need to double click the agent install software, "
         "MTConnectAgentInstallxx.xx.exe, where xx.xx is the distributed version "
         "of the agent.\n";
    s << " The installation uses the NSIS software package. NSIS stands for the "
         "Nullsoft Scriptable Install System (NSIS) and is a script-driven "
         "installation system for Microsoft Windows with minimal overhead backed "
         "by Nullsoft, the creators of Winamp.\n";
    s << " NSIS is free software released under a combination of free software "
         "licenses, primarily the zlib license.\n";
    s << "The Install is divided into Pages, with each Page either prompting the "
         "user for intput, or being informative, or both. \n";
    s << "</LI>\n ";
    s << "<LI> The first screen that will appear is a Welcome page. Click Next. "
         "At any time you can stop the install by clicking the Cancel button.  "
         "\n";
    s << "</LI>\n ";
    s << "<LI> The second screen is the License Agreement page. This outlines "
         "the terms of agreement for using the software.\n";
    s << "In effect there is no copyright. The License Agreement allows the "
         "right to distribute copies and modified versions of the Agent and but "
         "does NOT require that the same rights be preserved in modified "
         "versions of the work. \n";
    s << "</LI>\n ";
    s << "<LI> The third screen is the Installation Directory page. The user is "
         "prompted to select a installation directory. \n";
    s << "The default directory is C:\\Program Files\\MTConnect\\MTConnectAgent "
         "for either 32 or 64 bit installs. The Instsall program will determine "
         "the underlying architecture. \n";
    s << "Note, unlike some application exe 32 bit agents will  not run on 64 "
         "bit machines. \n";
    s << " Two agents could be installed at the same time, as long as they have "
         "different service names and installation diretories. \n";
    s << " The installation over an existing directory is not guaranteed to "
         "overwrite older versions (even though it should). \n";
    s << "</LI>\n ";
    s << "<LI> After the user clicks Next, the fourth page details the "
         "installation of all the file is done, but has not been configured. \n";
    s << " Again, the installation program determines whether to use a 32 or 64 "
         "bit installation exe. \n";
    s << "</LI>\n ";
    s << "<LI> The  fifth page prompts the user to configure the Agent specific "
         "parameters. \n";
    s << "The User specifies service name, http port, delay, and debug level, "
         "which are agent specific parameters. \n";
    s << "Note, You have the option of starting the agent service by checking "
         "the start agent checkbox. If this is your first agent installatino, it "
         "may be preferable to uncheck the box, and manually check the "
         "installation parameters. \n";
    s << "The user clicks Next and now the adapter configuration installation "
         "will then be executed to configure the remaining portions of agent.cfg "
         "and devices.xml in the installation directory. \n";
    s << "</LI>\n ";
    s << "<LI> A flash screen alerts the user that the Adapter Wizard is going "
         "to run. \n";
    s << "Now, adapters will be configured.   Adapter configuration is a NSIS "
         "install program that DOES NO INSTALLING. Instead, it prompts the users "
         "to specify the set of Adatpers that provide SHDR input to the Agent, "
         "and then creates a Devices.xml file and the Adapter configuration part "
         "of the Agent.cfg file. \n";
    s << " Advanced users should read "
         "https://github.com/mtconnect/adapter/blob/master/MTC_SHDR.docx for "
         "information on the SHDR protocol. Note, SHDR is not an MTConnect "
         "standard, but is widely used as a back-end for device communication. "
         "\n";
    s << "<OL type=\"a\">\n ";
    s << "<LI> A Welcome page offers the user a Adapter configuration greeting. "
         "Clicking Next is all that is needed. \n";
    s << "</LI>\n ";
    s << "<LI> The second Adapter configuration page, queries the user to select "
         "a vendor, machine type and version. \n";
    s << "You must select an MTConnect to match your machine tool vendor CNC, "
         "and for which MTConnect templates are available. \n";
    s << "Device descriptions are currently organized into MachineTools, "
         "Turning, or Robots. There are not many models, and it is unclear if "
         "this is the correct model for specifying MTConnect Adapters. \n";
    s << "</LI>\n ";
    s << "<LI> The next Adapter configuration screen asks the necessary "
         "information  to configure an Adapter: Device Name, Type, IP and Port. "
         "\n";
    s << "The manufacturer and model have already been selected. \n";
    s << "When the users clicks Apply, an adapter.cfg entry and the devices.xml "
         "piece are saved, for later file writing. \n";

    s << "The user clicks Apply button to save the configuration, and will take "
         "you back a slide to append a new adapter to your configuration. \n";
    s << "The user clicks Done button completes the Adapter configuration "
         "process, writes the configuration files and returns to the Agent "
         "installation wizard. \n";
    s << "</LI>\n";
    s << "</OL>\n";
    s << "</LI>\n";

    s << "<LI> The  final Agent installation page is the Finish Page, which "
         "signals the end of the installation process. \n";
    s << "The user clicks Finish, and if the user selected start the agent, the "
         "agent service will be started. \n";
    s << "</LI>\n";
    s << "</OL>\n";
    s << "<P>There are several steps to insure that the Agent installed "
         "correctly. </P>\n";

    s << "<OL>\n";
    s << "<LI> First, you will need to double check that the MTConnectAgent "
         "installed correctly in the Windows Service Manager (SCM). \n";
    s << " To check the service, right click ont he My Computer and select "
         "Manage. You may need to provide administrator credentials, so be "
         "prepared. \n";
    s << "Assuming you can manage My Computer, double click 'Services and "
         "Applications' and then double click 'Services'. \n";
    s << "At this point, you should see a list of all the services in your "
         "Windows platform. You will need to scroll down to the service name (or "
         "default name) provided during the Agent installation. \n";
    s << "We will assume the default name, 'MTConnectAgent', and scroll down to "
         "this entry, double click the name and a dialog popup describing the "
         "service parameters should appear. \n";
    s << "In the popup, you should be on 'General' tab and the see the "
         "following: \n";
    s << "<OL type=\"i\"> \n";
    s << "<LI> Service Name should say MTConnectAgent (default) or the service "
         "name provided when installing the agent. </LI> \n";
    s << "<LI> Startup type as automatic </LI> \n";
    s << "<LI>Path to executable as (the default assuming you didnt change the "
         "install directory) C:\\Program "
         "Files\\MTConnect\\MTConnectAgent\\agent.exe </LI> \n";
    s << "<LI> Service Status should be stopped. If you had checked the service "
         "startup it should say started. </LI> \n";
    s << "<LI> The tabs Logon, Recovery and Dependencies can be ignored. </LI> "
         "\n";
    s << "</OL> \n";
    s << "</LI>\n";
    s << "<LI>If you see all the correct parameters, you can then start the "
         "Agent, by clicking the Start button. A popup dialog should appear and "
         "within a minute or so, the service should be started. \n";
    s << "Close the Properties popup, and iF the agent status has not changed to "
         "'Started' problems occured. \n";
    s << "Assuming the Agent service started, you can now see XML data from your "
         "MTConnect Adapter(s) (really devices). \n";
    s << "To see the output, open a web browser, (such as Internet Explorer), "
         "and type into the address bar: \n";
    s << "</LI>\n";
    s << "You should see data, as opposed to 'Unable to connect' or 'This page "
         "cannot be displayed' in the web browser. \n";
    s << "If you dont see data the service is not running, or you configured the "
         "HTTP port in the agent to something besides 5000. \n";
    s << "If you see data, but all the data is UNAVAILABLE, this is also most "
         "likely bad. \n";
    s << "You see all UNAVAILABLE data either: \n";
    s << "<OL type=\"i\"> \n";
    s << "<LI> No adapters were configured. </LI> \n";
    s << "<LI> The adapters is down, and no data is available. To check this, "
         "telnet to ip/port of the adapter, you should see output. </LI> \n";
    s << "<LI> The adapters data blocked by the firewall. Firewall can be on "
         "either the host or the remote adapter computer.  To check this, telnet "
         "to ip/port of the adapter, you should see output. \n";
    s << "If you see data, the firewall is most likely preventing the agent from "
         "remote access. \n";
    s << "If you don't see data, (and you can 'ping' the remote computer, a "
         "firewall on the remote platform may be preventing an adapter from "
         "outputting SHDR to socket port 7878 (the default). \n";
    s << "</LI>\n";
    s << "</OL> \n";

    s << "<LI> If you see data, then the agent has installed and been configured "
         "correctly. Now, just monitor the agent for a couple days to insure "
         "that it is booting correctly and operating properly. \n";
    s << "</LI>\n";
    s << "</OL> \n";

    s << "<H1 align=\"center\">UNINSTALLAION SOFTWARE INFORMATION </H1>\n";
    s << "There are two methods for uninstalling the Agent. Either use: \n";
    s << "<OL><LI>The Microsoft Windows Control Panel, </LI>\n";
    s << "or <LI>navigate to the install directory (default C:\\Program "
         "Files\\MTConnect\\MTConnectAgent) and click uninstall.exe. </LI>\n";
    s << "</OL>\n";
    s << "<P>The control panel uninstall procedure wil be described (although "
         "once the uninstall program is activated, the procedure is the same.) "
         "\n";
    s << "</P>\n";
    s << "<P>Uninstall uses the Windows mechanism: Start->Control Panale -> "
         "Programs and Features to fetch all the installed programs on the "
         "platform.\n";
    s << "Scroll down and find the MTConnectAgentxx.xx installation. Double "
         "click uninstall, YOU WILL NEED TO HAVE ADMINISTRATOR priviledges to "
         "uninstall on windows 7 machines. \n";
    s << "Three uninstall pages will diplay: Click Next, then Uninstall and "
         "finally Finish, and the Agent will be removed. \n";
    s << "</P>\n";
    s << "<P>Uninstalling using the control panel is preferrable, but often some "
         "quirks appear. It may be necessary to manually top the MTConnectAgent "
         "service before uninstalling. \n";
    s << "Also, the Control Panel programs and features may not remove the "
         "MTConnectAgentxx.xx entry even though it has uninstalled the agent. A "
         "refresh may take care of this. \n";
    s << "</P>\n";
    s << "</BODY>\n";

    //   s << "</HTML>\n";
    return s.str( );
}
std::string CProductVersion::GetCssStyle2 ( )
{
    std::stringstream s;

    s << "	BODY\n";

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
    return s.str( );
}
std::string CProductVersion::GetCssStyle ( )
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
    s << "    BACKGROUND-COLOR: #f0f0e0;\n";
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
    return s.str( );
}
// From
// http://forum.codecall.net/topic/60159-how-to-determine-windows-operating-system-version-information-using-visual-c/
#pragma comment(lib, "User32.lib")

#define BUFSIZE    256

#define VISTA      TEXT("Vista")
#define WIN7       TEXT("Windows 7")
#define Win2K8     TEXT("Windows Server 2008")
typedef void ( WINAPI * PGETSYSTEMINFO )(LPSYSTEM_INFO);
typedef BOOL ( WINAPI * PGETPRODUCTINFO )(DWORD, DWORD, DWORD, DWORD, PDWORD);

std::string CProductVersion::GetOSDisplayString ( )
{
    OSVERSIONINFOEX osvi;
    SYSTEM_INFO     si;
    PGETSYSTEMINFO  pGetSystemInfo;
    PGETPRODUCTINFO pGetProductInfo;
    BOOL            bOsVersionInfoEx;
    DWORD           dwType;

    std::string pszOS;

    ZeroMemory(&si, sizeof( SYSTEM_INFO ));
    ZeroMemory(&osvi, sizeof( OSVERSIONINFOEX ));

    osvi.dwOSVersionInfoSize = sizeof( OSVERSIONINFOEX );

    if ( !( bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO *) &osvi) ) )
    {
        return "";
    }

    // Call GetNativeSystemInfo if supported or GetSystemInfo otherwise.

    pGetSystemInfo = (PGETSYSTEMINFO) GetProcAddress(
        GetModuleHandle(TEXT("kernel32.dll")), "GetNativeSystemInfo");

    if ( NULL != pGetSystemInfo )
    {
        pGetSystemInfo(&si);
    }
    else
    {
        GetSystemInfo(&si);
    }

    if ( ( VER_PLATFORM_WIN32_NT == osvi.dwPlatformId ) &&
         ( osvi.dwMajorVersion > 4 ) )
    {
        pszOS += TEXT("Microsoft ");

        // Test for the specific product.

        if ( osvi.dwMajorVersion == 6 )
        {
            if ( osvi.dwMinorVersion == 0 )
            {
                if ( osvi.wProductType == VER_NT_WORKSTATION )
                {
                    pszOS += TEXT("Windows Vista ");
                }
                else
                {
                    pszOS += TEXT("Windows Server 2008 ");
                }
            }

            if ( osvi.dwMinorVersion == 1 )
            {
                if ( osvi.wProductType == VER_NT_WORKSTATION )
                {
                    pszOS += TEXT("Windows 7 ");
                }
                else
                {
                    pszOS += TEXT("Windows Server 2008 R2 ");
                }
            }

            pGetProductInfo = (PGETPRODUCTINFO) GetProcAddress(
                GetModuleHandle(TEXT("kernel32.dll")), "GetProductInfo");

            pGetProductInfo(osvi.dwMajorVersion, osvi.dwMinorVersion, 0, 0, &dwType);

            switch ( dwType )
            {
            case PRODUCT_ULTIMATE:
                {
                    pszOS += TEXT("Ultimate Edition");
                }
                break;

            case 0x00000030:
                {
                    pszOS += TEXT("Professional");
                }
                break;

            case PRODUCT_HOME_PREMIUM:
                {
                    pszOS += TEXT("Home Premium Edition");
                }
                break;

            case PRODUCT_HOME_BASIC:
                {
                    pszOS += TEXT("Home Basic Edition");
                }
                break;

            case PRODUCT_ENTERPRISE:
                {
                    pszOS += TEXT("Enterprise Edition");
                }
                break;

            case PRODUCT_BUSINESS:
                {
                    pszOS += TEXT("Business Edition");
                }
                break;

            case PRODUCT_STARTER:
                {
                    pszOS += TEXT("Starter Edition");
                }
                break;

            case PRODUCT_CLUSTER_SERVER:
                {
                    pszOS += TEXT("Cluster Server Edition");
                }
                break;

            case PRODUCT_DATACENTER_SERVER:
                {
                    pszOS += TEXT("Datacenter Edition");
                }
                break;

            case PRODUCT_DATACENTER_SERVER_CORE:
                {
                    pszOS += TEXT("Datacenter Edition (core installation)");
                }
                break;

            case PRODUCT_ENTERPRISE_SERVER:
                {
                    pszOS += TEXT("Enterprise Edition");
                }
                break;

            case PRODUCT_ENTERPRISE_SERVER_CORE:
                {
                    pszOS += TEXT("Enterprise Edition (core installation)");
                }
                break;

            case PRODUCT_ENTERPRISE_SERVER_IA64:
                {
                    pszOS += TEXT("Enterprise Edition for Itanium-based Systems");
                }
                break;

            case PRODUCT_SMALLBUSINESS_SERVER:
                {
                    pszOS += TEXT("Small Business Server");
                }
                break;

            case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM:
                {
                    pszOS += TEXT("Small Business Server Premium Edition");
                }
                break;

            case PRODUCT_STANDARD_SERVER:
                {
                    pszOS += TEXT("Standard Edition");
                }
                break;

            case PRODUCT_STANDARD_SERVER_CORE:
                {
                    pszOS += TEXT("Standard Edition (core installation)");
                }
                break;

            case PRODUCT_WEB_SERVER:
                {
                    pszOS += TEXT("Web Server Edition");
                }
                break;
            }
        }

        if ( ( osvi.dwMajorVersion == 5 ) && ( osvi.dwMinorVersion == 2 ) )
        {
            if ( GetSystemMetrics(SM_SERVERR2) )
            {
                pszOS += TEXT("Windows Server 2003 R2, ");
            }
            else if ( osvi.wSuiteMask & VER_SUITE_STORAGE_SERVER )
            {
                pszOS += TEXT("Windows Storage Server 2003");
            }
            else if ( osvi.wSuiteMask & 0x00008000 )
            {
                pszOS += TEXT("Windows Home Server");
            }
            else if ( ( osvi.wProductType == VER_NT_WORKSTATION ) &&
                      ( si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ) )
            {
                pszOS += TEXT("Windows XP Professional x64 Edition");
            }
            else
            {
                pszOS += TEXT("Windows Server 2003, ");
            }

            // Test for the server type.
            if ( osvi.wProductType != VER_NT_WORKSTATION )
            {
                if ( si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 )
                {
                    if ( osvi.wSuiteMask & VER_SUITE_DATACENTER )
                    {
                        pszOS += TEXT("Datacenter Edition for Itanium-based Systems");
                    }
                    else if ( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
                    {
                        pszOS += TEXT("Enterprise Edition for Itanium-based Systems");
                    }
                }
                else if ( si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 )
                {
                    if ( osvi.wSuiteMask & VER_SUITE_DATACENTER )
                    {
                        pszOS += TEXT("Datacenter x64 Edition");
                    }
                    else if ( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
                    {
                        pszOS += TEXT("Enterprise x64 Edition");
                    }
                    else
                    {
                        pszOS += TEXT("Standard x64 Edition");
                    }
                }
                else
                {
                    if ( osvi.wSuiteMask & VER_SUITE_COMPUTE_SERVER )
                    {
                        pszOS += TEXT("Compute Cluster Edition");
                    }
                    else if ( osvi.wSuiteMask & VER_SUITE_DATACENTER )
                    {
                        pszOS += TEXT("Datacenter Edition");
                    }
                    else if ( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
                    {
                        pszOS += TEXT("Enterprise Edition");
                    }
                    else if ( osvi.wSuiteMask & VER_SUITE_BLADE )
                    {
                        pszOS += TEXT("Web Edition");
                    }
                    else
                    {
                        pszOS += TEXT("Standard Edition");
                    }
                }
            }
        }

        if ( ( osvi.dwMajorVersion == 5 ) && ( osvi.dwMinorVersion == 1 ) )
        {
            pszOS += TEXT("Windows XP ");

            if ( osvi.wSuiteMask & VER_SUITE_PERSONAL )
            {
                pszOS += TEXT("Home Edition");
            }
            else
            {
                pszOS += TEXT("Professional");
            }
        }

        if ( ( osvi.dwMajorVersion == 5 ) && ( osvi.dwMinorVersion == 0 ) )
        {
            pszOS += TEXT("Windows 2000 ");

            if ( osvi.wProductType == VER_NT_WORKSTATION )
            {
                pszOS += TEXT("Professional");
            }
            else
            {
                if ( osvi.wSuiteMask & VER_SUITE_DATACENTER )
                {
                    pszOS += TEXT("Datacenter Server");
                }
                else if ( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
                {
                    pszOS += TEXT("Advanced Server");
                }
                else
                {
                    pszOS += TEXT("Server");
                }
            }
        }

        // Include service pack (if any) and build number.

        if ( _tcslen(osvi.szCSDVersion) > 0 )
        {
            pszOS += TEXT(" ");
            pszOS += osvi.szCSDVersion;
        }

        pszOS += StdStringFormat(" (build %d)", osvi.dwBuildNumber);

        if ( osvi.dwMajorVersion >= 6 )
        {
            if ( si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 )
            {
                pszOS += TEXT(", 64-bit");
            }
            else if ( si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL )
            {
                pszOS += TEXT(", 32-bit");
            }
        }

        // _tprintf(_T("OS Version Information %s\n"), pszOS);
        return pszOS;
    }
    else
    {
        OutputDebugString(
            "This sample does not support this version of Windows.\n");
        return pszOS;
    }
}
