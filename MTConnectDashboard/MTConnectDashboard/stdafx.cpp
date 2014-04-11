// stdafx.cpp : source file that includes just the standard includes
//	MtConnectDashboard.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"
#include "stdStringFcn.h"

#if (_ATL_VER < 0x0700)
#include <atlimpl.cpp>
#endif //(_ATL_VER < 0x0700)

#ifdef _ATL_STATIC_REGISTRY
#include <statreg.h>
#if (_ATL_VER < 0x0700)
#include <statreg.cpp>
#endif //(_ATL_VER < 0x0700)
#endif //_ATL_STATIC_REGISTRY
_bstr_t ErrorFormatMessage(HRESULT dwLastErr )
{
	TCHAR * pTemp;
	_bstr_t errmsg(L"");
	int nLen = ::FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_IGNORE_INSERTS |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, 
		dwLastErr,
		MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
		(LPTSTR) &pTemp, 
		1, 
		NULL );

	errmsg=(LPCTSTR) pTemp;
	::LocalFree( pTemp );
	return errmsg;
}

