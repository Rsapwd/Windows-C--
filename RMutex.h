/********************************************************************************
************************************* Mutex *************************************
* BOOL Exists() to check the mutex is already exists.
* BOOL Create() to create mutex.
* void Release() to release mutex.
********************************************************************************/
#pragma once
#include "Header.h"
class RMutex
{
public:
	RMutex(string _name);
	~RMutex();
	BOOL Exists();
	BOOL Create();
	void Release();

protected:
	string m_name;
	HANDLE m_hMutex = NULL;
};

