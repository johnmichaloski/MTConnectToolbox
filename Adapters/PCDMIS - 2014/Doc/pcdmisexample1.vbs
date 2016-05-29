

' These are global variable declarations
Dim PCDApp As PCDLRN.Application

Sub LaunchPCDMIS()
Set PCDApp = CreateObject("PCDLRN.Application")
Set AppEvents = PCDApp.ApplicationEvents
PCDApp.Visible = True
End Sub



LaunchPCDMIS