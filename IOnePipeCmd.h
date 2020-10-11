/********************************************************************************
************************ One pipe execute shell command *************************
* This is a virtual base class.
* You must inherit and implement the 'void Out()'.
* void Close() to close the double pipe.
* BOOL Write() to execute shell command.
* void ThreadCallOutput() to out pipe data.(Thread call, you can't call it.)
* void Out() The double pipe output data.(You must implement it)
********************************************************************************/
#pragma once
#include "Header.h"
#define ONE_PIPE_BUFFER_SIZE  0xFF00
class IOnePipeCmd
{
public:
	IOnePipeCmd();
	~IOnePipeCmd();
	void Close();
	//  Thread call output.
	void ThreadCallOutput();
	// Write shell command to one pipe.
	BOOL Write(const BYTE* _ptr, const size_t _size);
	// The pipe out data.(virtual function. must implement it.)
	virtual void Out(const BYTE* _ptr, const DWORD _size) = 0;
	
protected:
	HANDLE m_hThreadOutput;
	DWORD m_dwThreadID;
	HANDLE m_hRead, m_hWrite;
	PROCESS_INFORMATION m_pi;
	BOOL CreatePipe(); // Create one pipe thread.(Start-Up)
};

