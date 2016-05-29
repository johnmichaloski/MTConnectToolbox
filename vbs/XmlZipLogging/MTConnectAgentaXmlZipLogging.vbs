'
' This software was developed by U.S. Government employees as part of
' their official duties and is not subject to copyright. No warranty implied 
' or intended.
'
' Questions: john.michaloski@Nist.gov

' GetElementById
' http://www.vistax64.com/vb-script/174625-editing-extended-active-directory-users-informations-vbscrip.html
' innerHTML is readonly for as pointed out here: http://msdn.microsoft.com/en-us/library/ms533897%28VS.85%29.aspx


' VBSCRIPT HELP: http://www.w3schools.com/vbScript/vbscript_ref_functions.asp
'                http://msdn.microsoft.com/en-us/library/sx7b3k7y(VS.85).aspx 
' XML HELP:      http://www.devguru.com/Technologies/xmldom/quickref/xmldom_properties.html 
'------------------------------------------------------------------------
'Define Constants & Variables
'------------------------------------------------------------------------
'BEGIN CALLOUT A
Option Explicit

Const fsoForReading = 1
Const fsoForWriting = 2
Const fsoForAppending = 8
Const ForAppending = 8
Const TristateFalse = 0

Dim strStatus, strPriority, strDueDate
Dim intRowCount : intRowCount = 2
'END CALLOUT A

Dim sleepamt
Dim item
Dim headerstr
Dim ie
Dim xmlDoc1 
Dim rootNode , nodes
dim i,j,k
dim html
dim table

dim samples, events
dim sample,  e, child
dim strComputer,NumAcross


'sleepamt = 3000 
sleepamt = 100 
NumAcross=5



strComputer = Inputbox ("Machine that you would like csv logging:", "Machine", "agent.mtconnect.org")
if strComputer = ""  Then
	Wscript.Quit(0)
End if


'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
Dim objIE 
function setupIE()
	Set objIE = WScript.CreateObject("InternetExplorer.Application", "IE_")
	objIE.visible = 1         ' keep visible
	objIE.navigate "about:blank"
	objIE.Document.Open
	objIE.Document.write "<HTML><BODY><H1>Close this Web Browser to Stop MTConnect Agent XML Zip Logging!!!</H1></BODY></HTML>" &  vbCrLf 
	objIE.Document.close
end function

Dim fso
Set fso = CreateObject("Scripting.FileSystemObject")
Sub NewZip(zipPath)
	Dim zipfile
	'Create empty Zip File
	Set zipfile = fso.CreateTextFile(zipPath, True)
	zipfile.write("PK" & chr(5) & chr(6) & string(18,chr(0)))
	zipfile.close
End Sub


'''''''''''''''''''''''''''''''''''''''''''''''''''''
'' https://msdn.microsoft.com/en-us/library/ms757828%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396

function setupMSXML()
	Set xmlDoc1 = CreateObject("Msxml2.DOMDocument")
	xmlDoc1.async = False
	xmlDoc1.setProperty "ServerHTTPRequest", true
end function


''''''''''''''''''
' Setup log file 
Dim  slogdir, slogfile,curLogDate 

' Log file name in script directory
slogdir = CreateObject("Scripting.FileSystemObject").GetParentFolderName(WScript.ScriptFullName)  & "\"

function setupLogfile()
	dim t,temp,Milliseconds 
	t = Timer
	' Int() behaves exacly like Floor() function, ie. it returns the biggest integer lower than function's argument
	temp = Int(t)
	curLogDate =  Month(Date) & "_" & Day(Date) & "_" & Year(Date)& "_"  & Hour(Time) & "_" & Minute(Time) & "_" & Second(Time)
	Milliseconds = Int((t-temp) * 1000)
	curLogDate= curLogDate & "_" &  right("0" & CStr(Milliseconds ),3) 
	
	setupLogfile= slogdir & "mtconnectlog_" & curLogDate & ".xml"
end function


''''''''''''''''''
' Flags for writing header into csv file and looping
Dim bFirst, bLoopFlag
bFirst = True
bLoopFlag=True

''''''''''''''''''
' Main loop
Dim logFileName
setupIE()
setupMSXML()

' Zip file extras
dim oApp,  WshShell, zipPath, DestFldr, zipcnt
zipPath = slogdir +"MTConnect.zip"
Set WshShell = WScript.CreateObject("WScript.Shell")
Set oApp = CreateObject("Shell.Application")
NewZip(zipPath)

On Error Resume Next


Do While bLoopFlag
	logFileName=setupLogfile()
	xmlDoc1.Load("http://" & strComputer & "/current") 

	'Set rootNode = xmlDoc1.documentElement
	'set nodes = rootNode.selectNodes("//DeviceStream")
	xmlDoc1.save (logFileName)
 	'oApp.Namespace(zipPath).CopyHere logFileName,Hex(4)
	Set DestFldr=oApp.NameSpace(zipPath)
	zipcnt = oApp.Namespace(zipPath).items.count
	DestFldr.CopyHere logFileName,4 +16
	do until (zipcnt+1) = oApp.Namespace(zipPath).items.count
  	  wscript.sleep 10
	loop
	wscript.sleep 150
	fso.DeleteFile logFileName

	'Wscript.Sleep(10)
	'Wscript.Sleep(sleepamt)
Loop


MsgBox "XML Zip Logging says Bye"
Wscript.Quit(0)

Sub IE_onQuit
     bLoopFlag=False
End Sub






