
ReadTelnet.vbs
==============

VBS to capture SHDR from Telnet
--------------------------------
Tuesday, May 12, 2015 10:00:05 AM EDT

This document describe the vb script shdrtelnet.vbs. The purpose of the vb script is to automate the capture of MTConnect Adapter SHDR text from a telnet port. Typically, MTConnect adapters use their IP and use TCP/IP to communicate over port 7878 a text format called SHDR.



You will want to input the IP and Port of the MTConnect device (typically a CNC machine). The default IP is set to the local host and port 7878.  You need to enter: xxx.xxx.xxx.xxx:yyyy  where xx’s constitute the IP address, and yy constitute the port number. The vbscript will separate the entry into the IP and Port fields for the telnet command. Note, if you hit the Cancel button, the script will be terminated.

![Figure 1](./images/image1.png?raw=true) 

 Next, you need to set the output filename for capturing the SHDR log. The file is appended to the current folder of the vbscript so the file name out.txt will actually log to

	 C:\Users\michalos\Documents\GitHub\Tech Providers\NIST\vbs\telnet\out.txt


where the vbscript was executed in the folder C:\Users\michalos\Documents\GitHub\Tech Providers\NIST\vbs\telnet. Again, if you hit the Cancel button, the script will be terminated.

![Figure 2](./images/image2.png?raw=true) 

Next a telnet session will be started (as specified by the previous two dialog boxes). The telnet session will be captured to the log file while at the same time creates a DOS cmd window, as shown below:

![Figure 3](./images/image3.png?raw=true) 

You should see SHDR output in the telnet session. When you have enough data, you can terminate the logging by hitting the upper right hand corner [X] to close the window. This will terminate the logging. 

You should see a “out.txt” file in the same folder that you executed the vbscript.
 
![Figure 4](./images/image4.png?raw=true) 

