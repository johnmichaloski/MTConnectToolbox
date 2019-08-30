

'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
dim objFS, objArgs, strFile, objFile, line, buildno, filestring

Set objFS = CreateObject("Scripting.FileSystemObject")

if WScript.Arguments.Count > 0 then
	Set objArgs = WScript.Arguments
	strFile= objArgs(0)
else
	strFile = "C:\Users\michalos\Documents\GitHub\Agent\MTConnectAgentFromShdr\MTConnectAgentFromShdr\version_info.h"
end if

Set objFile = objFS.OpenTextFile(strFile)

do while not objFile.AtEndOfStream 
    line =  objFile.ReadLine()

   if instr(1, line, "define VERSION_BUILD") then
       line = Trim(Mid(line, Len("#define VERSION_BUILD") + 1))
       'MsgBox "match " & line
       buildno = CInt(line) +1 
       line = "#define VERSION_BUILD " & CStr(buildno)
    end if 

      filestring = filestring & line & vbCRLF
loop

objFile.Close

'' Output file with updated version number

set objFile = objFS.CreateTextFile(strFile, 2)
objFile.Write filestring 
objFile.Close
