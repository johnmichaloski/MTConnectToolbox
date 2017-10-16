
dim objShell, FSO, strPath, wshShell, UAC
Set objShell = CreateObject("Shell.Application")
Set FSO = CreateObject("Scripting.FileSystemObject")
strPath = FSO.GetParentFolderName (WScript.ScriptFullName) + "\UR_Agent.exe"
msgbox strPath


Set UAC = CreateObject("Shell.Application")
UAC.ShellExecute "sc.exe", "create UR_Agent binPath= " & strPath, "", "runas", 1

UAC.ShellExecute "sc.exe", "start UR_Agent " , "", "runas", 1



