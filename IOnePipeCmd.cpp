#include "IOnePipeCmd.h"
DWORD WINAPI ThreadOnePipeCmdOutput(void* lpParam) 
{
	IOnePipeCmd* ptr = reinterpret_cast<IOnePipeCmd*>(lpParam);
	ptr->ThreadCallOutput();
	ptr->Close();
	return 0;
}

IOnePipeCmd::IOnePipeCmd()
{
	this->m_dwThreadID = 0;
	this->m_hThreadOutput = NULL;
	this->m_hRead = NULL;
	this->m_hWrite = NULL;
	ZeroMemory(&this->m_pi, sizeof(this->m_pi));
}
IOnePipeCmd::~IOnePipeCmd()
{
	this->Close();
}

void IOnePipeCmd::Close()
{	if (NULL != this->m_pi.hProcess) {
		::TerminateProcess(this->m_pi.hProcess, 0);
		::CloseHandle(this->m_pi.hProcess);
	}
	if (NULL != this->m_pi.hThread) ::CloseHandle(this->m_pi.hThread);
	if (NULL != this->m_hThreadOutput) ::CloseHandle(this->m_hThreadOutput);
	if (NULL != this->m_hRead) ::CloseHandle(this->m_hRead);
	if (NULL != this->m_hWrite) ::CloseHandle(this->m_hWrite);
}

void IOnePipeCmd::ThreadCallOutput()
{
	BYTE bytes[ONE_PIPE_BUFFER_SIZE] = { 0 };
	DWORD readed = 0;
	::WaitForSingleObject(this->m_pi.hProcess, INFINITE);
	while (true)
	{
		::memset(bytes, 0, ONE_PIPE_BUFFER_SIZE);
		if (::ReadFile(this->m_hRead, bytes, ONE_PIPE_BUFFER_SIZE, &readed, NULL)) {
			this->Out(bytes, readed);
			Sleep(100);
		}
		else break;
	}
}

BOOL IOnePipeCmd::Write(const BYTE* _ptr, const size_t _size)
{
	BOOL ret = FALSE;
	if (0 < _size) {
		const char* p = "cmd /c ";
		int len = ::lstrlenA(p);
		BYTE* ptr = new BYTE[_size + 2 + len];
		::memset(ptr, 0, _size + 2 + len);
		::memcpy(ptr, p, len);
		::memcpy(ptr + len, _ptr, _size);
		if (this->CreatePipe()) {
			STARTUPINFOA si;
			si.cb = sizeof(STARTUPINFO);
			GetStartupInfoA(&si);
			si.hStdError = this->m_hRead;
			si.hStdOutput = this->m_hWrite;
			si.wShowWindow = SW_HIDE;
			si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
			BOOL bRes = ::CreateProcessA(NULL,
				(LPSTR)ptr, NULL, NULL, TRUE, NULL, NULL, NULL,
				&si, &this->m_pi);
			if (bRes) {
				this->m_hThreadOutput = ::CreateThread(NULL, NULL,
					ThreadOnePipeCmdOutput, this, NULL, &this->m_dwThreadID);
				if (0 != this->m_dwThreadID) {
					ret = TRUE;
				}
			}
		}
		delete[] ptr;
	}
	if (!ret) this->Close();
	return ret;
}

BOOL IOnePipeCmd::CreatePipe()
{
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;
	return ::CreatePipe(&this->m_hRead, &this->m_hWrite, &sa, 0);
}
