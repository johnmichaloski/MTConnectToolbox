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
array(0) = "127.0.0.1"

array(0) = Inputbox ("Machine that you would like monitor on:", "Machine", "127.0.0.1")
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
objXL.Cells(1,OWNER).Value = "Machine"
objXL.Cells(1,POWER).Value = "Power"
objXL.Cells(1,MODE).Value = "Mode"
objXL.Cells(1,EXECUTION).Value = "Execution"
objXL.Cells(1,PROGRAM).Value = "Program"


objXL.Rows("2:2").Select
objXL.ActiveWindow.FreezePanes = True
'END CALLOUT B

'------------------------------------------------------------------------
'Connect to the MT CONnect agent and read xml
'------------------------------------------------------------------------
Dim xmlDoc1 
Dim rootNode 
dim i,j

Set xmlDoc1 = CreateObject("Msxml2.DOMDocument")
xmlDoc1.async = False
xmlDoc1.setProperty "ServerHTTPRequest", true
i=0

Do While True

For Each item In array
On Error Resume Next

   xmlDoc1.Load("http://" + item + ":5000/current")



  '------------------------------------------------------------------------
  'Add the data from the agent into the Excel cells for the current row
  '------------------------------------------------------------------------
  'BEGIN CALLOUT G
  Set rootNode = xmlDoc1.documentElement
  objXL.Cells(2+i,TIMESTAMP).Value = FormatDateTime(Now)
  objXL.Cells(2+i,OWNER).Value = item
  objXL.Cells(2+i,MODE).Value = rootNode.selectSingleNode("//*[@name='mode']").text
  objXL.Cells(2+i,EXECUTION).Value = rootNode.selectSingleNode("//*[@name='execution']").text
  objXL.Cells(2+i,PROGRAM).Value = rootNode.selectSingleNode("//*[@name='program']").text
'On Error Goto 0
  On Error Resume Next
 objXL.Cells(2+i,POWER).Value = rootNode.selectSingleNode("//*[@name='power']").text
	If Err.Number <> 0 Then 
 	    Wscript.Quit(0)
	End if 

  objXL.Columns("A:I").AutoFit

  i=i+1 
Next
Wscript.Sleep(100)

Loop
