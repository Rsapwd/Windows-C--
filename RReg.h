/********************************************************************************
********************************** Reg operate **********************************
* Get or set reg value.
********************************************************************************/
#pragma once
#include "Header.h"
class RReg
{
public:
	RReg(HKEY _mainKey = HKEY_CURRENT_USER);
	~RReg();
	BOOL ExistsSubKey(wstring _subKey);
	// REG_SZ default.
	wstring GetSubKeyValue(wstring _subKey);
	// The param ext such as L".xxx"
	wstring GetDefaultAppNameByExt(wstring _ext);
	// REG_DWORD
	BOOL Write(wstring _subKey, wstring _newKey, wstring _key, 
		DWORD _value);
	// REG_SZ
	BOOL Write(wstring _subKey, wstring _newKey, wstring _key, 
		wstring _value);
	// REG_BINARY
	BOOL Write(wstring _subKey, wstring _newKey, wstring _key,
		BYTE* _bytes, DWORD _size);
protected:
	HKEY m_main_key;
	BOOL CreateKey(wstring _subKey, wstring _newKey, HKEY& hKey);
};

