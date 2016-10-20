
#ZEISS LOGFILE README 
----

7/1/2016 10:32:20 AM
This **document** describes the data gathering for a Zeiss CMM machine that updates a tab separated log file. Multiple adapters to Zeiss software are possible, each contained within one MTConnect Agent.
The ZEISS Agent contains back end adapters that read a log file generated from the CMM periodically (typically when an event occurs within the CMM). The log file contains events and not samples, but all the events are time stamped and in absolute order of occurrence.
The file is _ specified_as a Window cross-platform file, so it must contain the PC or computer name. UNC is  short for Universal Naming Convention and specifies a Windows syntax to describe the location of a network resource, such as a shared file, directory, or printer. The UNC syntax for Windows systems has the generic form:
	\\ComputerName\SharedResource
In our case the SharedResource is a shared file that must be explicitly sharable. In order to use the UNC file, Microsoft File Operations: CreateFile, ReadFile and CloseFile are used as other generic C++ file operations did not work (but were originally tried.) UNC files on Windows seem to require Windows specific File operations. Note, the UNC file path must be accessible to other computers or it cannot be read.mInside the Agent are Adapters for each UNC file. Each Adapter runs as a thread, hence the distinction between 64 bit and 32 bit C++ solutions must be explicitly acknowledged in installing the binary exe. 
 *That is, 32-bit MTConnect agents do not on 64 bit platforms, although they may appear to.

In the Zeiss file, it logs major events in a UNC shared file. Below is a sample of the last line found in the Zeiss shared file. 
	Measurement Plan Name: 138Z4039-501
	Run Speed: 160 mm/s
	Measurement Start Time: 9:49:47 am
	Measurement End Time: 10:13:57 am
	Duration of Run: 00:24:21.0
	Date of Run: 2016-5-20 
Each run of a Zeiss inspection is logged, starting with a plan name, run speed and start time. When the inspection is done the end time, duration and date of run are logged. Unfortunately, the date of the run (inspection) occurs after the inspection has completed, thus, no error detection of runaway date or times can be done.  
There are three fields of interest, plan name, start time, and speed event.
<TABLE>
<TR>
<TD>Field
</TD>
<TD>Example
</TD>
</TR>
<TR>
<TD>Start timestamp
</TD>
<TD>9:48:56 am or pm
</TD>
</TR>
<TR>
<TD>End time
</TD>
<TD>9:58:56 am or pm
</TD>
</TR>
<TR>
<TD>date
</TD>
<TD>01/23/2014  
</TD>
</TR>
<TR>
<TD>Plan Name
</TD>
<TD>138Z4039-501
</TD>
</TR>
<TR>
<TD>Speed (mm/sec)
</TD>
<TD>160
</TD>
</TR>
</TABLE>
<TABLE>
<TR>
<TD>State
</TD>
<TD>Action
</TD>
</TR>
<TR>
<TD>End time detected
</TD>
<TD>power=ON<BR>controllermode=MANUAL<BR>execution=IDLE<BR>program = “”<BR>feed=0
</TD>
</TR>
<TR>
<TD>Start time
</TD>
<TD>power=ON<BR>controllermode=AUTOMATIC<BR>execution=EXECUTING<BR>program = plan name<BR>feed = speed
</TD>
</TR>
<TR>
<TD>Side effects
</TD>
<TD>RPM and xyz move if automatic and executing
</TD>
</TR>
<TR>
<TD>
</TD>
<TD>
</TD>
</TR>
</TABLE>
Because of the deficiency of the MTConnect state logic, some side effects are generated to make the controller appear to be operating: positions for x,y,z and RPM change after every update if the controller is in automatic mode and executing.
Some MTConnect tags are updated indepently of the contents of the log file, these include:
<TABLE>
<TR>
<TD>SIDE EFFECTS
</TD>
</TR>
<TR>
<TD>power
</TD>
<TD>ON/OFF depending if log file found
</TD>
</TR>
<TR>
<TD>AVAIL
</TD>
<TD>AVAILABLE
</TD>
</TR>
<TR>
<TD>last_update
</TD>
<TD>Most recent line with “:” in it
</TD>
</TR>
<TR>
<TD>heartbeat
</TD>
<TD>Increments every data gathering
</TD>
</TR>
</TABLE>![Figure1](./images/image1.jpg?raw=true)
![Figure2](./images/image2.jpg?raw=true)
![Figure3](./images/image3.jpg?raw=true)

