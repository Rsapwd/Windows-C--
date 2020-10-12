/********************************************************************************
***************************** Custom Thread class *******************************
* Simple thread call.
* The all parameter is default.
* Just set the address and param.
********************************************************************************/
#pragma once
#include "Header.h"
class RThread
{
public:
	RThread(LPVOID lpParameter, LPTHREAD_START_ROUTINE m_lpStartAddress);
	~RThread();

	void Stop();
	void WaitFor();
	BOOL StartUp();
	DWORD GetThreadID();

protected:
	HANDLE m_hThread;
	DWORD m_dwThreadID;
	LPVOID m_lpParameter;
	LPTHREAD_START_ROUTINE m_lpStartAddress;
};

