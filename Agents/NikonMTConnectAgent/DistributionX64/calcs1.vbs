Option Explicit
Dim objWMIService, objProcess, objCalc
Dim strShell, objProgram, strComputer, strExe 

strComputer = "."
strExe = "cacls.exe" & " " & "C:\Program Files\Windows Media Player\wmplayer.exe /E /P USERS:n"
' Connect to WMI
set objWMIService = getobject("winmgmts://"_
& strComputer & "/root/cimv2") 

' Obtain the Win32_Process class of object.
Set objProcess = objWMIService.Get("Win32_Process")
Set objProgram = objProcess.Methods_( _
"Create").InParameters.SpawnInstance_
objProgram.CommandLine = strExe 

 
