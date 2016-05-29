Agent2Excel.vbs
===============

This is a Microsoft Windows Visual Basic Script (vbs) that asks the user for 
a MTConnect Agent URL (e.g., agent.mtconnect.org) and then queries the MTConnect
Agent for the latest data readings, and creates/appends the data into an Microsoft Office
Excel file  (xslx extension). The excel file is opened and data is appended into the file. 
When the user is done logging data, first optionally saving the file and then closing the file will
stop the excel logging.


The logging reads the data item type -  samples, events, and condition -  tag names and data values. 
The vbs creates a sorted dictionary and creates an excel file to append the polled values. 
If its the first time, a header is appended to the start of the Excel file with all the tag names separated
by commas.


