
'======================================
' Comspec.vbs
' VBScript Comspec Example
' Author Guy Thomas http://computerperformance.co.uk/
' Ezine 80 Version 1.4 - July 2005
'======================================

Option Explicit
Dim objShell,strOriginalCD

Set objShell = CreateObject("WScript.Shell")
strOriginalCD = objShell.CurrentDirectory

objShell.Run "%comspec% /k" & superuser.bat
objShell.Run "%comspec% /k .\agfmagent.exe install" 
objShell.Run "%comspec% /k .\sc.exe start agfmagent"

WScript.Quit
