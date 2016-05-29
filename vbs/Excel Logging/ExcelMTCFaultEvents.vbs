'------------------------------------------------------------------------
'Define Constants & Variables
'------------------------------------------------------------------------
'BEGIN CALLOUT A
Option Explicit

Const TIMESTAMP=1, OWNER=2, POWER=3,MODE=4, EXECUTION=5, PROGRAM=6
Const xlCenter = &HFFFFEFF4

Dim objXL, objOL, olNS, olTask, olTaskList, olTasksFolder
Dim strStatus, strPriority, strDueDate
Dim intRowCount : intRowCount = 2
'END CALLOUT A

Dim item
Dim array(0)

array(0) = Inputbox ("Machine that you would like monitor faults on:", "Machine", "127.0.0.1")
if array(0) = ""  Then
	Wscript.Quit(0)
End if


'------------------------------------------------------------------------
'Create the worksheet and setup titles
'------------------------------------------------------------------------
'BEGIN CALLOUT B
Set objXL = Wscript.CreateObject("Excel.Application")
objXL.Visible = TRUE
objXL.WorkBooks.Add

objXL.Range("A1:H1").Font.Bold = True
objXL.Range("A1:H1").Interior.ColorIndex = 5
objXL.Range("A1:H1").Interior.Pattern = 1
objXL.Range("A1:H1").Font.ColorIndex = 2
objXL.Range("A1:H1").HorizontalAlignment = xlCenter
objXL.Cells(1,TIMESTAMP).Value = "Time"
objXL.Cells(1,TIMESTAMP).NumberFormat = "dd/mm/yy hh:mm:ss"
objXL.Cells(1,OWNER).Value = "Machine"
objXL.Cells(1,POWER).Value = "Power"
objXL.Cells(1,MODE).Value = "Mode"
objXL.Cells(1,EXECUTION).Value = "Execution"
objXL.Cells(1,PROGRAM).Value = "Program"

objXL.Rows("2:2").Select
objXL.ActiveWindow.FreezePanes = True
'END CALLOUT B


Dim xmlDoc1 
Dim rootNode 
dim i,j
dim newalarm, lastalarm

Set xmlDoc1 = CreateObject("Msxml2.DOMDocument")
xmlDoc1.async = False
xmlDoc1.setProperty "ServerHTTPRequest", true
i=0

Do While True

For Each item In array
On Error Resume Next

  lastalarm = newalarm 
  Wscript.Sleep(5000)

  '------------------------------------------------------------------------
  'Connect to the MT CONnect agent and read xml
  '------------------------------------------------------------------------
  xmlDoc1.Load("http://" + item + "/current")

  Set rootNode = xmlDoc1.documentElement

  newalarm = rootNode.selectSingleNode("//*[@name='alarm']").text

  If Err.Number <> 0 Then 
	Wscript.Quit(0)
  End if 

  '------------------------------------------------------------------------
  'If new alarm, then add the data from the agent into the Excel cells for the current row
  '------------------------------------------------------------------------
  if newalarm <> lastalarm AND NOT IsEmpty(newalarm)   AND newalarm<>"UNAVAILABLE" then

  	objXL.Cells(2+i,TIMESTAMP).Value = FormatDateTime(Now)
  	objXL.Cells(2+i,OWNER).Value = item
  	objXL.Cells(2+i,MODE).Value = rootNode.selectSingleNode("//*[@name='mode']").text
  	objXL.Cells(2+i,EXECUTION).Value = rootNode.selectSingleNode("//*[@name='execution']").text
  	objXL.Cells(2+i,PROGRAM).Value = rootNode.selectSingleNode("//*[@name='program']").text
  	objXL.Cells(2+i,POWER).Value = rootNode.selectSingleNode("//*[@name='power']").text

  	objXL.Columns("A:I").AutoFit

  	i=i+1 
  end if
Next


Loop
