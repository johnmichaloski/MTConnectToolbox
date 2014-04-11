//
// Registry.cpp: implementation of the CRegistry class.
//


#include "stdafx.h"
#include "Registry.h"

CRegistry::CRegistry(HKEY hKey)
{
	ATLASSERT(hKey);

	m_hKey = hKey;
}

CRegistry::~CRegistry()
{
	Close();
}

BOOL CRegistry::Open(HKEY hKey, LPCTSTR lpszPath)
{
	ATLASSERT(hKey);
	ATLASSERT(lpszPath);

	m_sPath = lpszPath;

	LONG lResult = RegOpenKeyEx(hKey, lpszPath, NULL, KEY_ALL_ACCESS, &m_hKey);

	if(lResult == ERROR_SUCCESS)
		return TRUE;

	return FALSE;
}

BOOL CRegistry::Open(LPCTSTR lpszPath)
{
	ATLASSERT(lpszPath);

	m_sPath = lpszPath;

	LONG lResult = RegOpenKeyEx(m_hKey, lpszPath, NULL, KEY_ALL_ACCESS, &m_hKey);

	if(lResult == ERROR_SUCCESS)
		return TRUE;

	return FALSE;
}

void CRegistry::Close()
{
	if (m_hKey)
	{
		RegCloseKey(m_hKey);
		m_hKey = NULL;
	}
}

BOOL CRegistry::VerifyKey(HKEY hKey, LPCTSTR lpszPath)
{
	ATLASSERT(hKey);
	ATLASSERT(lpszPath);

	LONG lResult = RegOpenKeyEx(hKey, lpszPath, NULL, KEY_ALL_ACCESS, &m_hKey);

	if(lResult == ERROR_SUCCESS)
		return TRUE;
	
	return FALSE;
}

BOOL CRegistry::VerifyKey(LPCTSTR lpszPath)
{
	ATLASSERT(lpszPath);

	LONG lResult = RegOpenKeyEx(m_hKey, lpszPath, NULL, KEY_ALL_ACCESS, &m_hKey);
	
	if(lResult == ERROR_SUCCESS)
		return TRUE;
	
	return FALSE;
}

BOOL CRegistry::VerifyValue(LPCTSTR lpszValue)
{
	ATLASSERT(lpszValue);

	LONG lResult = RegQueryValueEx(m_hKey, lpszValue, NULL, NULL, NULL, NULL);

	if(lResult == ERROR_SUCCESS)
		return TRUE;

	return FALSE;
}

BOOL CRegistry::CreateKey(HKEY hKey, LPCTSTR lpszPath, DWORD dwOptions)
{
	ATLASSERT(hKey);
	ATLASSERT(lpszPath);

	DWORD dwDisp = -1;

	LONG lResult = RegCreateKeyEx(hKey, lpszPath, NULL, NULL, dwOptions, KEY_ALL_ACCESS, NULL, &m_hKey, &dwDisp);

	if(lResult == ERROR_SUCCESS)
		return TRUE;

	return FALSE;
}

BOOL CRegistry::DeleteKey(HKEY hKey, LPCTSTR lpszPath)
{
	ATLASSERT(hKey);
	ATLASSERT(lpszPath);

	LONG lResult = RegDeleteKey(hKey, lpszPath);

	if(lResult == ERROR_SUCCESS)
		return TRUE;

	return FALSE;
}

BOOL CRegistry::DeleteValue(LPCTSTR lpszValue)
{
	ATLASSERT(lpszValue);

	LONG lResult = RegDeleteValue(m_hKey, lpszValue);

	if(lResult == ERROR_SUCCESS)
		return TRUE;

	return FALSE;
}

BOOL CRegistry::WriteString(LPCTSTR lpszKey, LPCTSTR lpszData)
{
	ATLASSERT(lpszKey);
	ATLASSERT(lpszData);

	LONG lResult = RegSetValueEx(m_hKey, lpszKey, NULL, REG_SZ, (const BYTE*) lpszData, strlen(lpszData) + 1);

	if(lResult == ERROR_SUCCESS)
		return TRUE;
	
	return FALSE;
}

BOOL CRegistry::ReadString(LPCTSTR lpszKey, CString &sData)
{
	ATLASSERT(lpszKey);

	DWORD dwType = -1;
	DWORD dwSize = -1;

	LONG lResult = RegQueryValueEx(m_hKey, (LPSTR) lpszKey, NULL, &dwType, NULL, &dwSize);

	if(lResult == ERROR_SUCCESS && dwType == REG_SZ)
	{
		UCHAR* pszData = new UCHAR[dwSize];
		lResult = RegQueryValueEx(m_hKey, (LPSTR) lpszKey, NULL, &dwType, pszData, &dwSize);
		if (lResult == ERROR_SUCCESS && dwType == REG_SZ)
		{
			sData = pszData;
			delete pszData;
			return TRUE;
		}
		delete pszData;
		return TRUE;
	}
	else
		return FALSE;
}

