#include "IMonitorDirChanges.h"

IMonitorDirChanges::IMonitorDirChanges(wstring _dir)
{
	do
	{
		size_t pos = _dir.find('/');
		if (wstring::npos != pos) _dir.replace(pos, 1, '\\', 0);
		else break;
	} while (true);
	if ('\\' != _dir[_dir.length() - 1]) {
		_dir += '\\';
	}
	this->m_wpath_dir = _dir;
}
IMonitorDirChanges::~IMonitorDirChanges()
{

}

void IMonitorDirChanges::Running()
{
	if (FILE_ATTRIBUTE_DIRECTORY & ::GetFileAttributesW(this->m_wpath_dir.c_str())) {
		BYTE buff[0x1000] = { 0 };
		FILE_NOTIFY_INFORMATION* pnotify = (FILE_NOTIFY_INFORMATION*)buff;
		HANDLE hDir = ::CreateFileW(this->m_wpath_dir.c_str(), FILE_LIST_DIRECTORY,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
			NULL);
		if (INVALID_HANDLE_VALUE != hDir) {
			do
			{
				DWORD dw = 0;
				ZeroMemory(buff, 0x1000);
				BOOL ok = ::ReadDirectoryChangesW(hDir, &buff, 0x1000, TRUE,
					FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE,
					&dw, NULL, NULL);
				if (ok) {
					wstring wpath = L"";
					FILE_NOTIFY_INFORMATION* tmp = pnotify;
					if (tmp->FileNameLength) {
						DWORD dwLength = tmp->FileNameLength + 1;
						wchar_t* wptrFileName = new wchar_t[dwLength];
						if (wptrFileName) {
							::memset(wptrFileName, 0, dwLength * 2);
							::memcpy(wptrFileName, tmp->FileName, dwLength * 2);
							wpath = this->m_wpath_dir + wptrFileName;
						}
						delete[] wptrFileName;
					}
					MDC_TYPE dct;
					switch (tmp->Action)
					{
					case FILE_ACTION_ADDED:
					{
						dct = MDCT_ADD;
						break;
					}
					case FILE_ACTION_REMOVED:
					{
						dct = MDCT_REMOVE;
						break;
					}
					case FILE_ACTION_MODIFIED:
					{
						dct = MDCT_MODIFY;
						break;
					}
					case FILE_ACTION_RENAMED_OLD_NAME:
					{
						dct = MDCT_RENAME_OLD;
						break;
					}
					case FILE_ACTION_RENAMED_NEW_NAME:
					{
						dct = MDCT_RENAME_NEW;
						break;
					}
					default:
						dct = MDCT_UNKNOWN;
						break;
					}
					this->FileChange(wpath, dct);
					if (tmp->NextEntryOffset != 0 &&
						tmp->FileNameLength > 0 &&
						tmp->FileNameLength < MAX_PATH) {
						PFILE_NOTIFY_INFORMATION p = (PFILE_NOTIFY_INFORMATION)((char*)tmp + tmp->NextEntryOffset);
						this->FileChange(this->m_wpath_dir + p->FileName, MDCT_RENAME_NEW);
					}
				}
			} while (TRUE);
		}
		::CloseHandle(hDir);
	}
}
