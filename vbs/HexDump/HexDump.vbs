

Dim strAscii 
Dim strData 
Dim i , j
dim linehex, lineascii, c
Dim fso,objFile
Dim strFile

Function Send2IE(text)
	Dim objIE 
	Set objIE = WScript.CreateObject("InternetExplorer.Application", "IE_")
	objIE.visible = 1         ' keep visible
	objIE.navigate "about:blank"
	objIE.Document.Open
	objIE.Document.write "<HTML><BODY><H2><U>" & strFile & "</U></H2><PRE>" & text &  vbCrLf 
	objIE.Document.write "</BODY></HTML>" &  vbCrLf 
	objIE.Document.close
end function

Function GetFileName(mydir)
	Dim objDialog
	' Create a dialog object
	Set objDialog = CreateObject( "MSComDlg.CommonDialog" )
	objDialog.InitialDir = mydir
	objDialog.Filter = "All files|*.*"
	' Open the dialog and return the selected file name
	If objDialog.ShowOpen Then
 		 GetFileName = objDialog.FileName
	Else
		GetFileName = ""
	End If
End Function

'strFile = GetFileName(mydir)
'Set objFile = objFSO.OpenTextFile(strFile)
'strData = objFile.ReadAll

set fso=CreateObject("Scripting.FileSystemObject")
Set wShell=CreateObject("WScript.Shell")

Set oExec=wShell.Exec("mshta.exe ""about:<input type=file id=FILE><script>FILE.click();new ActiveXObject('Scripting.FileSystemObject').GetStandardStream(1).WriteLine(FILE.value);close();resizeTo(0,0);</script>""")
strFile =oExec.StdOut.ReadAll 

If Right(strFile, 2) = vbCrLf Then
  strFile = Left(strFile, Len(strFile) - 2)
End If 

Set objFile = fso.OpenTextFile(strFile,1,True)
strData = objFile.ReadAll


strAscii =""

For i = 1 to Len(strData)/16
	linehex=""
	lineascii=" "
	For j = 1 To 16 
		 c  = Mid(strData, (i-1)*16+j, 1)
		 linehex =  linehex &  right("0" & Hex(Asc(c)),2) & " "
		 if Asc(c) < 32 Or Asc(c) > 127 Then c="."
		 lineascii = lineascii & c 
	Next 
	strAscii = strAscii  & linehex  & lineascii  & chr(13)&chr(10)	

Next 

Send2IE(strAscii)
