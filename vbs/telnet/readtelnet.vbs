Option Explicit

On Error Resume Next

Dim WshShell
set WshShell=CreateObject("WScript.Shell")

WshShell.run "cmd.exe"
WScript.Sleep 1000

'Send commands to the window as needed - IP and commands need to be customized
'Step 1 - Telnet to remote IP'

WshShell.SendKeys "telnet 127.0.0.1 7878"
WshShell.SendKeys ("{Enter}")
WScript.Sleep 1000
'The section of code above will automatically open a command window and then telnet to the device on whatever specific port you need to connect. Replace “x’s” with your own IP.

'The sleep command will wait long enough for the device to respond and prompt your script for the next command. Make sure this wait time is long enough for that action to take place.

'Secondly, you need to send each command, one at a time, providing enough wait time between them for the telnet session to respond.

'Step 2 - Issue Commands with pauses'
WshShell.SendKeys ("{Enter}")
WScript.Sleep 1000
WshShell.SendKeys "5"
WshShell.SendKeys ("{Enter}")
WScript.Sleep 1000
In this example, I’ve issued the two commands that I noted above. First, I have the script send the “Enter” command, wait a second, then send a “5” and press “Enter” again. This short series of actions will perform exactly as though you were sitting in front of the telnet command window doing them yourself. You’ll just need to customize this script to perform the exact responses that your telnet session requires.

Finally, don’t forget to close the command window and end the script.

'Step 3 - Exit Command Window
WshShell.SendKeys "exit"
WshShell.SendKeys ("{Enter}")
WScript.Quit 