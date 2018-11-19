Option Explicit

Dim thefile, thefolder, path


dim fso: set fso = CreateObject("Scripting.FileSystemObject")
dim CurrentDirectory
CurrentDirectory = fso.GetAbsolutePathName(".")

dim scriptdir
scriptdir = CreateObject("Scripting.FileSystemObject").GetParentFolderName(WScript.ScriptFullName)

Function SelectFolder( myStartFolder )
' This function opens a "Select Folder" dialog and will
' return the fully qualified path of the selected folder
'
' Argument:
'     myStartFolder    [string]    the root folder where you can start browsing;
'                                  if an empty string is used, browsing starts
'                                  on the local computer
'
' Returns:
' A string containing the fully qualified path of the selected folder
'
' Written by Rob van der Woude
' http://www.robvanderwoude.com

    ' Standard housekeeping
    Dim objFolder, objItem, objShell
    
    ' Custom error handling
    On Error Resume Next
    SelectFolder = vbNull

    ' Create a dialog object
    Set objShell  = CreateObject( "Shell.Application" )
    Set objFolder = objShell.BrowseForFolder( 0, "Select Folder", 0, myStartFolder )

    ' Return the path of the selected folder
    If IsObject( objfolder ) Then SelectFolder = objFolder.Self.Path

    ' Standard housekeeping
    Set objFolder = Nothing
    Set objshell  = Nothing
    On Error Goto 0
End Function


