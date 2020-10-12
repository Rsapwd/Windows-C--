#include "RSystem.h"
#include <algorithm>

RSystem::RSystem()
{
	this->m_list_ip.clear();
}
RSystem::~RSystem()
{

}

void RSystem::Update()
{
	this->UpdateIpList();
	this->UpdateSystemInfo();
	this->UpdateComputerName();
}

wstring RSystem::GetCmdInfo(wstring _cmd)
{
	wstring ret = L"";
	HANDLE hRead = NULL;
	HANDLE hWrite = NULL;
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;
	if (::CreatePipe(&hRead, &hWrite, &sa, 0)) {
		PROCESS_INFORMATION pi;
		ZeroMemory(&pi, sizeof(pi));
		STARTUPINFOW si;
		si.cb = sizeof(STARTUPINFOW);
		GetStartupInfoW(&si);
		si.hStdError = hRead;
		si.hStdOutput = hWrite;
		si.wShowWindow = SW_HIDE;
		si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
		wstring cmd = L"cmd /c " + _cmd;
		if (::CreateProcessW(NULL, (LPWSTR)cmd.c_str() , NULL, NULL,
			TRUE, NULL, NULL, NULL, &si, &pi)) {
			WaitForSingleObject(pi.hProcess, INFINITE);
			wstring info = L"";
			while (true)
			{
				DWORD read = 0;
				BYTE bytes[0x1000] = { 0 };
				if (::ReadFile(hRead, bytes, 0x1000, &read, NULL)) {
					int num = ::MultiByteToWideChar(CP_ACP, 0, (char*)bytes, -1, NULL, 0);
					wchar_t* wide = new wchar_t[num];
					::MultiByteToWideChar(CP_ACP, 0, (char*)bytes, -1, wide, num);
					info += wide;
					delete[] wide;
					if (0x1000 > read) break;
					Sleep(100);
				}
				else break;
			}
			ret = info;
		}
		::CloseHandle(pi.hThread);
	}
	::CloseHandle(hRead);
	::CloseHandle(hWrite);
	return ret;
}

void RSystem::UpdateIpList()
{
	wstring wipconfig = this->GetCmdInfo(L"ipconfig");
	size_t offset = 0;
	do
	{
		size_t index = wipconfig.find_first_of('\n', offset);
		if (wstring::npos == index) break;
		wstring i = wipconfig.substr(offset, index - offset + 1);
		offset = index + 1;
		if (0 == i.compare(L"\r\n")) continue;
		i = i.substr(0, i.find_first_of('\r'));
		transform(i.begin(), i.end(), i.begin(), ::tolower);
		wstring key = i.substr(0, i.find_first_of(':'));
		wstring value = i.substr(i.find_first_of(':') + 1);
		if (0 == value.compare(L"\r\n") || 0 == key.compare(value)) continue;
		size_t index_i = i.find_first_of('i');
		size_t index_p = i.find_first_of('p');
		size_t index_v = i.find_first_of('v');
		size_t index_4 = i.find_first_of('4');
		if (wstring::npos != index_i && wstring::npos != index_p && \
			wstring::npos != index_v && wstring::npos != index_4) {
			if (index_i + 1 == index_p && index_p + 1 == index_v &&
				index_v + 1 == index_4) {
				this->m_list_ip.push_back(this->RemoveSpace(value));
			}
		}
	} while (true);
}

void RSystem::UpdateSystemInfo()
{
	wstring info = this->GetCmdInfo(L"systeminfo");
	int c = 1;
	size_t offset = 0;
	do
	{
		size_t index = info.find_first_of('\n', offset);
		if (wstring::npos == index) break;
		wstring i = info.substr(offset, index - offset + 1);
		offset = index + 1;
		if (0 == i.compare(L"\r\n")) continue;
		i = i.substr(0, i.find_first_of('\r'));
		wstring key = i.substr(0, i.find_first_of(':'));
		wstring value = i.substr(i.find_first_of(':') + 1);
		switch (c)
		{
		case 1:
			this->m_host_name = this->RemoveSpace(value);
			break;
		case 2:
			this->m_os_name = this->RemoveSpace(value);
			break;
		case 3:
			this->m_os_version = this->RemoveSpace(value);
			break;
		case 9:
			this->m_product_id = this->RemoveSpace(value);
			break;
		case 10:
			this->m_install_date = this->RemoveSpace(value);
			break;
		case 11:
			this->m_boot_time = this->RemoveSpace(value);
			break;
		case 12:
			this->m_system = this->RemoveSpace(value);
			break;
		case 13:
			this->m_system += L" " + this->RemoveSpace(value);
			break;
		case 14:
			this->m_system += L" " + this->RemoveSpace(value);
			break;
		default:
		{
			transform(key.begin(), key.end(), key.begin(), ::tolower);
			size_t index_b = key.find_first_of('b');
			size_t index_i = key.find_first_of('i');
			size_t index_o = key.find_first_of('o');
			size_t index_s = key.find_first_of('s');
			if (wstring::npos != index_b && wstring::npos != index_i && \
				wstring::npos != index_o && wstring::npos != index_s) {
				if (index_b + 1 == index_i && index_i + 1 == index_o &&
					index_o + 1 == index_s) {
					this->m_bios_version = this->RemoveSpace(value);
				}
			}
			break;
		}
		}
		c++;
	} while (true);
}

void RSystem::UpdateComputerName()
{
	DWORD size = 0;
	::GetComputerNameW(NULL, &size);
	if (size > 0) {
		wchar_t* wptr = new wchar_t[size];
		if (::GetComputerNameW(wptr, &size)) {
			this->m_computer_name = wptr;
		}
		delete[] wptr;
	}
}

wstring RSystem::RemoveSpace(wstring _p)
{
	size_t index = 0;
	do
	{
		index = _p.find_first_of(' ');
		if (wstring::npos == index || 0 != index) break;
		_p = _p.substr(1);
	} while (true);
	do
	{
		index = _p.find_last_of(' ');
		if (wstring::npos == index || (_p.length() - 1) != index) break;
		_p = _p.substr(0, _p.length() - 1);
	} while (true);
	return _p;
}