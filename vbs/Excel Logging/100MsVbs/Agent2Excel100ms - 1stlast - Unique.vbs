'
' This software was developed by U.S. Government employees as part of
' their official duties and is not subject to copyright. No warranty implied 
' or intended.
'
' Questions: john.michaloski@Nist.gov


'
' This software will read all the devices samples/event in an MTConnect agent
' and display each device as a separate worksheet in Excel.
'

Option Explicit

Const xlCenter = &HFFFFEFF4


Dim	sleepamt  
Dim	ws,wb, objXL, objOL, olNS,	olTask,	olTaskList,	olTasksFolder
Dim	readings, readingKeys 
dim	item,strComputer
Dim 	xmlDoc1
dim 	samples, events
dim 	sample,  e, child
Dim 	rootNode , nodes
dim 	devices,devicelist()
dim 	i, d,intSheetCount
dim     lastReading

sleepamt = 100	' in milliseconds
'strComputer	= "agent.mtconnect.org"	
strComputer	=  Inputbox ("Machine that you would	like monitor:",	"Machine", "agent.mtconnect.org")

if strComputer = ""	 Then
	Wscript.Quit(0)
End	if

Function ConvertToLetter(iCol) 
   Dim iAlpha 
   Dim iRemainder
   iAlpha = Int(iCol / 27)
   iRemainder = iCol - (iAlpha * 26)
   If iAlpha > 0 Then
      ConvertToLetter = Chr(iAlpha + 64)
   End If
   If iRemainder > 0 Then
      ConvertToLetter = ConvertToLetter & Chr(iRemainder + 64)
   End If
End Function




''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
' http://support.microsoft.com/kb/246067
'
Const dictKey  = 1
Const dictItem = 2

Function SortDictionary(objDict,intSort)
  '	declare	our	variables
  Dim strDict()
  Dim objKey
  Dim strKey,strItem
  Dim X,Y,Z

  '	get	the	dictionary count
  Z	= objDict.Count

  '	we need	more than one item to warrant sorting
  If Z > 1 Then
	' create an	array to store dictionary information
	ReDim strDict(Z,2)
	X =	0
	' populate the string array
	For	Each objKey	In objDict
		strDict(X,dictKey)	= CStr(objKey)
		strDict(X,dictItem)	= CStr(objDict(objKey))
		X =	X +	1
	Next

	' perform a	a shell	sort of	the	string array
	For	X =	0 to (Z	- 2)
	  For Y	= X	to (Z -	1)
		If StrComp(strDict(X,intSort),strDict(Y,intSort),vbTextCompare)	> 0	Then
			strKey	= strDict(X,dictKey)
			strItem	= strDict(X,dictItem)
			strDict(X,dictKey)	= strDict(Y,dictKey)
			strDict(X,dictItem)	= strDict(Y,dictItem)
			strDict(Y,dictKey)	= strKey
			strDict(Y,dictItem)	= strItem
		End	If
	  Next
	Next

	' erase	the	contents of	the	dictionary object
	objDict.RemoveAll

	' repopulate the dictionary	with the sorted	information
	For	X =	0 to (Z	- 1)
	  objDict.Add strDict(X,dictKey), strDict(X,dictItem)
	Next

  End If


End	Function


Function IsUniqueDictionary(objDict)
dim str, laststr, myreadingKeys	
'On Error Resume	Next

	myreadingKeys	= readings.Keys
	
	'msgbox myreadingKeys.Count
	IsUniqueDictionary=True	

	for	i =	0 to objDict.Count	-1
		msgbox myreadingKeys(i)
		str = readings.Item( myreadingKeys(i))
		laststr = lastReading.Item( myreadingKeys(i))
		msgbox str &"+" & laststr
		if(str <> laststr) then
			IsUniqueDictionary=False
			Exit For
		end if
			
	next 
	lastReading=readings
End Function

function alldevices(machine)
dim j
	xmlDoc1.Load("http://" & machine & "/current")
	if(	 0 <> Err.Number ) then	
		 MsgBox	"Either	MTConnect not running or Internet Explorer Closed"
		Wscript.Quit(0)
	End	if 


	Set	rootNode = xmlDoc1.documentElement
	set	nodes= rootNode.selectNodes("//DeviceStream")
	redim devicelist(nodes.length-1)
	j=0
	For	Each item In nodes
		devicelist(j)= item.attributes.getNamedItem("name").nodeValue
		j=j+1
	next

