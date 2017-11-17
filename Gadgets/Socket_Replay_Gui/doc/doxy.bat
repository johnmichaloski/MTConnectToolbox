cd /d %~dp0
type .\doxygen.config  & echo "INPUT=../Socket_Replay_Gui/Socket_Replay_Dlg.h" | doxygen -
pause
