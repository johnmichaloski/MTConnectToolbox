
MTConnectPage-X.Y.vbs
============================
X.Y denotes the version of the vb script.

This is a Microsoft Windows Visual Basic Script (vbs) that asks the user for 
a MTConnect Agent URL (e.g., agent.mtconnect.org) and then queries the MTConnect
Agent for the latest data readings, and creates/appends the data in a 
Windows Internet Explorer web page. Each device in the agent has a separate table of logged values,
and the web page tables are put into rows to make sure all the data per device is visible (sort of).


The logging reads the data item type -  samples, events, and condition -  tag names and data values. 
The vbs creates a sorted dictionary and updates the web page table with polled values. 

A delay of roughly 3 seconds between polls is done. If a MTConnect device is down, 
all the agent data for the device will read UNAVAILABLE. This corresponds to a table with tags names 
on the left, and UNAVAILABLE in the value fields.


