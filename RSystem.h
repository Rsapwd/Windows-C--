/********************************************************************************
****************************** System information *******************************
* Get system information.
* The computer name.
* The host name.
* The os name.
* The os version.
* The product id.
* The install date.
* The boot time.
* The system.
* The bios version.
* The all ip in system.
********************************************************************************/
#pragma once
#include "Header.h"
class RSystem
{
public:
	RSystem();
	~RSystem();
	void Update();

	list<wstring> m_list_ip;
	wstring m_computer_name = L"";
	wstring m_host_name = L"";
	wstring m_os_name = L"";
	wstring m_os_version = L"";
	wstring m_product_id = L"";
	wstring m_install_date = L"";
	wstring m_boot_time = L"";
	wstring m_system = L"";
	wstring m_bios_version = L"";

private:
	wstring GetCmdInfo(wstring _cmd);
	void UpdateIpList();
	void UpdateSystemInfo();
	void UpdateComputerName();
	wstring RemoveSpace(wstring _p);
};

