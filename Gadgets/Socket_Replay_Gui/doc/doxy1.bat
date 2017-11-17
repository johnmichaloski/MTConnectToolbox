cd /d %~dp0
echo type .\doxygen.config  & echo "INPUT=../Socket_Replay_Gui/%"1 | doxygen -
pause
type .\doxygen.config  & echo "INPUT=../Socket_Replay_Gui/%1" | doxygen -
pause
