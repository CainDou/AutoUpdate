#include "stdafx.h"
#include "IniFile.h"


CIniFile::CIniFile()
{

}

CIniFile::~CIniFile()
{

}

CIniFile::CIniFile(LPCTSTR lpszName)
{
	m_strFileName = lpszName;
}

UINT CIniFile::GetInt(LPCTSTR lpSection, LPCTSTR lpKeyName, int nDefault)
{
	return GetPrivateProfileInt(lpSection, lpKeyName, nDefault, m_strFileName.c_str());
}

wstring CIniFile::GetString(LPCTSTR lpSection, LPCTSTR lpKeyName, LPCTSTR lpDefault)
{
	const int nBufSize = 1024;
	wchar_t strBuf[1024]=L"";
	GetPrivateProfileString(lpSection, lpKeyName, lpDefault, strBuf, nBufSize, m_strFileName.c_str());
	return (wstring)strBuf;
}

BOOL CIniFile::GetStruct(LPCTSTR lpszSection, LPCTSTR lpszKeyName, LPVOID lpStruct, UINT uSize)
{
	return GetPrivateProfileStruct(lpszSection, lpszKeyName, lpStruct, uSize, m_strFileName.c_str());
}

BOOL CIniFile::WriteInt(LPCTSTR lpszSection, LPCTSTR lpszKeyName, int nValue)
{
	wchar_t strBuf[1024];
	wsprintf(strBuf, L"%d", nValue);
	return WritePrivateProfileString(lpszSection, lpszKeyName, strBuf, m_strFileName.c_str());
}

BOOL CIniFile::WriteString(LPCTSTR lpszSection, LPCTSTR lpszKeyName, LPCTSTR lpString)
{
	return WritePrivateProfileString(lpszSection, lpszKeyName, lpString, m_strFileName.c_str());
}


BOOL CIniFile::WriteStruct(LPCTSTR lpszSection, LPCTSTR lpszKeyName, LPVOID lpStruct, UINT uSize)
{
	return WritePrivateProfileStruct(lpszSection, lpszKeyName, lpStruct, uSize, m_strFileName.c_str());
}

void CIniFile::SetFileName(LPCTSTR lpszFileName)
{
	m_strFileName = lpszFileName;
}

BOOL CIniFile::DeleteKey(LPCTSTR lpszSection, LPCTSTR lpszKeyName)
{
	return WritePrivateProfileString(lpszSection, lpszKeyName, NULL, m_strFileName.c_str());
}


CIniFile::CIniFile(LPCSTR lpszName)
{
	m_strFileNameA = lpszName;

}


BOOL CIniFile::DeleteKeyA(LPCSTR lpszSection, LPCSTR lpszKeyName)
{
	return WritePrivateProfileStringA(lpszSection, lpszKeyName, NULL, m_strFileNameA.c_str());
}

void CIniFile::SetFileNameA(LPCSTR lpszFileName)
{
	m_strFileNameA = lpszFileName;
}

BOOL CIniFile::WriteStructA(LPCSTR lpszSection, LPCSTR lpszKeyName, LPVOID lpStruct, UINT uSize)
{
	return WritePrivateProfileStructA(lpszSection, lpszKeyName, lpStruct, uSize, m_strFileNameA.c_str());
}

BOOL CIniFile::WriteStringA(LPCSTR lpszSection, LPCSTR lpszKeyName, LPCSTR lpString)
{
	return WritePrivateProfileStringA(lpszSection, lpszKeyName, lpString, m_strFileNameA.c_str());
}

BOOL CIniFile::WriteIntA(LPCSTR lpszSection, LPCSTR lpszKeyName, int nValue)
{
	char strBuf[1024];
	sprintf_s(strBuf, "%d", nValue);
	return WritePrivateProfileStringA(lpszSection, lpszKeyName, strBuf, m_strFileNameA.c_str());
}

BOOL CIniFile::GetStructA(LPCSTR lpszSection, LPCSTR lpszKeyName, LPVOID lpStruct, UINT uSize)
{
	return GetPrivateProfileStructA(lpszSection, lpszKeyName, lpStruct, uSize, m_strFileNameA.c_str());
}

string CIniFile::GetStringA(LPCSTR lpSection, LPCSTR lpKeyName, LPCSTR lpDefault)
{
	const int nBufSize = 1024;
	char strBuf[1024]="";
	GetPrivateProfileStringA(lpSection, lpKeyName, lpDefault, strBuf, nBufSize, m_strFileNameA.c_str());
	return (string)strBuf;
}

UINT CIniFile::GetIntA(LPCSTR lpSection, LPCSTR lpKeyName, int nDefault)
{
	return GetPrivateProfileIntA(lpSection, lpKeyName, nDefault, m_strFileNameA.c_str());
}