'
' Description: VBScript/VBS open file dialog
'              Compatible with most Windows platforms
' Author: wangye  <pcn88 at hotmail dot com>
' Website: http://wangye.org
'
' dir is the initial directory; if no directory is
' specified "Desktop" is used.
' filter is the file type filter; format "File type description|*.ext"
'
Public Function GetOpenFileName(dir, filter)
    Const msoFileDialogFilePicker = 3
 
    If VarType(dir) <> vbString Or dir="" Then
        dir = CreateObject( "WScript.Shell" ).SpecialFolders( "Desktop" )
    End If
 
    If VarType(filter) <> vbString Or filter="" Then
        filter = "All files|*.*"
    End If
 
    Dim i,j, objDialog, TryObjectNames
    TryObjectNames = Array( _
        "UserAccounts.CommonDialog", _
        "MSComDlg.CommonDialog", _
        "MSComDlg.CommonDialog.1", _
        "Word.Application", _
        "SAFRCFileDlg.FileOpen", _
        "InternetExplorer.Application" _
        )
 
    On Error Resume Next
    Err.Clear
 
    For i=0 To UBound(TryObjectNames)
        Set objDialog = WSH.CreateObject(TryObjectNames(i))
        If Err.Number<>0 Then
        Err.Clear
        Else
        Exit For
        End If
    Next
 
    Select Case i
        Case 0,1,2
        ' 0. UserAccounts.CommonDialog XP Only.
        ' 1.2. MSComDlg.CommonDialog MSCOMDLG32.OCX must registered.
        If i=0 Then
            objDialog.InitialDir = dir
        Else
            objDialog.InitDir = dir
        End If
        objDialog.Filter = filter
        If objDialog.ShowOpen Then
            GetOpenFileName = objDialog.FileName
        End If
        Case 3
        ' 3. Word.Application Microsoft Office must installed.
        objDialog.Visible = False
        Dim objOpenDialog, filtersInArray
        filtersInArray = Split(filter, "|")
        Set objOpenDialog = _
            objDialog.Application.FileDialog( _
                msoFileDialogFilePicker)
            With objOpenDialog
            .Title = "Open File(s):"
            .AllowMultiSelect = False
            .InitialFileName = dir
            .Filters.Clear
            For j=0 To UBound(filtersInArray) Step 2
                .Filters.Add filtersInArray(j), _
                     filtersInArray(j+1), 1
            Next
            If .Show And .SelectedItems.Count>0 Then
                GetOpenFileName = .SelectedItems(1)
            End If
            End With
            objDialog.Visible = True
            objDialog.Quit
        Set objOpenDialog = Nothing
        Case 4
        ' 4. SAFRCFileDlg.FileOpen xp 2003 only
        ' See http://www.robvanderwoude.com/vbstech_ui_fileopen.php
        If objDialog.OpenFileOpenDlg Then
           GetOpenFileName = objDialog.FileName
        End If
        Case 5
 
        Dim IEVersion,IEMajorVersion, hasCompleted
        hasCompleted = False
        Dim shell
        Set shell = CreateObject("WScript.Shell")
        ' ????IE??
        IEVersion = shell.RegRead( _
            "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Internet Explorer\Version")
        If InStr(IEVersion,".")>0 Then
            ' ??????
            IEMajorVersion = CInt(Left(IEVersion, InStr(IEVersion,".")-1))
            If IEMajorVersion>7 Then
                ' ???????7,?????IE7,???MSHTA??
                ' Bypasses c:\fakepath\file.txt problem
                ' http://pastebin.com/txVgnLBV
                Dim fso
                Set fso = CreateObject("Scripting.FileSystemObject")
 
                Dim tempFolder : Set tempFolder = fso.GetSpecialFolder(2)
                Dim tempName : tempName = fso.GetTempName()
                Dim tempFile : Set tempFile = tempFolder.CreateTextFile(tempName & ".hta")
                Dim tempBaseName
                tempBaseName = tempFolder & "\" & tempName
                tempFile.Write _
                    "<html>" & _
                    "  <head>" & _
                    "    <title>Browse</title>" & _
                    "  </head>" & _
                    "  <body>" & _
                    "    <input type='file' id='f'>" & _
                    "    <script type='text/javascript'>" & _
                    "      var f = document.getElementById('f');" & _
                    "      f.click();" & _
                    "      var fso = new ActiveXObject('Scripting.FileSystemObject');" & _
                    "      var file = fso.OpenTextFile('" & _
                              Replace(tempBaseName,"\", "\\") & ".txt" & "', 2, true);" & _
                    "      file.Write(f.value);" & _
                    "      file.Close();" & _
                    "      window.close();" & _
                    "    </script>" & _
                    "  </body>" & _
                    "</html>"
                tempFile.Close
                Set tempFile = Nothing
                Set tempFolder = Nothing
                shell.Run tempBaseName & ".hta", 1, True
                Set tempFile = fso.OpenTextFile(tempBaseName & ".txt", 1)
                GetOpenFileName = tempFile.ReadLine
                tempFile.Close
                fso.DeleteFile tempBaseName & ".hta"
                fso.DeleteFile tempBaseName & ".txt"
                Set tempFile = Nothing
                Set fso = Nothing
                hasCompleted = True ' ??????
            End If
        End If
        If Not hasCompleted Then
            ' 5. InternetExplorer.Application IE must installed
            objDialog.Navigate "about:blank"
            Dim objBody, objFileDialog
            Set objBody = _
                objDialog.document.getElementsByTagName("body")(0)
            objBody.innerHTML = "<input type='file' id='fileDialog'>"
            while objDialog.Busy Or objDialog.ReadyState <> 4
                WScript.sleep 10
            Wend
            Set objFileDialog = objDialog.document.all.fileDialog
                objFileDialog.click
                GetOpenFileName = objFileDialog.value
        End If
        objDialog.Quit
        Set objFileDialog = Nothing
        Set objBody = Nothing
        Set shell = Nothing
        Case Else
        ' Sorry I cannot do that!
    End Select
 
    Set objDialog = Nothing
End Function
Function ModFileDT(strDir, strFileName, DateTime)
     
    Dim objShell, objFolder
    
    Set objShell = CreateObject("Shell.Application")
    Set objFolder = objShell.NameSpace(strDir)
    objFolder.Items.Item(strFileName).ModifyDate = DateTime
End Function

' From: https://gist.github.com/mlhaufe/1034241
' unfreaking believable
'
'Bypasses IE7+ c:\fakepath\file.txt problem
Function BrowseForFile()
    With CreateObject("WScript.Shell")
        Dim fso : Set fso = CreateObject("Scripting.FileSystemObject")
        Dim tempFolder : Set tempFolder = fso.GetSpecialFolder(2)
        Dim tempName : tempName = fso.GetTempName() & ".hta"
        Dim path : path = "HKCU\Volatile Environment\MsgResp"
        With tempFolder.CreateTextFile(tempName)
            .Write "<input type=file name=f>" & _
            "<script>f.click();(new ActiveXObject('WScript.Shell'))" & _
            ".RegWrite('HKCU\\Volatile Environment\\MsgResp', f.value);" & _
            "close();</script>"
            .Close
        End With
        .Run tempFolder & "\" & tempName, 1, True
        BrowseForFile = .RegRead(path)
        .RegDelete path
        fso.DeleteFile tempFolder & "\" & tempName
    End With
End Function


path= BrowseForFile
if Len(path) = 0 then 
	msgbox "Abort"
	WScript.Quit
end if

thefile = fso.GetFileName(path)
thefolder = Left(path, Len(path)-Len(thefile))

ModFileDT thefolder , thefile, now

MsgBox path & " time has been modified to " & now

