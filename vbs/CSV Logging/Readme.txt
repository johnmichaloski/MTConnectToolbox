MTConnectAgentCsvLogging.vbs
===============================

This is a Microsoft Windows Visual Basic Script (vbs) that asks the user for 
a MTConnect Agent URL (e.g., agent.mtconnect.org) and then queries the MTConnect
Agent for the latest data readings, and appends the data into an comma separated value
(csv) file called mtconnectlog with date and time appended and the extension is csv. 

For example, the file mtconnectlog_5_28_2016_15_6_17.csv is created in the same file system location
or  folder as the vbs script is running. 

In addition, a Internet Explorer window pops up with the
message: Close this Web Browser to Stop CSV MTConnect Agent Logging!!!
Closing this Internet Explorer window  stops the MTConnect Agent logging. The pop up dialog box should
appear with the message: CSV Scripting says Bye and an OK Button.

The logging reads the data item type -  samples, events, and condition -  tag names and data values. 
The vbs creates a sorted dictionary and creates a comma separated file to append the polled values. 
If its the first time, a header is appended to the start of the file with all the tag names separated
by commas.


