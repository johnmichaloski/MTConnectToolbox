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

Dim readings, readingKeys 
Set readings = CreateObject("Scripting.Dictionary")

function gettimestamp()
	dim t,temp,Milliseconds , str
	t = Timer
	' Int() behaves exacly like Floor() function, ie. it returns the biggest integer lower than function's argument
	temp = Int(t)
	str=FormatDateTime(Now(),2) & " " & FormatDateTime(Now(),4)
	str=  str & ":" & right("0" & second(Now()),2)
	Milliseconds = Int((t-temp) * 1000)
	gettimestamp = str & ":" &  right("0" & CStr(Milliseconds ),3) 
end function


''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
' http://support.microsoft.com/kb/246067
'
Const dictKey  = 1
Const dictItem = 2

Function SortDictionary(objDict,intSort)
  ' declare our variables
  Dim strDict()
  Dim objKey
  Dim strKey,strItem
  Dim X,Y,Z

  ' get the dictionary count
  Z = objDict.Count

  ' we need more than one item to warrant sorting
  If Z > 1 Then
    ' create an array to store dictionary information
    ReDim strDict(Z,2)
    X = 0
    ' populate the string array
    For Each objKey In objDict
        strDict(X,dictKey)  = CStr(objKey)
        strDict(X,dictItem) = CStr(objDict(objKey))
        X = X + 1
    Next

    ' perform a a shell sort of the string array
    For X = 0 to (Z - 2)
      For Y = X to (Z - 1)
        If StrComp(strDict(X,intSort),strDict(Y,intSort),vbTextCompare) > 0 Then
            strKey  = strDict(X,dictKey)
            strItem = strDict(X,dictItem)
            strDict(X,dictKey)  = strDict(Y,dictKey)
            strDict(X,dictItem) = strDict(Y,dictItem)
            strDict(Y,dictKey)  = strKey
            strDict(Y,dictItem) = strItem
        End If
      Next
    Next

    ' erase the contents of the dictionary object
    objDict.RemoveAll

    ' repopulate the dictionary with the sorted information
    For X = 0 to (Z - 1)
      objDict.Add strDict(X,dictKey), strDict(X,dictItem)
    Next

  End If

End Function


'
'
'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
function updatetable(machine)
     On Error Resume Next
  	updatetable=""
	headerstr=""
	xmlDoc1.Load("http://" & machine & "/current")
	if(  0 <> Err.Number ) then 
	     MsgBox "Either MTConnect not running or Internet Explorer Closed"
 	    Wscript.Quit(0)
	End if 


	Set rootNode = xmlDoc1.documentElement
	set nodes = rootNode.selectNodes("//DeviceStream")
	k=0
	' This is for each device in MTConnect Agent stream
	For Each item In nodes

	  ' Log device name
	headerstr="Timestamp,Device"
	updatetable = gettimestamp() &  "," & item.attributes.getNamedItem("name").nodeValue
	readings.RemoveAll

      ''''''''''''''''''''''''''''''''''''''''



      Set events = item.SelectNodes(".//Events")
      For Each e In events

        For Each child In e.ChildNodes

            If Not child.Attributes.getNamedItem("name") Is Nothing Then
            readings.Add child.Attributes.getNamedItem("name").NodeValue, child.Text
            Else
             	If Not child.Attributes.getNamedItem("dataItemId") Is Nothing Then
    			readings.Add child.Attributes.getNamedItem("dataItemId").NodeValue,  child.Text
		end if
            End If
        Next
      Next
	SortDictionary readings,1 
	readingKeys = readings.Keys
	for i = 0 to readings.Count -1
			headerstr =  headerstr & ", " & readingKeys(i)   
			updatetable =  updatetable & ", " & readings.Item( readingKeys(i))   
 	next 

	updatetable = updatetable &  vbCrLf 
	headerstr = headerstr &  vbCrLf 

	k=k+1
	if k>NumAcross then
		updatetable =  updatetable & "</TR><TR>"
		k=0		
	end if
	Next



end function





'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
Dim objIE 
function setupIE()
	Set objIE = WScript.CreateObject("InternetExplorer.Application", "IE_")
	objIE.visible = 1         ' keep visible
	objIE.navigate "about:blank"
	objIE.Document.Open
	objIE.Document.write "<HTML><BODY><H1>Close this Web Browser to Stop CSV MTConnect Agent Logging!!!</H1></BODY></HTML>" &  vbCrLf 
	objIE.Document.close
end function


function setupMSXML()
	Set xmlDoc1 = CreateObject("Msxml2.DOMDocument")
	xmlDoc1.async = False
	xmlDoc1.setProperty "ServerHTTPRequest", true
end function


''''''''''''''''''
' Setup csv log file 
Dim  slogdir, slogfile, curLogDate
Dim fso, f

function setupCsvLogfile()
	curLogDate =  Month(Date) & "_" & Day(Date) & "_" & Year(Date)& "_"  & Hour(Time) & "_" & Minute(Time) & "_" & Second(Time)

	' Log file name in script directory

	slogdir = CreateObject("Scripting.FileSystemObject").GetParentFolderName(WScript.ScriptFullName)  & "\"
	slogfile = slogdir & "mtconnectlog_" & curLogDate & ".csv"


	''''''''''''''''''
	' Create or truncate existing log file, then open as append

	Set fso = CreateObject("Scripting.FileSystemObject")
	If Not (fso.FileExists(slogfile)) Then
		' Create
		Set f = fso.CreateTextFile(slogfile, fsoForWriting, TristateFalse)
	    	f.Close
	else
		' Truncate
		Set f = fso.OpenTextFile(slogfile, fsoForWriting, TristateFalse)
	    	f.Close
	End If

	'Set f = fso.OpenTextFile( slogfile , fsoForAppending, true)
	Set f = fso.OpenTextFile( slogfile , 2, true)
end function


''''''''''''''''''
' Flags for writing header into csv file and looping
Dim bFirst, bLoopFlag
bFirst = True
bLoopFlag=True

''''''''''''''''''
' Main loop
setupIE()
setupMSXML()
setupCsvLogfile()
On Error Resume Next

Do While bLoopFlag
	table = ""
	table = table & updatetable(strComputer)
	if bFirst then
		'table = table & headerstr 
		f.write headerstr  
		bFirst=False
	End If
	f.write table
	Wscript.Sleep(sleepamt)
Loop

f.Close
fso.Copyfile slogfile, slogdir & "mtconnectlog.csv"

Set f=Null
Set fso=Null
MsgBox "CSV Scripting says Bye"
Wscript.Quit(0)

Sub IE_onQuit
     bLoopFlag=False
End Sub






