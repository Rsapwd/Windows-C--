/********************************************************************************
************************************ Process ************************************
* void Close() to close the process.
* void WaitFor() to wait the process end.
* BOOL CreateCmd() to create a shell command process.
********************************************************************************/
#pragma once
#include "Header.h"
#pragma pack(push, 1)
struct TagProcess
{
	DWORD m_dwPID = 0;
	DWORD m_dwParentPID = 0;
	WCHAR m_wname[MAX_PATH] = { 0 };
};
#pragma pack(pop)
class RProcess
{
public:
	RProcess();
	~RProcess();
	void Close();
	void WaitFor();
	BOOL CreateCmd(const char* _ptr);
	BOOL CreateCmd(wchar_t* _wptr);

	static list<TagProcess*> GetSystemProcessList();
	static void ReleaseSystemProcessList(list<TagProcess*>& _list);
	static list<DWORD> GetPidListByName(wstring _name);
	static list<HWND> GetProcessHwndListByPid(DWORD _pid);
	static list<HWND> GetProcessHwndListByPidList(list<DWORD>& _listPid);
	static wstring GetHwndTitle(HWND _hWnd);
	static list<wstring> GetTitleListByHwndList(list<HWND>& _listHwnd);

protected:
	STARTUPINFOW m_si;
	PROCESS_INFORMATION m_pi;

private:
	void Init();
};

