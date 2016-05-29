

' These are global variable declarations
Dim PCDApp As PCDLRN.Application
Dim WithEvents AppEvents As PCDLRN.ApplicationObjectEvents

Sub Start()
' This is the subroutine to run to start the script
HideExcel
End Sub


Private Sub HideExcel()
Dim intAnswer As Integer
intAnswer = MsgBox("Do you want to make Excel invisible? For this test, you should
click Yes. It will become visible when you open a part program.", vbYesNo, "Hide Excel?")
If intAnswer = vbYes Then
Application.Visible = False
Else
Application.Visible = True
End If
LaunchPCDMIS
End Sub


Sub LaunchPCDMIS()
Set PCDApp = CreateObject("PCDLRN.Application")
Set AppEvents = PCDApp.ApplicationEvents
PCDApp.Visible = True
End Sub



Start()