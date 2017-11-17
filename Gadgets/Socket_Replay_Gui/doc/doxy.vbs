

' Attempt to generate Doxygen one file at a time

Dim wShell, oShell, oExec, sFileSelected, scriptdir 

scriptdir = CreateObject("Scripting.FileSystemObject").GetParentFolderName(WScript.ScriptFullName)

Set wShell=CreateObject("WScript.Shell")
Set oExec=wShell.Exec("mshta.exe ""about:<input type=file id=FILE><script>FILE.click();new ActiveXObject('Scripting.FileSystemObject').GetStandardStream(1).WriteLine(FILE.value);close();resizeTo(0,0);</script>""")
sFileSelected = oExec.StdOut.ReadLine

Set oShell = WScript.CreateObject("WSCript.shell")
 
'oShell.run "cd /d %~dp0; type .\doxygen.config  & echo "INPUT=../Socket_Replay_Gui/Socket_Replay_Dlg.h" | doxygen -"

msgbox "type " & scriptdir & "\doxygen.config  & echo " & chr(34) & "INPUT=" & sFileSelected  & chr(34) & " | doxygen -"
' oShell.run "type " & scriptdir & "\doxygen.config  & echo " & chr(34) & "INPUT=" & sFileSelected  & chr(34) & " | doxygen -"

oShell.run "doxy1.bat " & chr(34) & sFileSelected & chr(34)
