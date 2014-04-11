
MtConnectDboardGuips.dll: dlldata.obj MtConnectDboardGui_p.obj MtConnectDboardGui_i.obj
	link /dll /out:MtConnectDboardGuips.dll /def:MtConnectDboardGuips.def /entry:DllMain dlldata.obj MtConnectDboardGui_p.obj MtConnectDboardGui_i.obj \
		kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL \
		$<

clean:
	@del MtConnectDboardGuips.dll
	@del MtConnectDboardGuips.lib
	@del MtConnectDboardGuips.exp
	@del dlldata.obj
	@del MtConnectDboardGui_p.obj
	@del MtConnectDboardGui_i.obj
