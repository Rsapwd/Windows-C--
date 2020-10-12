#include "RReg.h"

RReg::RReg(HKEY _mainKey)
{
	this->m_main_key = _mainKey;
}
RReg::~RReg()
{
	
}

BOOL RReg::ExistsSubKey(wstring _subKey)
{
	return this->GetSubKeyValue(_subKey).compare(L"") != 0;
}

wstring RReg::GetSubKeyValue(wstring _subKey)
{
	WCHAR key[0x100] = { 0 };
	HKEY hKey;
	DWORD dwtype = REG_SZ, sl = 256;
	if (ERROR_SUCCESS == ::RegOpenKeyExW(this->m_main_key, _subKey.c_str(), NULL,
		KEY_QUERY_VALUE, &hKey)) {
		if (ERROR_FILE_NOT_FOUND == ::RegQueryValueExW(hKey,
			NULL, NULL, &dwtype, (LPBYTE)key, &sl)) {
			ZeroMemory(key, 0x100);
		}
	}
	::RegCloseKey(hKey);
	return key;
}

wstring RReg::GetDefaultAppNameByExt(wstring _ext)
{
	wstring name = L"";
	HKEY hKey = this->m_main_key;
	this->m_main_key = HKEY_CLASSES_ROOT;
	wstring extKey = this->GetSubKeyValue(_ext);
	if (0 != extKey.compare(L"")) {
		extKey += L"\\shell\\open\\command";
		wstring v = this->GetSubKeyValue(extKey);
		if (!v.empty()) {
			size_t index = v.find_last_of('\\');
			size_t i = v.find_first_of(' ', index + 1);
			name = v.substr(index + 1, i - index - 1);
			if ('\"' == name[name.length() - 1]) {
				name = name.substr(0, name.length() - 1);
			}
		}
	}
	this->m_main_key = hKey;
	return name;
}

BOOL RReg::Write(wstring _subKey, wstring _newKey, wstring _key, 
	DWORD _value)
{
	BOOL ret = FALSE;
	HKEY hKey;
	if (this->CreateKey(_subKey, _newKey, hKey)) {
		if (ERROR_SUCCESS == ::RegSetValueExW(hKey,
			_key.c_str(), 0, REG_DWORD, (CONST BYTE*) & _value, sizeof(DWORD))) {
			ret = TRUE;
		}
		::RegCloseKey(hKey);
	}
	return ret;
}

BOOL RReg::Write(wstring _subKey, wstring _newKey, wstring _key,
	wstring _value)
{
	BOOL ret = FALSE;
	HKEY hKey;
	if (this->CreateKey(_subKey, _newKey, hKey)) {
		if (ERROR_SUCCESS == ::RegSetValueExW(hKey, _key.c_str(), 0, 
			REG_SZ, (CONST BYTE*)_value.c_str(), _value.size() * sizeof(WCHAR))) {
			ret = TRUE;
		}
		::RegCloseKey(hKey);
	}
	return ret;
}

BOOL RReg::Write(wstring _subKey, wstring _newKey, wstring _key,
	BYTE* _bytes, DWORD _size)
{
	BOOL ret = FALSE;
	HKEY hKey;
	if (this->CreateKey(_subKey, _newKey, hKey)) {
		if (ERROR_SUCCESS == ::RegSetValueExW(hKey, _key.c_str(), 0,
			REG_BINARY, _bytes, _size)) {
			ret = TRUE;
		}
		::RegCloseKey(hKey);
	}
	return ret;
}

BOOL RReg::CreateKey(wstring _subKey, wstring _newKey, HKEY& hKey)
{
	BOOL ret = FALSE;
	HKEY hKeyTmp = HKEY_CLASSES_ROOT;
	if (this->ExistsSubKey(_subKey)) {
		if (ERROR_SUCCESS == ::RegOpenKeyExW(this->m_main_key, _subKey.c_str(),
			0, KEY_SET_VALUE, &hKey)) {
			if (0 != _newKey.compare(L"")) {
				if (ERROR_SUCCESS == ::RegCreateKeyW(hKey, _newKey.c_str(), &hKeyTmp)) {
					ret = TRUE;
				}
			}
			else {
				ret = TRUE;
			}
		}
	}
	if (0 != _newKey.compare(L"")) {
		::RegCloseKey(hKey);
		if (ret) hKey = hKeyTmp;
	}
	return ret;
}