BOOL CRegistry::WriteDWORD(LPCTSTR lpszKey, const DWORD &dwData)
{
	ATLASSERT(lpszKey);
	
	LONG lResult = RegSetValueEx(m_hKey, lpszKey, NULL, REG_DWORD, (const BYTE*) &dwData, sizeof(DWORD));

	if(lResult == ERROR_SUCCESS)
		return TRUE;

	return FALSE;
}

BOOL CRegistry::ReadDWORD(LPCTSTR lpszKey, DWORD &dwData)
{
	ATLASSERT(lpszKey);

	DWORD dwType = -1;
	DWORD dwSize = sizeof(DWORD);

	LONG lResult = RegQueryValueEx(m_hKey, (LPSTR) lpszKey, NULL, &dwType, (BYTE*) &dwData, &dwSize);

	if(lResult == ERROR_SUCCESS && dwType == REG_DWORD)
		return TRUE;

	return FALSE;
}

//BOOL CRegistry::Write(LPCTSTR lpszKey, CWindow &pWnd)
//{
//	ATLASSERT(lpszKey);
//	ATLASSERT(pWnd);
//
//	WINDOWPLACEMENT wp;
//	wp.length = sizeof(WINDOWPLACEMENT);
//	pWnd.GetWindowPlacement(&wp);
//
//	std::stringstream ar;
//
//	ar << wp.flags
//		 << wp.showCmd
//		 << wp.ptMinPosition
//		 << wp.ptMaxPosition
//		 << wp.rcNormalPosition;
//
//
//	LONG lResult = RegSetValueEx(m_hKey, ar.str().c_str(), NULL, REG_BINARY, pByte, ar.str().size());
//
//	if (pByte)
//		free(pByte);
//
//	if(lResult == ERROR_SUCCESS)
//		return TRUE;
//	
//	return FALSE;
//}
//
//BOOL CRegistry::Read(LPCTSTR lpszKey, CWnd* pWnd)
//{
//	ATLASSERT(lpszKey);
//	ATLASSERT(pWnd);
//
//	DWORD dwType;
//	DWORD dwSize = sizeof(WINDOWPLACEMENT);
//	UCHAR* pszData = new UCHAR[dwSize];
//
//	WINDOWPLACEMENT wp;
//	wp.length = dwSize;
//
//	LONG lResult = RegQueryValueEx(m_hKey, lpszKey, NULL, &dwType, (BYTE*) pszData, &dwSize);
//
//	if(lResult == ERROR_SUCCESS && dwType == REG_BINARY)
//	{
//	std::stringstream ar;
//
//		ar >> wp.flags
//			 >> wp.showCmd
//			 >> wp.ptMinPosition
//			 >> wp.ptMaxPosition
//			 >> wp.rcNormalPosition;
//		
//		pWnd->SetWindowPlacement(&wp);
//
//		delete pszData;
//		return TRUE;
//	}
//
//	delete pszData;
//	return FALSE;
//}
//
//BOOL CRegistry::Write(LPCTSTR lpszKey, CObject& object)
//{
//	ATLASSERT(lpszKey);
//	
//	CMemFile file;
//	CArchive ar(&file, CArchive::store);
//	
//	object.Serialize(ar);
//	ar.Close();
//	
//	DWORD dwSize = file.GetLength();
//	#ifdef _DEBUG
//		if (dwSize > 2048)
//			TRACE1("CRegistry::Write - Object size = %d (> 2048) better use a file !\n", dwSize);
//	#endif
//	BYTE* pByte = file.Detach();
//	
//	LONG lResult = RegSetValueEx(m_hKey, lpszKey, NULL, REG_BINARY, pByte, dwSize);
//	
//	if (pByte)
//		free(pByte);
//	
//	if(lResult == ERROR_SUCCESS)
//		return TRUE;
//	
//	return FALSE;
//}
//
//BOOL CRegistry::Read(LPCTSTR lpszKey, CObject& object)
//{
//	ATLASSERT(lpszKey);
//	
//	DWORD dwType = -1;
//	DWORD dwSize = -1;
//	UCHAR *pszData = NULL;
//	
//	LONG lResult = RegQueryValueEx(m_hKey, lpszKey, NULL, &dwType, (BYTE*) pszData, &dwSize);
//	
//	if (lResult == ERROR_SUCCESS && dwType == REG_BINARY)
//		pszData = new UCHAR[dwSize];
//
//	lResult = RegQueryValueEx(m_hKey, lpszKey, NULL, &dwType, (BYTE*) pszData, &dwSize);
//	if (lResult == ERROR_SUCCESS && dwType == REG_BINARY)
//	{
//		CMemFile file(pszData, dwSize);
//		CArchive ar(&file, CArchive::load);
//		
//		object.Serialize(ar);
//		ar.Close();
//			
//		file.Close();
//			
//		delete pszData;
//		return TRUE;
//	}
//
//	delete pszData;
//	return FALSE;
//}
