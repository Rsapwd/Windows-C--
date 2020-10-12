/********************************************************************************
****************************** Monitor USB Changes ******************************
* void UpdateDevice(char _disk, WPARAM wParam);
* It is USB insert or remove to update.
********************************************************************************/
#pragma once
#include "Header.h"
class RMonitorUSB
{
public:
	RMonitorUSB();
	~RMonitorUSB();
	// Close thread
	void Close();
	// Start-up thread to monitor USB changes.
	BOOL StartUp();
	// Wait the thread over.
	void WaitFor();
	// Thread call functions. ((Thread call, you can't call it.))
	void ThreadCallMonitorUSB();
	// USB Changes call function.
	static void UpdateDevice(char _disk, WPARAM wParam);

protected:
	HWND m_hWnd = NULL;
	DWORD m_dwThreadID = 0;
	HANDLE m_hThreadListen = NULL;
};


