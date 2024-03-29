// StrLib.cpp : DLL アプリケーション用にエクスポートされる関数を定義します。
//

#include "stdafx.h"
#include <stdio.h>
#include "StrLib.h"
#include <wchar.h> // for wide-character string functions
// shared memory section (requires /SECTION:shared,RWS in link options)
#pragma data_seg ("shared")
int iTotal = 0;
WCHAR szStrings[MAX_STRINGS][MAX_LENGTH + 1] = { '\0' };
#pragma data_seg () 
#pragma comment(linker,"/SECTION:shared,RWS")
int WINAPI DllMain(HINSTANCE hInstance, DWORD fdwReason, PVOID pvReserved)
{
	return TRUE;
}
EXPORT BOOL CALLBACK AddStringA(PCSTR pStringIn)
{
	BOOL bReturn;
	int iLength;
	PWSTR pWideStr;
	// Convert string to Unicode and call AddStringW
	iLength = MultiByteToWideChar(CP_ACP, 0, pStringIn, -1, NULL, 0);
	pWideStr = (PWSTR)malloc(iLength);
	MultiByteToWideChar(CP_ACP, 0, pStringIn, -1, pWideStr, iLength);
	bReturn = AddStringW(pWideStr);
	free(pWideStr);
	return bReturn;
}
EXPORT BOOL CALLBACK AddStringW(PCWSTR pStringIn)
{
	PWSTR pString;
	int i, iLength;

	if (iTotal == MAX_STRINGS - 1)
		return FALSE;
	if ((iLength = wcslen(pStringIn)) == 0)
		return FALSE;
	// Allocate memory for storing string, copy it, convert to uppercase
	pString = (PWSTR)malloc(sizeof(WCHAR) * (1 + iLength));
	wcscpy_s(pString, iLength + 1, pStringIn);
	_wcsupr_s(pString, iLength + 1);
	// Alphabetize the strings
	for (i = iTotal; i > 0; i--)
	{
		if (wcscmp(pString, szStrings[i - 1]) >= 0)
			break;
		wcscpy_s(szStrings[i], szStrings[i - 1]);
	}
	wcscpy_s(szStrings[i], pString);
	iTotal++;
	free(pString);
	return TRUE;
}
EXPORT BOOL CALLBACK DeleteStringA(PCSTR pStringIn)
{
	BOOL bReturn;
	int iLength;
	PWSTR pWideStr;
	// Convert string to Unicode and call DeleteStringW
	iLength = MultiByteToWideChar(CP_ACP, 0, pStringIn, -1, NULL, 0);
	pWideStr = (PWSTR)malloc(iLength);
	MultiByteToWideChar(CP_ACP, 0, pStringIn, -1, pWideStr, iLength);
	bReturn = DeleteStringW(pWideStr);
	free(pWideStr);
	return bReturn;
}
EXPORT BOOL CALLBACK DeleteStringW(PCWSTR pStringIn)
{
	int i, j;
	if (0 == wcslen(pStringIn))
		return FALSE;
	for (i = 0; i < iTotal; i++)
	{
		if (_wcsicmp(szStrings[i], pStringIn) == 0)
			break;
	}
	// If given string not in list, return without taking action
	if (i == iTotal)
		return FALSE;
	// Else adjust list downward
	for (j = i; j < iTotal; j++)
		wcscpy_s(szStrings[j], szStrings[j + 1]);
	szStrings[iTotal - 1][0] = '\0';
	return TRUE;
}
EXPORT int CALLBACK GetStringsA(GETSTRCB pfnGetStrCallBack, PVOID pParam)
{
	BOOL bReturn;
	int i, iLength;
	PSTR pAnsiStr;
	for (i = 0; i < iTotal; i++)
	{
		// Convert string from Unicode
		iLength = WideCharToMultiByte(CP_ACP, 0, szStrings[i], -1, NULL, 0, NULL, NULL);
		pAnsiStr = (PSTR)malloc(iLength);
		WideCharToMultiByte(CP_ACP, 0, szStrings[i], -1, pAnsiStr, iLength, NULL, NULL);
		// Call callback function
		bReturn = pfnGetStrCallBack((PCTSTR)pAnsiStr, pParam);

		if (bReturn == FALSE)
			return i + 1;
		free(pAnsiStr);
	}
	return iTotal;
}
EXPORT int CALLBACK GetStringsW(GETSTRCB pfnGetStrCallBack, PVOID pParam)
{
	BOOL bReturn;
	int i;

	for (i = 0; i < iTotal; i++)
	{
		bReturn = pfnGetStrCallBack(szStrings[i], pParam);
		if (bReturn == FALSE)
			return i + 1;
	}
	return iTotal;
}