end function
'
'
'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
function updatetable(machine, device)
Dim	j
	 On	Error Resume Next
	updatetable=""
	xmlDoc1.Load("http://" & machine & "/current")
	if(	 0 <> Err.Number ) then	
		 MsgBox	"Either	MTConnect not running or Internet Explorer Closed"
		Wscript.Quit(0)
	End	if 


	Set	rootNode = xmlDoc1.documentElement
	set	nodes =	rootNode.selectNodes("//DeviceStream[@name='" & device & "']")
	For	Each item In nodes


	  ''''''''''''''''''''''''''''''''''''''''
	  Set samples =	item.SelectNodes(".//Samples")
	 For Each sample In	samples

		For	Each child In sample.ChildNodes

			If Not child.Attributes.getNamedItem("name") Is	Nothing	Then
				readings.Add child.Attributes.getNamedItem("name").NodeValue, child.Text
			Else
				If Not child.Attributes.getNamedItem("dataItemId") Is Nothing Then
				readings.Add child.Attributes.getNamedItem("dataItemId").NodeValue,	child.Text
				end	if
			End	If

		Next
	  Next


	  Set events = item.SelectNodes(".//Events")
	  For Each e In	events

		For	Each child In e.ChildNodes

			If Not child.Attributes.getNamedItem("name") Is	Nothing	Then
			readings.Add child.Attributes.getNamedItem("name").NodeValue, child.Text
			Else
				If Not child.Attributes.getNamedItem("dataItemId") Is Nothing Then
				readings.Add child.Attributes.getNamedItem("dataItemId").NodeValue,	 child.Text
		end	if
			End	If
		Next
	  Next

	Next
end	function




function writeheaders()
Dim	j
	SortDictionary readings,1 
	readingKeys	= readings.Keys
	j=2
	objXL.Cells(1,1).Value = "Timestamp"
	'objXL.Cells(1,1).NumberFormat = "dd/mm/yy hh:mm:ss"
	objXL.Cells(1,1).Font.Bold = True
	for	i =	0 to readings.Count -1	
		objXL.Cells(1,j).Value = readingKeys(i)
		objXL.Cells(1,j).Font.Bold = True
		'objXL.Cells(1,j).Interior.ColorIndex = 5
		'objXL.Cells(1,j).Interior.Pattern = 1
		'objXL.Cells(1,j).Font.ColorIndex = 2
		objXL.Cells(1,j).HorizontalAlignment = xlCenter

		j=j+1 
	next 
end	function

dim t,temp,Milliseconds , str
function gettimestamp()
	t = Timer
	' Int() behaves exacly like Floor() function, ie. it returns the biggest integer lower than function's argument
	temp = Int(t)
	str=FormatDateTime(Now(),2) & " " & FormatDateTime(Now(),4)
	str=  str & ":" & right("0" & second(Now()),2)
	Milliseconds = Int((t-temp) * 1000)
	gettimestamp = str & ":" & CStr(Milliseconds )
end function

Dim n 

function writeReading()
Dim	j
dim str
	SortDictionary readings,1 
	readingKeys	= readings.Keys

	if(IsUniqueDictionary(readings)) Then 
		Exit Function
	end if

	j=2  ' column index

	n=n+1 ' row index

	objXL.Cells(n,1).Value = gettimestamp() ' FormatDateTime(Now)
	for	i =	0 to readings.Count	-1
		str = readings.Item( readingKeys(i))
		if str = "UNAVAILABLE" then str = "--" End IF
		objXL.Cells(n,j).Value = str
		objXL.Cells(n,j).Font.Bold = false
		j=j+1
	next 

end	function


Set	readings = CreateObject("Scripting.Dictionary")
Set lastReading =  CreateObject("Scripting.Dictionary")
 


Set xmlDoc1 = CreateObject("Msxml2.DOMDocument")
xmlDoc1.async = False
xmlDoc1.setProperty "ServerHTTPRequest", true

Set	objXL =	Wscript.CreateObject("Excel.Application")
if objXL  is nothing then
	 MsgBox	"You must have Excel installed to run this script"
	Wscript.Quit(0)
end if

objXL.Visible =	TRUE

alldevices(strComputer)

set wb = objXL.WorkBooks.Add

For Each item In devicelist
	intSheetCount = objXL.Application.Worksheets.Count ' count the worksheets 	
	wb.Sheets.Add(,objXL.Worksheets(intSheetCount)).Name=item
	updatetable strComputer,item
	writeheaders()
next

if UBOUND(devicelist) > 0 then
wb.Sheets("Sheet1").Delete
wb.Sheets("Sheet2").Delete
wb.Sheets("Sheet3").Delete
end if

n=1

Do While True
On Error Resume	Next
ws=wb.ActiveSheet
objXL.Application.ScreenUpdating = False
For Each d In devicelist
	readings.RemoveAll
	updatetable strComputer,d
	wb.Sheets(d).Select
	writeReading
	If objWMIServiceobjXL Is Nothing Then
	'objXL.Columns("A:" & ConvertToLetter( readings.Count-1) ).AutoFit
	'If Err.Number <> 0 Then	
		Wscript.Quit(0)
	End if	

next
wb.ActiveSheet=ws
objXL.Application.ScreenUpdating = True

Wscript.Sleep(sleepamt)

Loop