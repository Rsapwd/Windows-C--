#include "RMonitorUSB.h"
#include <Dbt.h>

RMonitorUSB::RMonitorUSB()
{

}
RMonitorUSB::~RMonitorUSB()
{

}


#define THRD_MESSAGE_EXIT WM_USER + 1
static const GUID GUID_DEVINTERFACE_LIST[] =
{
	// GUID_DEVINTERFACE_USB_DEVICE
	{ 0xA5DCBF10, 0x6530, 0x11D2, { 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED } },
	// GUID_DEVINTERFACE_DISK
	{ 0x53f56307, 0xb6bf, 0x11d0, { 0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b } },
	// GUID_DEVINTERFACE_HID, 
	{ 0x4D1E55B2, 0xF16F, 0x11CF, { 0x88, 0xCB, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30 } },
	// GUID_NDIS_LAN_CLASS
	{ 0xad498944, 0x762f, 0x11d0, { 0x8d, 0xcb, 0x00, 0xc0, 0x4f, 0xc3, 0x35, 0x8c } }
};

void RMonitorUSB::UpdateDevice(char _disk, WPARAM wParam)
{
	string disk;
	disk[0] = _disk; disk += ":/";
	UINT type = ::GetDriveTypeA(disk.c_str());
	if (DBT_DEVICEARRIVAL == wParam) {
		// Insert
		printf_s("Insert£º %c   %d\r\n", _disk, type);
	}
	else {
		// Remove
		printf_s("Remove£º %c    %d\r\n", _disk, type);
	}
}

char FirstDriveFromMask(ULONG unitmask)
{
	char i;
	for (i = 0; i < 26; ++i)
	{
		if (unitmask & 0x1)
			break;
		unitmask >>= 1;
	}
	return (i + 'A');
}

LRESULT DeviceChange(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (DBT_DEVICEARRIVAL == wParam || DBT_DEVICEREMOVECOMPLETE == wParam)
	{
		PDEV_BROADCAST_HDR pHdr = (PDEV_BROADCAST_HDR)lParam;
		PDEV_BROADCAST_VOLUME pDevVolume;
		switch (pHdr->dbch_devicetype)
		{
		case DBT_DEVTYP_VOLUME:
			pDevVolume = (PDEV_BROADCAST_VOLUME)pHdr;
			char disk = FirstDriveFromMask(pDevVolume->dbcv_unitmask);
			UpdateDevice(disk, wParam);
			break;
		}
	}
	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
		break;
	case WM_SIZE:
		break;
	case WM_DEVICECHANGE:
		return DeviceChange(message, wParam, lParam);
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

DWORD WINAPI ThreadListenUSB(LPVOID lpParam)
{
	RMonitorUSB* ptr = reinterpret_cast<RMonitorUSB*>(lpParam);
	ptr->ThreadCallMonitorUSB();
	return 0;
}

void RMonitorUSB::Close()
{
	HWND hBtnClose = ::GetDlgItem(this->m_hWnd, 2);
	::PostMessage(hBtnClose, BM_CLICK, NULL, NULL);
	::CloseHandle(this->m_hThreadListen);
	this->m_hThreadListen = NULL;
	this->m_dwThreadID = 0;
}

BOOL RMonitorUSB::StartUp()
{
	this->m_hThreadListen = ::CreateThread(NULL, 0, 
		ThreadListenUSB, this, 0, &this->m_dwThreadID);
	if (NULL != this->m_hThreadListen) {
		::PostThreadMessage(this->m_dwThreadID, THRD_MESSAGE_EXIT, 0, 0);
		return TRUE;
	}
	return FALSE;
}

void RMonitorUSB::WaitFor()
{
	::WaitForSingleObject(this->m_hThreadListen, INFINITE);
}

void RMonitorUSB::ThreadCallMonitorUSB()
{
	WCHAR CLASS_NAME[] = L"Monitor USB";
	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = WndProc;
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpszClassName = CLASS_NAME;
	if (0 != ::RegisterClass(&wc)) {
		this->m_hWnd = ::CreateWindowEx(0, CLASS_NAME, L"", WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
			NULL, NULL, GetModuleHandle(NULL), NULL);
		if (NULL != this->m_hWnd) {
			HDEVNOTIFY hDevNotify;
			for (int i = 0; i < sizeof(GUID_DEVINTERFACE_LIST) / sizeof(GUID); i++)
			{
				DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
				ZeroMemory(&NotificationFilter, sizeof(NotificationFilter));
				NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
				NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
				NotificationFilter.dbcc_classguid = GUID_DEVINTERFACE_LIST[i];
				hDevNotify = ::RegisterDeviceNotification(this->m_hWnd, &NotificationFilter, DEVICE_NOTIFY_WINDOW_HANDLE);
			}
			// Message processing
			MSG msg;
			while (::GetMessage(&msg, NULL, 0, 0)) {
				if (msg.message == THRD_MESSAGE_EXIT) return;
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
		}
	}
}