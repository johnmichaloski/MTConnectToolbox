Dim fso

' create a global copy of the filesystem object
Set fso = CreateObject("Scripting.FileSystemObject")

Dim OLDSTRING
DIM NEWSTRING

OLDSTRING="ABB"
NEWSTRING="ABBftp"


Sub RecurseFolders(sPath, funcName)
Dim folder

  With fso.GetFolder(sPath)
    if .SubFolders.Count > 0 Then
      For each folder in .SubFolders

         ' Perform function's operation
         Execute funcName & " " & chr(34) & folder.Path & chr(34)

         ' Recurse to check for further subfolders
         RecurseFolders folder.Path, funcName
	 RenameFolder(folder.Path)
      Next
    End if
  End With

End Sub


Sub RenameFolder(path)
Dim sName, folPath
   set folPath=	fso.GetFolder(path)
  ' check if the file name contains underscore
    If InStr(1,folPath.Name, OLDSTRING) <> 0 Then

  ' replace '&' with 'and'
      sName = Replace(folPath.Name, OLDSTRING, NEWSTRING)
 
  ' rename the file
    folPath.Name = sName

    End If

end sub

Sub RenameIt(folPath)
Dim sName, fil

  ' go thru each file in the folder
  For Each fil In fso.GetFolder(folPath).Files

  ' check if the file name contains underscore
    If InStr(1, fil.Name, OLDSTRING) <> 0 Then

  ' replace '&' with 'and'
      sName = Replace(fil.Name, OLDSTRING, NEWSTRING)
 
  ' rename the file
    fil.Name = sName

    End If
  Next
end sub



' Call the RecurseFolders routine with name of function to be performed
' Takes one argument - in this case, the Path of the folder to be searched
RecurseFolders ".", "RenameIt"

' echo the job is completed
WScript.Echo "Completed!"

