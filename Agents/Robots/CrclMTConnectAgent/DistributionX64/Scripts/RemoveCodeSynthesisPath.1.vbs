'
' Change path
'

Function Tokenize(byVal TokenString, byRef TokenSeparators())

	Dim NumWords, a()
	NumWords = 0
	
	Dim NumSeps
	NumSeps = UBound(TokenSeparators)
	
	Do 
		Dim SepIndex, SepPosition
		SepPosition = 0
		SepIndex    = -1
		
		for i = 0 to NumSeps-1
		
			' Find location of separator in the string
			Dim pos
			pos = InStr(TokenString, TokenSeparators(i))
			
			' Is the separator present, and is it closest to the beginning of the string?
			If pos > 0 and ( (SepPosition = 0) or (pos < SepPosition) ) Then
				SepPosition = pos
				SepIndex    = i
			End If
			
		Next

		' Did we find any separators?	
		If SepIndex < 0 Then

			' None found - so the token is the remaining string
			redim preserve a(NumWords+1)
			a(NumWords) = TokenString
			
		Else

			' Found a token - pull out the substring		
			Dim substr
			substr = Trim(Left(TokenString, SepPosition-1))
	
			' Add the token to the list
			redim preserve a(NumWords+1)
			a(NumWords) = substr
		
			' Cutoff the token we just found
			Dim TrimPosition
			TrimPosition = SepPosition+Len(TokenSeparators(SepIndex))
			TokenString = Trim(Mid(TokenString, TrimPosition))
						
		End If	
		
		NumWords = NumWords + 1
	loop while (SepIndex >= 0)
	
	Tokenize = a
	
End Function

dim path, paths
Dim Seps(1)
Seps(0) = ";"

Set wshShell = CreateObject( "WScript.Shell" )
path =  wshShell.ExpandEnvironmentStrings( "%PATH%" )

paths=Tokenize(path, Seps)

Dim index  
Dim finalpath
index = LBound(paths)

finalpath=""
Do While index < UBound(paths)
   ' WScript.Echo(paths(index))
    if( InStr(paths(index) , "CodeSynthesis XSD 4.0")= 0) then 
	finalpath=finalpath&paths(index)&";"
    else
	'WScript.Echo("FOUND CodeSynthesis XSD 4.0")
    end if
    index = index  + 1
Loop
WScript.Echo(finalpath)

dim varSet
set varSet = wshShell.Environment("SYSTEM") 
varSet("Path") = finalpath
