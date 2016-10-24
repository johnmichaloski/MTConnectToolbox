cd /d "%~dp0"

icacls *  /GRANT *S-1-1-0:(OI)(CI)F   /T /C
pause