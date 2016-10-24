
MTConnect Fanuc Installation
User Guide  

















								
Draft
Friday, October 21, 2016









#Table of Contents 

 

#Revision History
**Date Description Author Version**
<TABLE>
<TR>
<TD>Date<BR></TD>
<TD>Description<BR></TD>
<TD>Author<BR></TD>
<TD>Version<BR></TD>
</TR>
<TR>
<TD>1/17/2014<BR></TD>
<TD>Initial MTConnect Agent SDK <BR><BR></TD>
<TD>NIST – rough draft of MSI Windows installation for Fanuc MTConnect Adapter. Could use Ethernet or HSSB interface from Focas to CNC. Two separate MSI one for Ethernet, and one for HSSB are required. <BR></TD>
<TD>1.0<BR></TD>
</TR>
<TR>
<TD>10//121/2016<BR></TD>
<TD><BR></TD>
<TD>Port to MSVC 2015 community edition. Remove boost includes (which was not used) and other minor changes.<BR></TD>
<TD><BR></TD>
</TR>
</TABLE>


#FANUC SHDR INSTALLATION 
Installing the MTConnect Agent for the Fanuc CNC, involves installing a SHDR Adapter and a MTConnect Agent with the SHDR Adapter back end (which is typical of open source MTConnect Agents). The MTConnect Fanuc Adapter generates MTConnect blessed "SHDR" text that is supported by the open source MTConnect Agent (and is easier to go thru firewalls): Example of SHDR text is:


	2011-03-01T16:31:59.0617|power|ON|execution|EXECUTING|controllermode|AUTO

Note the timestamp, tags and values delimited by "|".

This section describes installing a custom SHDR Adapter developed for Fanuc iSeries (18, 21, 30, 31 models etc) that uses a Focas DLL library to retrieve data from a compatible Fanuc CNC. The Focas library *must* match it the controller and connection, that is, the type be it 15B, 15I or iSeries and the connection, beit HSSB or Ethernet. In addition, 32 bit and 64 bit DLL possibilities are now present in Focas DLLs.

The setup is quote/unquote straightforward, as there is a MSI (Microsoft installation) program to install the SHDR adapter program. There is no configuration as this is actually confusing with all the alternative Fanuc Focas options.

Figure 1 shows the MTConnect components involved (Device, Front End PC, Client Application). In the Boeing example, the CTLM Device is a Fanuc iSeries CNC. Because it is known that the open source C++ Adapter (not Agent) works for both 32 bit and 64 bit, the Adapter is compatible with Windows XP and Windows 7. Of note, if Windows 7 logging files will require increased folder permissions (and there is a batch script to enable this.)

NOTE it is very important that you know what Focas library is connecting to your Fanuc CNC. If you assume HSSB and its Ethernet or vice versa, IT WILL NOT WORK.




<CENTER>
![Figure1](./images/image1.jpg?raw=true)
</CENTER>

<p align="center">
**Figure 1 Fanuc Installation Overview**
</p>

##Installation Requirements
The Fanuc software installation requirements were:

Focas  library must be installed and located in the system Path so MTConnect can gather data and then output into SHDR format. It expects this DLL in the path, and if not will fail miserably. Since Fanuc charges for the DLL it cannot be distributed with the installer.
Ability to install programs, possibility to increase to folder's permissions (on Windows 7/8)
Ability to run MTConnect program as service. MTConnect has ability to install Adapter as service that will automically run, test for running CNC and monitor TCP connections as well.
How do I upgrade the MTConnect Agent service?
Stop the service.  Upgrade the Exe, and the Restart the Service.
Make sure telnet is installed on PC. (Look up on your favorite search engine)
Insure that Visual C++ 9.0 CRT (x86)  DLL is installed. (Should be installed with setup script). If not Download vcredist_x64.exe or vcredist_x86.exe depending on whether you are on 32 or 64 bit windows machine.
.

##Installation Steps
If Ethernet connection from Rename FanucSetupEthernet.msx to FanucSetupEthernet.msi FanucShdrStartup.msi from ftp://ftp.isd.mel.nist.gov/pub/MTConnect 

Run MSI, and you should see an Install Wizard step. You should safely just Next, Next, … , Finish
 


Installation
 1. Run FanucSetup.msi  The first installation page you should see is:
<CENTER>
![Figure2](./images/image2.jpg?raw=true)
</CENTER>

 2. Ignore where to install but make sure everyone is checked:

<CENTER>
![Figure3](./images/image3.jpg?raw=true)
</CENTER>


 3. Penultimate anxiety.
<CENTER>
![Figure4](./images/image4.jpg?raw=true)
</CENTER>

 4. Installation of MTConnect Fanuc HSSB Adapter 