![Figure4](./images/image4.jpg?raw=true)
![Figure5](./images/image5.jpg?raw=true)
![Figure6](./images/image6.jpg?raw=true)

Acknowledge the installation permission challenge (you must have administrator rights to install the mtconnect agent) then you should see the installation screen:![Figure7](./images/image7.jpg?raw=true)
If you bring up Services in the Computer Management ( enter “**_ compmgmt.msc”**_in Run command line, then browse tree for services – see below.) Again, you must have administrator rights to view this screen, and you will be challenged to authenticate that you have access rights.![Figure8](./images/image8.jpg?raw=true)
The agent will NOT start unless you tell it to START (unless you reboot).
#Uninstall
First make sure the Nikon Agent is stopped in the Service Control Manager:
Right click on My Computer -> Select Manage -> Acknowledge UAC challenge
Select Services and Applications and then Services, scroll down to ZeissAgent, and push Stop button.
After you have stopped the Nikon Agent service, go into the control panel and uninstall the program: MTConnectAgentNikonx64
![Figure9](./images/image9.jpg?raw=true)
![Figure10](./images/image10.jpg?raw=true)

Please wait .. acknowledge UAC permission to uninstall challenge (you must be administrator or have administrator priviledges).  It will uninstall and you may see a black console screen popup in the background momemtarily.
And then in the Service Manager click Action->Refresh, and the Nikon Agent service should be removed. The agent code in C:\Program Files\MTConnect\MTConnectAgentNikonx64 should also be removed.
#Configuration
The installation wizard installs the log file Agent into the folder: C:\Program Files\MTConnect\ZeissMTConnectAgentX64 where X64 means a 64 bit installation platform, e.g., windows 7. The MTConnect Agent executable must be paired with the correct platform (32 or 64 bit).![Figure11](./images/image11.jpg?raw=true)
In the folder, the vb script MTConnectPage-1.4.vbs in the can be used to verify that the log file Agent is working. MTConnectPage-1.4.vbs reads the data from the agent via http://127.0.0.1:5000 and then formats the data. (assuming you have configure the agent port to 5000).
#Configuration
##Configuring MTConnect Devices
Configuration of the Devices.xml file is done with the Config.ini file.  The LogFile Agent looks in the [GLOBALS] section at the “MTConnectDevice” tag to see how many devices with accompanying sections are defined.  (The device name must be unique, but no check is done.)  For example,
	[GLOBALS]
	MTConnectDevice=M1
specifies a device M1, which has a unique section in the Config.ini file that defines all the necessary configuration parameters required. For actual deployment in a shop environment, only the ini file tag “ProductionLog” is important and must be instantiated, because the embedded MTConnect “Adapter” must know where the log file is located. 
	[M1]
	ProductionLog=C:\Users\michalos\Documents\GitHub\Agents\AgfmMTConnectAgent\AgfmAgent\x64\Debug\ProductionLog.csv
Typically, a UNC file name is used to describe the file for the production log. In the example above used for testing, a local file is shown.
In the UNC Name Syntax UNC names identify network resources using a specific notation. UNC names consist of three parts - a server name, a share name, and an optional file path. These three elements are combined using backslashes as follows:
	\\server\share\file_path
The server portion of a UNC name references the strings maintained by a network naming service such as DNS or WINS. Server names are “computer names”. The share portion of a UNC name references a label created by an administrator or, in some cases, within the operating system. In most versions of Microsoft Windows, for example, the built-in share name admin$ refers to the root directory of the operating system installation (usually C:\WINNT or C:\WINDOWS). The file path portion of a UNC name references the local subdirectories beneath the share point. For example:
	\\grandfloria\temp (to reach C:\temp on the computer grandfloria)
