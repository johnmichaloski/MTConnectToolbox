//
// Registry.h: interface for the CRegistry class.
//
#pragma once


class CRegistry  
{
public:
	CRegistry(HKEY hKey = HKEY_CURRENT_USER);
	virtual ~CRegistry();

	BOOL Open(HKEY hKey, LPCTSTR lpszPath);
	BOOL Open(LPCTSTR lpszPath);
	void Close();

	BOOL VerifyKey(LPCTSTR lpszPath);
	BOOL VerifyKey(HKEY hKey, LPCTSTR lpszPath);
	BOOL VerifyValue(LPCTSTR lpszValue);

	BOOL CreateKey(HKEY hKey, LPCTSTR lpszPath, DWORD dwOptions = REG_OPTION_NON_VOLATILE);
	BOOL DeleteKey(HKEY hKey, LPCTSTR lpszPath);
	BOOL DeleteValue(LPCTSTR lpszValue);

	BOOL WriteString(LPCTSTR lpszKey, LPCTSTR lpszData);
	BOOL WriteDWORD(LPCTSTR lpszKey, const DWORD& dwData);
//	BOOL Write(LPCTSTR lpszKey, CWindow &pWnd);

	BOOL ReadString(LPCTSTR lpszKey, CString& sData);
	BOOL ReadDWORD(LPCTSTR lpszKey, DWORD& dwData);
	//BOOL Read(LPCTSTR lpszKey, CWnd *pWnd);
	//BOOL Read(LPCTSTR lpszKey, CObject& object);

	// Templates...
	template<class T> BOOL WriteType(LPCTSTR lpszKey, T type)
	{
		ATLASSERT(lpszKey);
		std::stringstream ar;
	
		ar << type;		
	//	LONG lResult = RegSetValueEx(m_hKey, ar.str().c_str(), NULL, REG_BINARY, pByte, dwSize);
		LONG lResult = RegSetValueEx(m_hKey, lpszKey, NULL, REG_SZ, (const BYTE*) ar.str().c_str(), ar.str().size()+1);
				
		if(lResult == ERROR_SUCCESS)
			return TRUE;
	
		return FALSE;
	}

	template<class T> BOOL ReadType(LPCTSTR lpszKey, T& type)
	{
		ATLASSERT(lpszKey);

		CString sData;
		if(ReadString(lpszKey, sData))
		{
			std::istringstream stream((LPCSTR) sData);

			try {
			if(stream >> type)
				return TRUE;
			}
			catch(...){}

		}
		return FALSE;
	}

protected:
	HKEY		m_hKey;
	CString	m_sPath;

};

