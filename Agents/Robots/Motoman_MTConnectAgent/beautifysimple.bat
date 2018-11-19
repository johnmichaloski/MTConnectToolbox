REM https://stackoverflow.com/questions/8397674/windows-batch-file-looping-through-directories-to-process-files#8398621 
cd UR_Agent
call :treeProcess
cd ..\NIST
call :treeProcess
goto :eof

:treeProcess
rem Do whatever you want here over the files of this subdir, for example:
for %%f in (*.cpp) do (
    echo uncrustify %%f
    "C:\Users\michalos\bin\uncrustify\uncrustify.exe"    -c "C:\Users\michalos\Documents\GitHub\Agents\UR_MTConnectAgent\doc\rr.cfg"    --no-backup "%%f"
)
for %%f in (*.h) do (
     echo uncrustify %%f
     "C:\Users\michalos\bin\uncrustify\uncrustify.exe" -c "C:\Users\michalos\Documents\GitHub\Agents\UR_MTConnectAgent\doc\rr.cfg"    --no-backup "%%f"
)

#exit /b
