

#MTConnect Agent Background 
----

There is a C++ open source agent from the MTConnect Institute github website (https://github.com/mtconnect/cppagent)  that:
 1. Reads shdr from 1 or more ip addresses 
 2. Performs agent duties – generally port 5000 configured in agent.cfg
 3. Peforms handshakes with SHDR agents 
 1. execution, mode, Fovr & Fact are newer naming versions 2.0 and NOT compatiable with boeing dashboard which is version 0.9, below mapping NEW into OLD
 2. execution.READY=IDLE;
 3. execution.ACTIVE=EXECUTING
 4. execution.INTERRUPTED=PAUSED
 5. execution.STOPPED=PAUSED
 6. mode=controllermode
 7. Fovr=path_feedrateovr
 8. Fact=path_feedratefrt 
 4. NOTE SHDR FROM SOLUTION DDE is already compatible with version 0.9 (uses controllermode, etc.) but if you use MTConnect SHDR from Mazak or other CNC source BEWARE!
The open source agent has a REAMDE.pdf on the github site to assist in development and configuration.
##SHDR Background
Shdr Adapters will output text like this thru a telnet connection:
2014-04-08T20:40:42.0280Z|heartbeat|714
2014-04-08T20:40:44.0426Z|heartbeat|715
2014-04-08T20:40:46.0544Z|heartbeat|716

The SHDR protocol is not part of the MTConnect® specification, and readers are advised to read "MTConnect® Standard SHDR Protocol Companion Specification" for more details.  Briefly, the SHDR protocol "was designed to be as simple as possible". SHDR provides a TCP socket stream such that one or more Agents can connect to the SHDR socket and listen to the data stream, with  behavior similarl to telnet. The communication format is a pipe '|' delimited data stream that begins with a timestamp and then follows wit a timestamp in ISO 8601 date time format with optional decimal places. The time is required to be in UTC and therefore must have a trailing Z to indicate that no time zone (+0) is being used. SHDR handles Samples, Events, Conditions, Assets, and Alarms (deprecated).  Below is an EBNF representation of the SHDR including the new multiline asset implementation:
<SHDR> ::= UTCDATE "|" <StatementList>
<StatementList> ::= <Statement> | <Statement> EOL <StatementList>
<Statement> ::= <SimpleStatement>  | < MultilineStatement >
<SimpleStatement> ::= <Tag> "|" <Value> { "|" <Value>}* 
<MultilineStatement> ::= "@" <Tag> "@" "|" ID  "|"  --multiline—{A-Z}+  .*  --multiline—{A-Z}+<Tag> ::= <Sample> | <Event> | <Condition>
<Sample> ::= ID  
<Event> ::= ID  
<Condition> ::= ID "|" <Level> "|" <NativeCode> "|"  <NativeSeverity> "|" <Qualifier> "|" <Description>
<Level> ::= UNVAILABLE | NORMAL | WARNING | FAULT
<NativeCode> ::= NUMBER | TEXT  
<NativeSeverity> ::= TEXT  
<Qualifier> :== HIGH, LOW, …
<Description> ::= TEXT
Here is a sample SHDR string for transmitting s data, and the line must be terminated with a line feed (\n) which is ASCII character 10 or 0xA.
2010-09-29T23:59:33.460470Z|Xact|1.4198908806\n


#Install
Double click the msi install script. YOU NEED TO BE ADMINISTRATOR OR HAVE ADMINISTRATOR PRIVILEDGES TO INSTALL.
![Figure1](./images/image1.jpg?raw=true)

Click next: (If location of MTConnect Agent ok and install for everyone please):![Figure2](./images/image2.jpg?raw=true)

Now you select the MTConnect service name and the port number (http://127.0.0.1:**5** /current ) in this example:![Figure3](./images/image3.jpg?raw=true)

Now you need to assign Device names, ips where the SHDR output from the CNC is located on the LAN, and the socket number (typically 7878) from which to read the SHDR text output.
![Figure4](./images/image4.jpg?raw=true)

Click next and it the MTConnect Agent SHDR service will be installed:![Figure5](./images/image5.jpg?raw=true)
Please be patient it can be a while, to understand your authentication.
You may get a authentication popup box to allow you to install. Please enter your name and password, (with administrator or install prividges).
You might see some black DOS boxes appear in the background, this is part of the installation process and these scripts install the agent and its service.

The MTConnect Agent from SHDR service should start, but is susceptible to Windows 7 issues so it may not. Check that the service was installed:
Right click My Computer, click Manages, navigate to Services and see if the MTConnectAgentFromShdr has started. (You may need to start, make sure automatic – so when the computer reboots it will start automatically in background collected CNC Shdr output).![Figure6](./images/image6.jpg?raw=true)
When you click on the MTConnectAgentFromShdr service name you should see:![Figure7](./images/image7.jpg?raw=true)

You can verify that the install script worked by moving to folder C:\Program Files\MTConnect\MTConnectAgentFromShdr and seeing:
![Figure8](./images/image8.jpg?raw=true)
If you do see the MTConnectAgentFromShdr in the service manager (accessed by My Computer manage) then you may need to manually install the agent, with install.bat
Of note, Windows 7 is of "more" cybersecurity and doesn't let you write to C:\Program Files\MTConnect\MTConnectAgentFromShdr folder except on install, UNLESS YOU CHANGE FILE PERMISSIOINS. The custom dll does this and should make all the files in the folder writeable. If not, the agent will fail. If you have this problem, run superuser.bat as administrator (right click Run As Administrator) and it should reset the file permisssions and allow the agent to write to agent.log and debug.txt.
As a safeguard and double check, you can check the agent.cfg and the devices.xml to make sure the files match what was input during installation for the agent service name, agent http port, the cncs ip, port and device names. 

#UNINSTALL
Double click the msi script or go into the control panel under software and remove  the program MTConnectAgentFromShdr