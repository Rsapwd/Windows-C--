#include "RMutex.h"

RMutex::RMutex(string _name)
{
	this->m_name = _name;
}
RMutex::~RMutex()
{
	this->Release();
}

BOOL RMutex::Exists()
{
	HANDLE hMutex = ::CreateMutexA(NULL, TRUE, this->m_name.c_str());
	if (ERROR_ALREADY_EXISTS == ::GetLastError()) return TRUE;
	if (NULL != hMutex) ::ReleaseMutex(hMutex);
	return FALSE;
}

BOOL RMutex::Create()
{
	this->Release();
	if (!this->Exists()) {
		this->m_hMutex = ::CreateMutexA(NULL, TRUE, this->m_name.c_str());
		if (NULL != this->m_hMutex) return TRUE;
	}
	return FALSE;
}

void RMutex::Release()
{
	if (NULL != this->m_hMutex) ::ReleaseMutex(this->m_hMutex);
	this->m_hMutex = NULL;
}