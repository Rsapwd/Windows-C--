#include "RThread.h"

RThread::RThread(LPVOID lpParameter, LPTHREAD_START_ROUTINE lpStartAddress)
{
	this->m_hThread = NULL;
	this->m_dwThreadID = 0;
	this->m_lpParameter = lpParameter;
	this->m_lpStartAddress = lpStartAddress;
}
RThread::~RThread()
{
	this->Stop();
}

void RThread::Stop()
{
	DWORD dw = ::WaitForSingleObject(this->m_hThread, 1000);
	if (dw != WAIT_OBJECT_0 && NULL != this->m_hThread) {
		::TerminateThread(this->m_hThread, 0);
	}
	::CloseHandle(this->m_hThread);
	this->m_hThread = NULL;
	this->m_dwThreadID = 0;
}

void RThread::WaitFor()
{
	if (NULL != this->m_hThread)
		::WaitForSingleObject(this->m_hThread, INFINITE);
}

BOOL RThread::StartUp()
{
	this->Stop();
	this->m_hThread = ::CreateThread(NULL, NULL,
		this->m_lpStartAddress, this->m_lpParameter, NULL, &this->m_dwThreadID);
	return NULL != this->m_hThread;
}

DWORD RThread::GetThreadID()
{
	return this->m_dwThreadID;
}
