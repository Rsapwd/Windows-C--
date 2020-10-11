/********************************************************************************
*************************** Monitor Directory Changes ***************************
* This is a virtual base class.
* You must inherit and implement the 'void FileChange()'.
* void Running() to monitor the directory changes.
* void FileChange() The double pipe output data.(You must implement it)
********************************************************************************/
#pragma once
#include "Header.h"
#define MDCT_UNKNOWN      0
#define MDCT_ADD          1
#define MDCT_REMOVE       2
#define MDCT_MODIFY       3
#define MDCT_RENAME_OLD   4
#define MDCT_RENAME_NEW   5
typedef unsigned short MDC_TYPE;
class IMonitorDirChanges
{
public:
	IMonitorDirChanges(wstring _dir);
	~IMonitorDirChanges();
	// Monitor directory changes.
	void Running();
	// The directory changes record.(virtual function. must implement it.)
	virtual void FileChange(wstring _wpath, MDC_TYPE _type) = 0;

protected:
	wstring m_wpath_dir;
};