<CENTER>
![Figure5](./images/image5.jpg?raw=true)
</CENTER>

 5. You must have Fanuc Focas HSSB DLL installed in Windows\System32 folder: 
<CENTER>
![Figure6](./images/image6.jpg?raw=true)
</CENTER>



 6. The final Installation Complete Dialog is issued to user. If you see the following splash screen mildly rejoice.


<CENTER>
![Figure7](./images/image7.jpg?raw=true)
</CENTER>


<CENTER>
![Figure8](./images/image8.jpg?raw=true)
</CENTER>


You should see the MTConnectFanucHSSBAdapter in the Service Control Manager (right click Computer icon, click Manage  - you will need "admin" rights to use it). Once in computer management, select Services under "Services and Applications". You should see the service MTConnectFanucHSSBAdapter – maybe started maybe not, maybe not installed at all.
<CENTER>
![Figure9](./images/image9.jpg?raw=true)
</CENTER>


For example, if you don't see the fwlib32.dll missing popup box, you will have to manaually install the MTConnect HSSD Adapter Service. 

If the Service Installation of the MTConnectFanucHSSBAdapter did not happen, you will need to navigate to the installation folder (e.g., C:\Program Files (x86)\MTConnect\FanucX32MTConnectAdapter) , (which depends on XP or Win 7 installation.) Then,  UAC privileges are required to install the Adapter on you own with a batch file. Right click on "InstallAdapter.bat" and Run as Administrator (works for both Win Xp or Windows 7).
<CENTER>
![Figure10](./images/image10.jpg?raw=true)
</CENTER>

If you see:
<CENTER>
![Figure11](./images/image11.jpg?raw=true)
</CENTER>

Then you cannot use the focas adapter until you get product installed with the DLL

 7. Once installed into the windows service control manager (SCM) you need to verify that the MTConnect Adapter has indeed been installed. The software developers only work a little on this, so should NEVER trust them. Open the service: right click manage on the Computer icon. (It will ask for UAC permission by asking for admin user/password on windows 7/8) You should see MTCFanucAdapter, Its state could be manual or automatic, started or stopped. If manual, make automatic. If stopped, start. If not there, you may need to install "by hand" which requires going to
 "C:\Program Files (x86)\MTConnect\ FanucX32MTConnectAdapter" and running the batch InstallAdapter.bat which will hopefully install the adapter into the Windows SCM. If not, please contact john.michaloski@nist.gov 
<CENTER>
![Figure12](./images/image12.jpg?raw=true)
</CENTER>


 8. Now, you can see the properties of MTConnectFanucHSSBAdapter which should show you the location of the program (exe)  in  "C:\Program Files (x86)\MTConnect\FanucInstall". Startup type should be automatic, so that every time you boot the PC, it will start the MTConnect Adapter. It should also give you service status of "Started" with Stop highlighted. If not start the service, please contact john.michaloski@nist.gov 

 
##UnInstallation Steps
Bring up control panel and remove programs  
Uninstall **_FanucMTConnectAdapter** 
Please note, this has to stop and remove a windows service 
You will need UAC privileges to allow you to uninstall on window 7 or 8. 


<CENTER>
![Figure13](./images/image13.jpg?raw=true)
</CENTER>




#Followup

Since the Fanuc MTConnect Adapter is a service, and already hand configured (if any configuration) you can navigate to the folder:

	C:\Program Files (x86)\MTConnect\FanucInstall

stop the service from the Windows SCM (right click Manage on My Computer and navigate to services, specifically MTCFanucAdapter), 
replace the EXE

	C:\Program Files (x86)\MTConnect\FanucInstall\MTCFanucAdapter.exe
and then start the service.


##Fanuc Adapter Logic

<CENTER>
![Figure14](./images/image14.jpg?raw=true)
</CENTER>

<p align="center">

</p>
##Software Architecture
<p align="center">

</p>
Fanuc HSSB Focas library with MTConnect Adapter using Port 7878 to communicate with MTConnect Agent:
<CENTER>
![Figure15](./images/image15.jpg?raw=true)
</CENTER>


When you look at the MTConnect Adapter telnet output, it will have a prepended "Device name:" to each tag. The Agent looks for the prepended device id: and will strip it out. e.g., 

	2014-06-25T13:55:22|DeviceA:controllermode|AUTOMATIC
If you leave the DeviceName blank during setup, the mobile device stuff described above will be ignored.



Autogenerated from Microsoft Word by [Word2Markdown](https://github.com/johnmichaloski/SoftwareGadgets/tree/master/Word2Markdown)