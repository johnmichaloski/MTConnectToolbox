
cd /d %~dp0
  
REM C:\Users\michalos\Documents\bin\uncrustify-0.59-win32\uncrustify -c C:\Users\michalos\Documents\bin\uncrustify-0.59-win32/rr.cfg   --no-backup AsioCrclClient.h 
REM C:\Users\michalos\Documents\bin\uncrustify-0.59-win32\uncrustify -c C:\Users\michalos\Documents\bin\uncrustify-0.59-win32/rr.cfg   --no-backup AsioCrclClient.cpp 
REM  exit
REM C:\Users\michalos\Documents\GitHub\CRCL\CRCL2Robot\Crcl2Robot\AsioCrclClient.h
for %%g in (*.cpp) do (
   C:\Users\michalos\Documents\bin\uncrustify-0.59-win32\uncrustify -c C:\Users\michalos\Documents\bin\uncrustify-0.59-win32/rr.cfg    --no-backup %%g 
)

for %%g in (*.h) do (
   C:\Users\michalos\Documents\bin\uncrustify-0.59-win32\uncrustify -c C:\Users\michalos\Documents\bin\uncrustify-0.59-win32/rr.cfg    --no-backup %%g 
)

pause