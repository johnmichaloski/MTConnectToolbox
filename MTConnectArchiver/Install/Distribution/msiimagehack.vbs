Option Explicit

Const msiOpenDatabaseModeReadOnly = 0
Const msiOpenDatabaseModeTransact = 1
Dim bitmap 

' MSI Database sql variables
Dim installer
Dim database
Dim view
Dim query
Dim dbname  



Const ownprocess  = &H10
Const shareprocess  = &H20
Const notinteractive  = &H0
Const interactive  = &H100
Const errorcontrolnormal  = &H1
Const errorcontrolignore  = &H0
Const errorcontrolcritical  = &H3
Const startauto  = &H2
Const startdemand  = &H3
Const startdisabled  = &H4

' MSI Event values
Const InstallStart  = 1
Const InstallStop  = 2
Const InstallDelete  = 8
Const UninstallStart  = 16
Const UninstallStop  = 32
Const UninstallDelete  = 128

Dim Id ' As String
Dim SrvName 'As String
Dim Arguments 'As String
Dim DisplayName 'As String
Dim ServiceType 'As Long
Dim InteractiveLong 'As Long
Dim ErrorControl 'As Long
Dim StartLong 'As Long
Dim Remove 'As String
Dim RemoveLong 'As Long
Dim wait 'As Boolean
Dim WaitLong 'As Long
Dim EventLong 'As Long

Dim cleandbname 


''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
sub DeleteFile(filename)
dim filesys: Set filesys = CreateObject("Scripting.FileSystemObject") 
If filesys.FileExists(filename) Then
   filesys.DeleteFile filename
End If 
ENd Sub

''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
sub FileCopy(filename,tofilename)

dim filesys, fromfile
set filesys = CreateObject ("Scripting.FileSystemObject")
set fromfile = filesys.GetFile(filename)
fromfile.Copy(tofilename)
end sub
  

''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
Function sqlstring(str ) 
sqlstring = "'" & str & "'"
End Function


''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
Sub CheckError()
Dim message, errRec
If Err = 0 Then Exit Sub
message = Err.Source & " " & Hex(Err) & ": " & Err.Description
If Not installer Is Nothing Then
Set errRec = installer.LastErrorRecord
If Not errRec Is Nothing Then message = message & vbLf & errRec.FormatText
End If
Fail message
End Sub
Sub Fail(message)
MsgBox message
Debug.Print message

End Sub


''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
Function GetComponentString(filename )
'Dim szMSI
Dim query
Dim view ' As view
Dim record ' As record
Dim i
Dim nComponent: nComponent = 1
Dim nFileName:  nFileName = 2
Dim strs
Dim file, component
'On Error Resume Next

' If installer Is Nothing Then Set installer = CreateObject("WindowsInstaller.Installer")
' If database Is Nothing Then Set database = installer.OpenDatabase(dbname, 1): CheckError

query = "SELECT `Component_`, `FileName` from `File`"

Set view = database.OpenView(query): CheckError
view.Execute

Do

    Set record = view.Fetch
    If record Is Nothing Then Exit Do
    
    If record.IsNull(nComponent) = False Then component = record.StringData(nComponent)
    If record.IsNull(nFileName) = False Then file = record.StringData(nFileName)
    strs = Split(file, "|")
    If strs(1) = filename Then Exit Do
    
    
    
Loop
view.Close
GetComponentString = component
End Function

''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
'Purpose   :    Save a binary file into a table in the msi file Binary Table
'Inputs    :    dbname              msi database path
'Inputs    :    filename            binary file (e.g.,  bitmap) path
'Inputs    :    rec                 name int the `Data` field, e.g., usually "DefBannerBitmap"
' Source   :    http://msdn.microsoft.com/en-us/library/aa367523(v=vs.85).aspx

''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
Sub savebinary(dbname , filename , rec )

Dim recname: recname = "'" & rec & "'"
'Dim installer, record, database, view
Dim record, view
'Set installer = CreateObject("WindowsInstaller.Installer")

Set record = installer.CreateRecord(1)
record.SetStream 1, filename

'Set database = installer.OpenDatabase(dbname, msiOpenDatabaseModeTransact)
'Set view = database.OpenView("INSERT INTO `Binary` (`Name`, `Data`) VALUES ('DefBannerBitmap', ?)")
Set view = database.OpenView("UPDATE `Binary` SET `Data`=? WHERE `Name`='DefBannerBitmap'")

view.Execute record
database.Commit ' save changes
End Sub



Dim component 
Dim field
 Dim exefolder

         exefolder = WScript.ScriptFullName
        bitmap = exefolder & "mtconnect.JPG"
        dbname = "FanucShdrSetup.msi"

'On Error Resume Next
     Set installer = CreateObject("WindowsInstaller.Installer")
    Set database = installer.OpenDatabase(dbname, 1): CheckError
   
   
   '' Save default bitmap as binary image
    component = "'" & GetComponentString(SrvName & ".exe") & "'"
    savebinary dbname, bitmap, "DefBannerBitmap"

        
    
    ServiceInstallTable SrvName, component
    Set database = Nothing
    Set installer = Nothing




