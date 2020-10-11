/********************************************************************************
*********************** Double pipe execute shell command ***********************
* This is a virtual base class.
* You must inherit and implement the 'void Out()'.
* void Close() to close the double pipe.
* BOOL Write() to execute shell command.
* void ThreadCallOutput() to out pipe data.(Thread call, you can't call it.)
* BOOL CreatePipe() to create double pipe.
* BOOL IsRunning() check the double pipe state.
* void Out() The double pipe output data.(You must implement it)
********************************************************************************/
#pragma once
#include "Header.h"
#define DOUBLE_PIPE_BUFFER_SIZE 0xFF00
class IDoublePipeCmd
{
public:
	IDoublePipeCmd();
	~IDoublePipeCmd();
	void Close();
	// Write shell command to double pipe.
	BOOL Write(const BYTE* _ptr, const DWORD _size);
	// Thread call output.
	void ThreadCallOutput();
	// Create double pipe.(Start-Up)
	BOOL CreatePipe();
	// The double pipe status.
	BOOL IsRunning();
	// The pipe out data.(virtual function. must implement it.)
	virtual void Out(const BYTE* _ptr, const DWORD _size) = 0;

protected:
	// Read pipe out data.(The max size of once is DOUBLE_PIPE_BUFFER_SIZE)
	DWORD Read(BYTE* _ptrOut);
	// Set the double pipe running state.
	void SetIsRunning(const BOOL _isRunning = FALSE);
protected:
	BOOL m_is_running;
	CRITICAL_SECTION m_cs_running;
	PROCESS_INFORMATION m_pi;
	HANDLE m_hOutRead, m_hInWrite;
	HANDLE m_hThreadOutput;
	DWORD m_dwThreadID;
};

