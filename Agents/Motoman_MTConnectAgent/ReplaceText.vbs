

Dim oldstring, newstring, folderpath
oldstring="UR"
newstring="Motoman"
folderpath="C:\Users\michalos\Documents\GitHub\Agents\Motoman_MTConnectAgent"

Dim fso, folder, fc, afile(),file 
on error resume next


Sub FindAndReplace(strFilename, strFind, strReplace)
    If 0 = Len(strFilename) Then Exit Sub
    Set inputFile = CreateObject("Scripting.FileSystemObject").OpenTextFile(strFilename, 1)
    strInputFile = inputFile.ReadAll
    inputFile.Close
    Set inputFile = Nothing
    If 0 = Len(strInputFile) Then Exit Sub
    Set outputFile = CreateObject("Scripting.FileSystemObject").OpenTextFile(strFilename,2,true)
    outputFile.Write Replace(strInputFile, strFind, strReplace)
    outputFile.Close
    Set outputFile = Nothing
end Sub 

Sub addFile(iref, ofile)
	dim  icount
        'iref=Ubound(afile) 
	icount=0
        redim preserve afile(iref+1)
	afile(iref+icount)=ofile.path
End Sub

'the key function buildfiletree
function buildfiletree(srootvalid,afile,fso)
    'errorless return 0
    'else return error number first encountered
    'afile return preserveed as the state just before the error encountered.

    buildfiletree=0
    on error resume next
    set ofolder=fso.getfolder(srootvalid)
    ifound=ofolder.files.count
    if ifound<>0 then
        iref=ubound(afile) : icount=0
        redim preserve afile(iref+ifound)
        for each ofile in ofolder.files
            icount=icount+1
            afile(iref+icount)=ofile.path
        next
    end if
    if ofolder.subfolders.count<>0 then
        for each osubfolder in ofolder.subfolders
            iretdyn=buildfiletree(osubfolder.path,afile,fso)
        next
    end if
    set ofolder=nothing
    if iretdyn<>0 then
        buildfiletree=iretdyn
    elseif err.number<>0 then
        buildfiletree=err.number : err.clear
    end if
    on error goto 0
end function


  
Set fso = CreateObject("Scripting.FileSystemObject")
Set folder = fso.GetFolder(folderpath)

redim afile(1)

addFile Ubound(afile), fso.GetFile("C:\Users\michalos\Documents\GitHub\Agents\ABBMTConnectAgent\ABBAgent.sln")
addFile Ubound(afile), fso.GetFile("C:\Users\michalos\Documents\GitHub\Agents\ABBMTConnectAgent\ABBSetupX64\ABBSetupX64.vdproj")
addFile Ubound(afile), fso.GetFile("C:\Users\michalos\Documents\GitHub\Agents\ABBMTConnectAgent\ABBMsiSetup\ABBMsiSetup.vdproj")
folderpath="C:\Users\michalos\Documents\GitHub\Agents\ABBMTConnectAgent\ABBAgent"
buildfiletree folderpath,afile,fso 
folderpath="C:\Users\michalos\Documents\GitHub\Agents\ABBMTConnectAgent\ABBAgent\Distribution"
buildfiletree folderpath,afile,fso 
folderpath="C:\Users\michalos\Documents\GitHub\Agents\ABBMTConnectAgent\ABBAgent\DistributionX64"
buildfiletree folderpath,afile,fso 
folderpath="C:\Users\michalos\Documents\GitHub\Agents\ABBMTConnectAgent\ABBAgent\MyCustomAction"
buildfiletree folderpath,afile,fso 


For Each file In afile
	'msgbox file
	FindAndReplace file, oldstring, newstring
Next 


'FindAndReplace WScript.Arguments.Item(0), oldstring, newstring


WScript.Echo "Operation Complete"

