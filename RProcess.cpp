#include "RProcess.h"
#include <tlhelp32.h>

RProcess::RProcess() 
{
    this->Init();
}
RProcess::~RProcess()
{
    this->Close();
}


void RProcess::Close()
{
    ::TerminateProcess(this->m_pi.hProcess, 0);
    ::CloseHandle(this->m_pi.hProcess);
    ::CloseHandle(this->m_pi.hThread);
}


void RProcess::WaitFor()
{
    ::WaitForSingleObject(this->m_pi.hProcess, INFINITE);
}


BOOL RProcess::CreateCmd(const char* _ptr)
{
    int num = ::MultiByteToWideChar(CP_UTF8, 0, _ptr, -1, NULL, 0);
    wchar_t* wide = new wchar_t[num];
    ::MultiByteToWideChar(CP_UTF8, 0, _ptr, -1, wide, num);
    BOOL ret = this->CreateCmd(wide);
    delete[] wide;
    return ret;
}


BOOL RProcess::CreateCmd(wchar_t* _wptr)
{
    return ::CreateProcessW(NULL,   // No module name (use command line)
        _wptr,        // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &this->m_si,            // Pointer to STARTUPINFO structure
        &this->m_pi);           // Pointer to PROCESS_INFORMATION structure
}


void RProcess::Init()
{
    ZeroMemory(&this->m_si, sizeof(this->m_si));
    this->m_si.cb = sizeof(this->m_si);
    this->m_si.dwFlags = STARTF_USESHOWWINDOW;
    this->m_si.wShowWindow = SW_HIDE;
    ZeroMemory(&this->m_pi, sizeof(this->m_pi));
}

list<TagProcess*> RProcess::GetSystemProcessList()
{
    list<TagProcess*> listProcess;
    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(pe32);
    HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (INVALID_HANDLE_VALUE != hProcessSnap) {
        if (::Process32First(hProcessSnap, &pe32)) {
            do
            {
                TagProcess* ptr = new TagProcess;
                ptr->m_dwPID = pe32.th32ProcessID;
                ptr->m_dwParentPID = pe32.th32ParentProcessID;
                ::lstrcpyW(ptr->m_wname, pe32.szExeFile);
                listProcess.push_back(ptr);
            } while (::Process32NextW(hProcessSnap, &pe32));
        }
        ::CloseHandle(hProcessSnap);
    }
    return listProcess;
}

void RProcess::ReleaseSystemProcessList(list<TagProcess*>& _list)
{
    for (const TagProcess* ptr : _list)
    {
        delete ptr;
    }
    _list.clear();
}

static wstring WStringToUpperLower(wstring _param, BOOL _toUpper = TRUE)
{
    wstring ret = L"";
    for (const WCHAR& w : _param)
    {
        WCHAR wc = _toUpper ? ::toupper(w) : ::tolower(w);
        ret.push_back(wc);
    }
    return ret;
}

list<DWORD> RProcess::GetPidListByName(wstring _name)
{
    wstring name = WStringToUpperLower(_name);
    list<DWORD> listPid;
    list<TagProcess*> listTagProcess = RProcess::GetSystemProcessList();
    for (const TagProcess* ptr : listTagProcess)
    {
        wstring item = ptr->m_wname;
        if (0 == name.compare(WStringToUpperLower(item))) {
            listPid.push_back(ptr->m_dwPID);
        }
    }
    RProcess::ReleaseSystemProcessList(listTagProcess);
    return listPid;
}


typedef struct EnumHWndsArg
{
    list<HWND>* vecHWnds;
    DWORD dwProcessId;
}EnumHWndsArg, * LPEnumHWndsArg;
BOOL CALLBACK lpEnumFunc(HWND hwnd, LPARAM lParam)
{
    EnumHWndsArg* pArg = (LPEnumHWndsArg)lParam;
    DWORD  processId;
    GetWindowThreadProcessId(hwnd, &processId);
    if (processId == pArg->dwProcessId)
    {
        pArg->vecHWnds->push_back(hwnd);
    }
    return TRUE;
}
list<HWND> RProcess::GetProcessHwndListByPid(DWORD _pid)
{
    list<HWND> listHwnd;
    EnumHWndsArg wi;
    wi.dwProcessId = _pid;
    wi.vecHWnds = &listHwnd;
    EnumWindows(lpEnumFunc, (LPARAM)&wi);
    return listHwnd;
}

list<HWND> RProcess::GetProcessHwndListByPidList(list<DWORD>& _listPid)
{
    list<HWND> listHwnd;
    for (const DWORD& pid : _listPid)
    {
        list<HWND> lh = RProcess::GetProcessHwndListByPid(pid);
        for (const HWND& h : lh)
        {
            listHwnd.push_back(h);
        }
    }
    return listHwnd;
}

wstring RProcess::GetHwndTitle(HWND _hWnd)
{
    WCHAR title[MAX_PATH] = { 0 };
    ::GetWindowTextW(_hWnd, title, MAX_PATH);
    return title;
}

list<wstring> RProcess::GetTitleListByHwndList(list<HWND>& _listHwnd)
{
    list<wstring> listTitle;
    for (const HWND& hWnd : _listHwnd)
    {
        wstring title = GetHwndTitle(hWnd);
        if (0 == title.compare(L"")) continue;
        listTitle.push_back(title);
    }
    return listTitle;
}