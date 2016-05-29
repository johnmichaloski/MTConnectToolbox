
MTConnectAgentaXmlZipLogging
============================


This is a Microsoft Windows Visual Basic Script (vbs) that asks the user for 
a MTConnect Agent URL (e.g., agent.mtconnect.org) and then queries the MTConnect
Agent for the latest data readings, and creates/appends the data into an zip folder called
MTConnect.zip. 


The logging reads the data item type -  samples, events, and condition -  tag names and data values. 
The vbs creates a sorted dictionary and creates an timestamped zipped xml file containing polled values. 

In addition, a Internet Explorer window pops up with the
message: Close this Web Browser to Stop CSV MTConnect Agent Logging!!!
Closing this Internet Explorer window  stops the MTConnect Agent logging. The pop up dialog box should
appear with the message: CSV Scripting says Bye and an OK Button.
