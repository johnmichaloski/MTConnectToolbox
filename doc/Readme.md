

#Troubleshooting Guide for MTConnect  
----

Michaloski, John L. (Fed)
1/17/2017 1:40:00 PM
MTConnectTroubleshootingGuide.docx
#Abstract
This document describes some common problems for deploying MTConnect. This is a troubleshooting guide for dealing with problems (such as firewall) and using MTConnect as 24/7 service. There is no guarantee for success. Patience and google search are your best friends.
##Check machine architecture.  Does the PC architecture match the agent software implementation?
 1. Is this a Win 7 64 bit machine?  Agent exe can be picky about 32 or 64 bit exe.
 2. 32 bit MTConnect 1.2/1.3 agents will not run on 64 bit platforms.
 Often they appear to be working, but are not. 
 3. Windows XP SP2 and later has its own issues.
##Test running Agent on command line

	cd "install directory"
	agent.exe debug 
This should at least not return immediately. The command should remain running until you either close the console window or hit control-C.  If it doesn't work, there is a possibility you do not have the proper C++ runtime installed: See https://support.microsoft.com/en-us/help/2977003/the-latest-supported-visual-c-downloads. However, the C++ runtime should be shipped with the agent.
Second, an underlying DLL that is required may not be found, e.g., focas library DLL, fwlib32.dll or fwlib64.dll. Usually you get a pop up dialog informing you of a missing DLL.
Check "Is Agent Running" to see if command line operation is working. Before you move on to using an MTConnect agent as a service, it MUST work as a command line operation. 
##Is Agent running?
Test local/remote agent access. 
 1. On local machine access the agent via a web page and the localhost IP :

	127.0.0.1:5000/current (or whatever the service port is)
 You should see a page full of XML.
 2. On remote machine, again access the agent via a web page and the MTConnect agent PC ip

	xxx.xxx.xxx.xxx:5000/current (or whatever the service port is)
 
 You should see a page full of XML. Potential problems:
 1. Local firewall blocking remote IP socket port access. See firewall issues for lcoal machine.
 2. Remote firewall block IP socket port access from your host.  See firewall issues for remote machine.
 
##You should see a page full of XML - but all unavailable data items from the Agent web page fetch.
If Agent is not reading SHDR (assuming SHDR interface), then you will  not get any data. Potential problems:
 1. firewall blocking remote access to SHDR IP and port.  You can try telnet to see if the SHDR service is even running 
 1. windows: bring up cmd window, and then start telnet

	> telnet 
	# COMMENT: open ip port
	> open xxx.xxx.xxx.xxx 5000

If windows you must have telnet isntalled as windows feature. https://technet.microsoft.com/en-us/library/cc771275(v=ws.10).aspx 

##Is the agent registered as a service?
 1. Open the service control manager: See https://msdn.microsoft.com/en-us/library/htkdfk18(v=vs.110).aspx 
 2. Is the agent (whatever you named it or the default) - actually there?
 3. If no, try manually installing using agent.exe

	> cd "agent install folder" 
	> ./agent.exe install
 4. Go to a, if success done, if fail
 
##Running service from Service Control Manager.
 1. Open the service control manager: See https://msdn.microsoft.com/en-us/library/htkdfk18(v=vs.110).aspx 
 2. Click Start: Start the agent (whatever you named it or the default) - actually start and stay alive?
 3. then see web browser 127.0.0.1:port#/current    See anything? Repeat instruction from previous step in verifying agent is working.
 4. If you want 24/7 operation, make sure the service is set to AUTOMATIC so it starts up when the PC starts. Otherwise, it will only start when you repeat the instructions from this step.
 
If this fails, check agent.log file for hints to why failure.
 
