Set objShell = CreateObject("Shell.Application")
Set FSO = CreateObject("Scripting.FileSystemObject")
strPath = FSO.GetParentFolderName (WScript.ScriptFullName) + "AgfmAgent.exe"

objShell.ShellExecute "cmd.exe /c sc.exe create AgfmAgent binPath= " & strPath ,"", "runas", 1


     objShell.ShellExecute "wscript.exe", _ 
        Chr(34) & strPath & "\installagent.vbs" & Chr(34), "", "runas", 1