In this example, the file temp must be shared to allow remote access for MTConnect to read the file. Also, file permissions must allow read access to this file. This may require administrator privileges in order to perform the file read. The embedded MTConnect adapter only reads the file, it does not write, delete or modify in any manner the contents of the log file. It is on the onus of the logging service of the CNC to modify the log file. Excessively large log files, will result in delays reading thru the log file to reach the latest reading (typically because we cannot be sure what, when is recorded in the log file.) 
A complete config.ini file is shown below:
	[GLOBALS]
	Config=UPDATED
	Debug=0
	MTConnectDevice=M1,M2
	ServiceName=ZeissAgent
	logging_level=FATAL
	QueryServer=10000
	ServerRate=5000
	AgentPort=5010
	
	[M1]
	ProductionLog=C:\Users\michalos\Documents\GitHub\Agents\ZeissMTConnectAgent\ZeissAgent\x64\Debug\RunTracker.txt
	
	[M2]
	ProductionLog=\\bpq465\log\events.log
	
There are three sections in the config.ini file: Globals, M1, and M2.  In the Globals section, the Config tag can be New or Updated. If New, then a devices.xml file is generated based on the names of the MTConnectDevice tag. The MTConnectDevice tag specifies the devices for which to monitor the log file.  In this case, device “M1” and “M2” are  specified, each with its own ini file section. The tag ServiceName specifies that the service that runs will be called ZeissAgent, and monitor the http port specified by AgentPort (i.e., 5010). The ServerRate tag specifies the wait in milliseconds between log file reads. The QueryServer flag specifies the wait in milliseconds in which to try to reconnect to the log file, if that read fails.
Sections [M1] and [M2] each contains a ProductionLog tag that give the accompanying log file name for that device.
#Add devices after installation
Modify Config.ini in C:\Program Files\MTConnect\ZeissAgentx64directory
 1. Stop Zeiss agent, edit config.ini file, add new configuration:
 
	[GLOBALS]
	Config=NEW

 2. Add new devices under [GLOBALS] section tag “MTConnectDevice” (spaces are stripped out)
 
	MTConnectDevice=M1, M2, M3

 3. Make sure there is an ini file “section” for each device (in this case M1, M2, M3) and ProductionLog tag that points to the UNC (Windows Universal Naming Convention) path to the log file as in:

	[M1]
	ProductionLog=\\grandflorio\c$\logfolder\Events_log_BP_NIKON.txt
	[M2]
	ProductionLog=\\rufous\c$\logfolder\Events_log_BP_NIKON.txt
	[M3]
	ProductionLog=\\synchro\c$\logfolder\Events_log_BP_NIKON.txt
 4. Start Ziess agent, the agent will detect a new configuration, and then write a new Devices.xml file to add the new devices.
 5. If it works config.ini tag should say :”Config=UPDATED” if a problem tag will say: “Config=ERROR”

##Add network share
The MTConnect agent runs as a service, and thus has the service rights. Service rights are generally local, and often cannot be used to access network files across the enterprise domain. The following describes a way to allow a domain user to access the files and then allow the MTConnect agent service to access the file as a networked share.
 1. Add  the lines in the config.ini file section under each devices tag, in this case [M1]
	[M1]
	User=AGFM6449\oper
	Pw= Qwerty123\#
	LocalShare=Z:
	NetworkShare=\\Agfm6449\gfm\CNC\UserData_PRT\Logging
Some caveats are in orders. It is apparent that plain password are not good. Further, you cannot use the same letter for the local network share.
#Version Information from MTConnect Agent
The version information of the various MTConnect components is  available through web browser access. It is recommended to use Google Chrome as it understand the XSLT formatting of the XML (Internet Explorer is baffled.) 
The open source core MTConnect agent is downloaded from github and “frozen”. The version used is:
	MTConnect Agent Version 1.2.0.0 - built on Sat Oct 12 13:30:24 2013
It is not perfect but the output shown below gives an indication of the software involved in the MTConnect Agent operation. It can be modified to include other version information, but requires a recompilation at this time.![Figure12](./images/image12.jpg?raw=true)
![Figure13](./images/image13.jpg?raw=true)