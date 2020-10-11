#include "IDoublePipeCmd.h"
DWORD WINAPI ThreadDoublePipeCmdOutput(void* lpParam) {
    IDoublePipeCmd* ptr = reinterpret_cast<IDoublePipeCmd*>(lpParam);
    ptr->ThreadCallOutput();
    return 0;
}

IDoublePipeCmd::IDoublePipeCmd()
{
    this->m_hInWrite = NULL;
    this->m_hOutRead = NULL;
    this->m_hThreadOutput = NULL;
    this->m_dwThreadID = 0;
    this->m_is_running = FALSE;
    ::ZeroMemory(&this->m_pi, sizeof(this->m_pi));
    ::InitializeCriticalSection(&this->m_cs_running);
}
IDoublePipeCmd::~IDoublePipeCmd()
{
    this->Close();
    ::DeleteCriticalSection(&this->m_cs_running);
}

void IDoublePipeCmd::Close()
{
    this->SetIsRunning();
    if (NULL != this->m_pi.hProcess) {
        ::TerminateProcess(this->m_pi.hProcess, 0);
        ::CloseHandle(this->m_pi.hProcess);
    }
    if (NULL != this->m_pi.hThread) ::CloseHandle(this->m_pi.hThread);
    if (NULL != this->m_hThreadOutput) ::CloseHandle(this->m_hThreadOutput);
    if (NULL != this->m_hInWrite) ::CloseHandle(this->m_hInWrite);
    if (NULL != this->m_hOutRead) ::CloseHandle(this->m_hOutRead);
}

BOOL IDoublePipeCmd::Write(const BYTE* _ptr, const DWORD _size)
{
    DWORD written = 0;
    if (this->IsRunning()) {
        do
        {
            DWORD w = 0;
            if (!::WriteFile(this->m_hInWrite, _ptr + written, _size - written, &w, NULL))
                break;
            if (0 == w) break;
            written += w;
        } while (written < _size);
    }
    return _size == written;
}

DWORD IDoublePipeCmd::Read(BYTE* _ptrOut)
{
    DWORD readed = 0;
    if (this->IsRunning()) {
        if (!::ReadFile(this->m_hOutRead, _ptrOut, DOUBLE_PIPE_BUFFER_SIZE, &readed, NULL)) {
            readed = 0;
        }
    }
    return readed;
}

void IDoublePipeCmd::ThreadCallOutput()
{
    do
    {
        BYTE bytes[DOUBLE_PIPE_BUFFER_SIZE] = { 0 };
        DWORD read = 0;
        if (::PeekNamedPipe(this->m_hOutRead, bytes, DOUBLE_PIPE_BUFFER_SIZE, &read, 0, 0)) {
            if (read) {
                read = this->Read(bytes);
                this->Out(bytes, read);
            }
        }
        else Sleep(100);
    } while (this->IsRunning());
}

BOOL IDoublePipeCmd::IsRunning()
{
    EnterCriticalSection(&this->m_cs_running);
    BOOL ret = this->m_is_running;
    LeaveCriticalSection(&this->m_cs_running);
    return ret;
}
void IDoublePipeCmd::SetIsRunning(const BOOL _isRunning)
{
    EnterCriticalSection(&this->m_cs_running);
    this->m_is_running = _isRunning;
    LeaveCriticalSection(&this->m_cs_running);
}

BOOL IDoublePipeCmd::CreatePipe()
{
    HANDLE hStdInRead = NULL, hStdOutWrite = NULL;
    SECURITY_ATTRIBUTES saIn, saOut;
    saIn.nLength = sizeof(saIn);
    saIn.bInheritHandle = TRUE;
    saIn.lpSecurityDescriptor = NULL;
    BOOL bResIn = ::CreatePipe(&hStdInRead, &this->m_hInWrite, &saIn, 0);
    saOut.nLength = sizeof(saOut);
    saOut.bInheritHandle = TRUE;
    saOut.lpSecurityDescriptor = NULL;
    BOOL bResOut = ::CreatePipe(&this->m_hOutRead, &hStdOutWrite, &saOut, 0);

    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    si.wShowWindow = SW_HIDE;
    si.hStdInput = hStdInRead;
    si.hStdOutput = hStdOutWrite;
    si.hStdError = hStdOutWrite;
    TCHAR szCmdLine[] =L"CMD.EXE";
    BOOL bResPro = ::CreateProcessW(NULL, szCmdLine, 
        NULL, NULL, TRUE, 0, NULL, NULL, 
        &si, &this->m_pi);
    
    ::CloseHandle(hStdInRead);
    ::CloseHandle(hStdOutWrite);
    if (bResIn && bResOut && bResPro) {
        this->m_hThreadOutput = ::CreateThread(NULL, NULL, 
            ThreadDoublePipeCmdOutput, this, NULL, &this->m_dwThreadID);
        if (0 != this->m_dwThreadID) {
            this->SetIsRunning(TRUE);
            return TRUE;
        }
    }
    this->Close();
    return FALSE;
}
