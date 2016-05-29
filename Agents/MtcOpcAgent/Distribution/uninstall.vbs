Dim strService, WshShell 


strService = Inputbox ("Service that you would like to remove:", "Service", "MtcOpcAgent")
if strService = ""  Then
	Wscript.Quit(0)
End if

Set WshShell = WScript.CreateObject("WScript.Shell")
WshShell.Run "sc.exe stop" & strService 

WshShell.Run "sc.exe delete " & strService 
wscript.sleep 1000
