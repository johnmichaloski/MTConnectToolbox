' Rename.vbs  -  Rename files by removing certain patterns and extension for the file
'
Option Explicit

dim path 
dim oldstr 
dim newStr 
Dim fso
	

'path = Inputbox ("Path:", "Path", "C:\Users\michalos\Documents\GitHub\MTConnectToolbox\MTConnectCmdSimAgent")
oldStr = Inputbox ("OldString:", "String", "MTConnectCmdSim")
newStr= Inputbox ("NewString:", "String", "MTConnectCmdSim")


Dim fso

' create a global copy of the filesystem object
Set fso = CreateObject("Scripting.FileSystemObject")


' Call the RecurseFolders routine with name of function to be performed
' Takes four arguments - the Path of the folder to be searched,
'                        the name of the function to be executed,
'                         the FROM string and the TO string
RecurseFolders "..", "RenameIt", oldStr, newStr
'RecurseFolders "..", "RenameIt", "..", "."
RecurseFolders "..", "RenameFLDR", oldStr, newStr

' echo the job is completed
WScript.Echo "Completed!"

Sub RecurseFolders(sPath, funcName, sfrom , sTo)
Dim folder, sCmd

  With fso.GetFolder(sPath)
    if .SubFolders.Count > 0 Then
      For each folder in .SubFolders
         sCmd = funcName & " " _
              & chr(34) & sFrom & chr(34) & "," _
              & chr(34) & sTo   & chr(34) & "," _
              & chr(34) & folder.Path & chr(34)

         ' Perform function's operation
         Execute sCmd

         ' Recurse to check for further subfolders
         RecurseFolders folder.Path, funcName, sfrom , sTo

      Next
    End if
  End With

End Sub

Sub RenameIt(sFrom, sTo, folPath) ' with error trapping added
Dim sName, fil

  set oFldr = fso.GetFolder(folPath)
  ' go thru each file in the folder
  For Each fil In oFldr.Files

  ' check if the file name contains the sFrom string
    If InStr(1, fil.Name, sFrom) <> 0 Then

  '   replace sFrom with sTo
      sName = Replace(fil.Name, sFrom, sTo)
 
      on error resume next
  '   rename the File
      fil.Name = sName



    End If

  Next
end sub

Sub RenameFLDR(sFrom, sTo, folPath) ' with error trapping added
Dim sName, oFldr

  set oFldr = fso.GetFolder(folPath)
  ' check if the file name contains the sFrom string
  If InStr(1, oFldr.Name, sFrom) <> 0 Then

  ' replace sFrom with sTo
    sName = Replace(oFldr.Name, sFrom, sTo)
 
    on error resume next
  ' rename the Folder
    oFldr.Name = sName

    if err.number <> 0 then
      oLogFile.Writeline "Folder Error: " & err.number & ", " & err.description _
        & " renaming " & oFldr.Name
    end if

  End If
  ' the ON Error scope ends at end of sub
end sub
