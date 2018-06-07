
@ECHO ON
@ECHO %CD%

IF NOT EXIST "C:\Program Files\ulapi" MKDIR "C:\Program Files\ulapi"
IF NOT EXIST "C:\Program Files\ulapi\include" MKDIR "C:\Program Files\ulapi\include"
IF NOT EXIST "C:\Program Files\ulapi\bin" MKDIR "C:\Program Files\ulapi\bin"
IF NOT EXIST "C:\Program Files\ulapi\lib" MKDIR "C:\Program Files\ulapi\lib"


COPY ..\src\*.h "C:\Program Files\ulapi\include"
COPY Debug\*.exe "C:\Program Files\ulapi\bin"
COPY Debug\*.lib "C:\Program Files\ulapi\lib"
COPY Debug\*.dll "C:\Program Files\ulapi\lib"

PAUSE
