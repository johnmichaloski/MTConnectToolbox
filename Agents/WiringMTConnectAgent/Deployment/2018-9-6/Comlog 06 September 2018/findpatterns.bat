
cd /d %~dp0
grep -e "/// CheckFullBufferRunResult" -e "Feeder Has Stopped" -e "The Run Finished" -e "New Run Started" -e "StartTheFeeder" .\Comlog06September2018.txt > ./matches.txt
pause
