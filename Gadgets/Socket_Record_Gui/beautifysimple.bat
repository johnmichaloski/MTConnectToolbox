REM https://stackoverflow.com/questions/8397674/windows-batch-file-looping-through-directories-to-process-files#8398621 
cd Socket_Record_Gui
call :treeProcess
goto :eof

:treeProcess
for %%f in (*.cpp) do (
    echo uncrustify %%f
    "C:\Users\michalos\bin\uncrustify\uncrustify.exe"    -c "..\doc\rr.cfg"    --no-backup "%%f"
)
for %%f in (*.h) do (
     echo uncrustify %%f
     "C:\Users\michalos\bin\uncrustify\uncrustify.exe" -c "..\doc\rr.cfg"    --no-backup "%%f"
)

#exit /b
