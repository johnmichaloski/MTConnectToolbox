

Function GetProductVersion (sFilePath, sProgram)
Dim objShell, objFolder, objFolderItem, i 
If FSO.FileExists(sFilePath & "\" & sProgram) Then
    Set objShell = CreateObject("Shell.Application")
    Set objFolder = objShell.Namespace(sFilePath)
    Set objFolderItem = objFolder.ParseName(sProgram)
    Dim arrHeaders(300)
    For i = 0 To 300
        arrHeaders(i) = objFolder.GetDetailsOf(objFolder.Items, i)
        'WScript.Echo i &"- " & arrHeaders(i) & ": " & objFolder.GetDetailsOf(objFolderItem, i)
        If lcase(arrHeaders(i))= "product version" Then
            GetProductVersion= objFolder.GetDetailsOf(objFolderItem, i)
            Exit For
        End If
    Next
End If
End Function

Set objFSO = CreateObject("Scripting.FileSystemObject")
Wscript.Echo objFSO.GetFileVersion("c:\windows\system32\scrrun.dll")