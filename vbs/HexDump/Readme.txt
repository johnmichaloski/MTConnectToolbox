
HexDump.vbs
===========

This is a Microsoft Windows Visual Basic Script (vbs) that asks the user for 
a local file and then displays the text in binary format




Sample output for the file:
G:\WorkBackup\GitHub\Tech Providers\NIST\vbs\HexDump\OEEinterfaceFILE.txt

is 

32 30 31 36 2D 30 34 2D 30 35 20 31 37 3A 31 31  2016-04-05 17:11
3A 34 37 2C 54 52 55 45 2C 31 30 30 2C 46 41 4C  :47,TRUE,100,FAL
53 45 2C 74 65 73 74 4E 61 6D 65 31 2C 38 34 37  SE,testName1,847
2E 32 32 35 2C 2D 31 30 35 32 2E 38 33 2C 36 38  .225,-1052.83,68
35 2E 32 31 31 0B 0D 0A 32 30 31 36 2D 30 34 2D  5.211...2016-04-
30 35 20 31 37 3A 31 31 3A 34 39 2C 54 52 55 45  05 17:11:49,TRUE
2C 31 30 30 2C 46 41 4C 53 45 2C 74 65 73 74 4E  ,100,FALSE,testN
61 6D 65 31 2C 38 34 37 2E 32 32 36 2C 2D 31 30  ame1,847.226,-10
35 32 2E 38 33 2C 36 38 35 2E 32 31 33 0B 0D 0A  52.83,685.213...


where 16 hex values are displayed for each character followed by the 
equivalent printable ascii character. If the character is not printable,
a period is printed in its place. Hence the hex sequence 0B 0D 0A
maps into Ascii ...   or   followed by line feed and carriage return.