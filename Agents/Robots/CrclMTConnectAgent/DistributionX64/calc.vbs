' CaclsExcel.vbs
' Example VBScript to set Cacls
' Version 2.4 - September 2010
' ---------------------------------------------------------' 
Option Explicit
Dim intRow, objExcel, objSheet, strPathExcel
Dim strHomeFolder, strHome, strUser
Dim objFSO, objShell, intRunError

' Note you will have to amend the following variables
strHome = "."
strPathExcel = "E:\Scripts\newUsers.xls"
intRow = 3 ' Row 1 contains headings

' Open the Excel spreadsheet
Set objFSO = CreateObject("Scripting.FileSystemObject")
Set objExcel = CreateObject("Excel.Application")
Set objSheet = objExcel.Workbooks.Open(strPathExcel)

' Create a shell for cmd and CACLS
Set objShell = CreateObject("Wscript.Shell")

' Here is the loop that cycles through the cells
Do Until (objExcel.Cells(intRow,1).Value) = ""
strUser = objExcel.Cells(intRow, 1).Value
call HomeDir ' I decided to use a subroutine
intRow = intRow + 1
Loop
objExcel.Quit ' Clears up Excel

Sub HomeDir()
strHomeFolder = strHome & strUser
If strHomeFolder <> "" Then
If Not objFSO.FolderExists(strHomeFolder) Then
On Error Resume Next
objFSO.CreateFolder strHomeFolder
     If Err.Number <> 0 Then
     On Error GoTo 0
     Wscript.Echo "Cannot create: " & strHomeFolder
     End If
On Error GoTo 0
End If
     If objFSO.FolderExists(strHomeFolder) Then
     ' Assign user permission to home folder.
     intRunError = objShell.Run("%COMSPEC% /c Echo Y| cacls "_
     & strHomeFolder & " /t /c /g Administrators:f "_
     & strUser & ":F", 2, True)
        If intRunError <> 0 Then
        Wscript.Echo "Error assigning permissions for user " _
        & strUser & " to home folder " & strHomeFolder
        End If
    End If
End If
End Sub
objExcel.Quit

WScript.Quit

' End of Cacls example VBScript