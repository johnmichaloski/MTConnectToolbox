Option Explicit


Dim ip, port, outfile, bLoopFlag
dim telnetArray

ip = Inputbox ("IP:Port that you would like telnet:", "IP:Port", "127.0.0.1:7878")
if ip = ""  Then
	Wscript.Quit(0)
End if

telnetArray= Split(ip, ":")
'if ubound(a) < 2 then 
ip=telnetArray(0)
port=telnetArray(1)

'msgbox ip & " " & port


''''''''''''''''''''''''''''''''
'' Logging file name
Dim fso, exefolder 
Set fso = CreateObject("Scripting.FileSystemObject") 
exefolder = fso.GetParentFolderName(WScript.ScriptFullName) 


outfile = Inputbox ("Filename to log telnet output:", "Log file", "out.txt")
if outfile  = ""  Then
	Wscript.Quit(0)
End if

outfile= exefolder& "\"  & outfile
'Msgbox outfile
'WScript.Quit

'''''''''''''''''''''''''''''''''''''''''''''''''''''
'' Use send keys to command telnet
Dim cloner
Set cloner = CreateObject("WScript.Shell")
cloner.Run "cmd.exe"
WScript.Sleep 1000
cloner.SendKeys "telnet " & ip  & " " & port & " -f " & Chr(34) & outfile & Chr(34)
cloner.SendKeys("{Enter}")
'setupIE

Do While bLoopFlag
	WScript.Sleep 1000 ' longer sleep, slower reaction time
Loop