##Test Agent memory leaking. 
Task Manager Memory Assessment: 
 1. Make sure agent running. 
 2. Start Task Manager (http://www.wikihow.com/Open-Windows-Task-Manager) .
 3. Then click Processes Tab, and look for the MTConnect agent exe name in the process list 
 4. note the Memory Field size of the agent.

After 24 hours repeat above Task Manager Memory Assessment. If Memory Field size of the agent has grown dramatically you may need to reset the agent nightly as there may be memory leak.
##Only 1 agent executable running
Make sure the MTConnect agent service restarts ok, and does not start 2 instances of agent (you will need to verify this in the Task manager – look for the MTConnect agent exe name in the process list – make sure you click the "Show Process From All Users" button in the task manager, as the service is a SYSTEM account, not the local user account.
##Fanuc Focas Not Working
 1. Make sure you have correct focas DLL in your installation folder
 2. if Ethernet Focas, make sure the local and remote windows firewall port 8193 is not blocked.
##Firewall Headaches
Note, we have so far assumed that you are only dealing with a Windows Firewall. This may not be the case at all. You may have another OEM firewall running instead of the windows firewall. Can't offer much help here – now at least Windows tells you another firewall is in charge.

## Mysterious failing no writing to agent.log
As of windows 7, files in Program Files folder are readonly for everyone (including SYSTEM the service user). You will need to run a script as Administrator (right click a batch or vbs file and Select Run As Administrator) with the following code (that works in Win 7):

	cd /d %~dp0
	echo y|  cacls .  /t  /c  /GRANT Everyone:F
	pause
The cd command changes to the current working directory, second line does magic and the third line prevents an error from instantly removing the console before you can read the error. You will need to hit any character witin the console window for pause to finish, and the console to close.

##More debugging - How do I configure the Agent logger?
The agent.cfg is responsible for the Agent configuration. Inside the agent.cfg file, logging configuration is specified using the logger_config block. 
<TABLE>
<TR>
<TD>logger_config configuration items<BR></TD>
</TR>
<TR>
<TD>logger_config <BR></TD>
<TD>The logging configuration section.<BR></TD>
</TR>
<TR>
<TD>logging_level <BR></TD>
<TD>The logging level: trace, debug, info, warn, error, or fatal. <BR>Default:info<BR></TD>
</TR>
<TR>
<TD>output <BR></TD>
<TD>The output file or stream. If a file is specified specify as:<BR>"file <filename>". cout and cerr can be used to specify the standard output and standard error streams. Defaults to the same directory as the executable.<BR>Default: file adapter.log<BR></TD>
</TR>
</TABLE>

You can change the logging_level to specify the verbosity of the logging as well as the destination of the logging output.
logger_config
{
logging_level = debug
output = file debug.log
}

This will log everything from debug to fatal to the file debug.log. For only fatal errors you can specify the following:
logger_config
{
logging_level = fatal
}

The default file is agent.log in the same directory as the agent.exe file resides. The default logging level is info. To have the agent log to the command window:
logger_config
{
logging_level = debug
output = cout
}

This will log debug level messages to the current console window. When the agent is run with debug, it is sets the logging configuration to debug and outputs to the standard output as specified above. 

##Changing Windows Firewall to allow MTConnect Agent (or Adapter) through
This is how the firewall is presented after you request "Advanced Features" in Windows Filewall under the Control Panel:
<CENTER>
![Figure1](./images/image1.gif?raw=true)
</CENTER>

Request a "New Rule" from the "Inbound Rules" panel on the right side of the window.

<CENTER>
![Figure2](./images/image2.gif?raw=true)
</CENTER>

Designate the xxx port number that matches ipaddress:xxxx/current (in the case shown below 5010)

<CENTER>
![Figure3](./images/image3.gif?raw=true)
</CENTER>


Allow !
<CENTER>
![Figure4](./images/image4.gif?raw=true)
</CENTER>

All is best..
<CENTER>
![Figure5](./images/image5.gif?raw=true)
</CENTER>

Give name and optional description:
<CENTER>
![Figure6](./images/image6.gif?raw=true)
</CENTER>



Double check opening inbound port came through:
<CENTER>
![Figure7](./images/image7.gif?raw=true)
</CENTER>


![Word2Markdown](./images/word2markdown.jpg?raw=true)  Autogenerated from Microsoft Word by [Word2Markdown](https://github.com/johnmichaloski/SoftwareGadgets/tree/master/Word2Markdown)