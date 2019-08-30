'
' Add path
'

p1 = "C:\Program Files (x86)\CodeSynthesis XSD 4.0\bin\;"
p2 = "C:\Program Files (x86)\CodeSynthesis XSD 4.0\bin64\;"

dim path

Set wshShell = CreateObject( "WScript.Shell" )
path =  wshShell.ExpandEnvironmentStrings( "%PATH%" )

dim varSet
set varSet = wshShell.Environment("Path") 
varSet("Path") = p1&p2&path