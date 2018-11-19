
dim objShell, FSO, strPath, wshShell, UAC
Set objShell = CreateObject("Shell.Application")
Set FSO = CreateObject("Scripting.FileSystemObject")
strPath = FSO.GetParentFolderName (WScript.ScriptFullName) + "\NikonAgent.exe"
msgbox strPath


Set UAC = CreateObject("Shell.Application")
UAC.ShellExecute "sc.exe", "create NikonAgent binPath= " & strPath, "", "runas", 1

UAC.ShellExecute "sc.exe", "start NikonAgent " , "", "runas", 1



