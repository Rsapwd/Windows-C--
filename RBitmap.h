/********************************************************************************
****************************** Custom Bitmap class ******************************
* void Clear() to clear the bitmap data.
* BOOL Load() to load bitmap data.(From file or HBITMAP)
* BOOL Save() to save 24 bitmap picture.
* DWORD ToGray() to get gray bitmap data.
* DWORD ToBinary() to get binary bitmap data.
* BOOL SaveToGray() to save gray bitmap picture.
* BOOL SaveToBinary() to save binary bitmap picture.
********************************************************************************/
#pragma once
#include "Header.h"
#define RBMP_BINARY_DEFAULT 142
class RBitmap
{
public:
	RBitmap();
	~RBitmap();
	// Clear data.
	void Clear();
	// Load bitmap data from HBITMAP.
	BOOL Load(HBITMAP hBmp);
	// Load bitmap data from file.
	BOOL Load(wstring wpath);
	// Save 24 bitmap to a file.
	BOOL Save(wstring wpath);
	// Get gray bitmap data.(The param 'ptr' is nullptr to return data size)
	DWORD ToGray(BYTE* ptr = nullptr);
	// Get Binary bitmap data.(The param 'ptr' is nullptr to return data size)
	DWORD ToBinary(BYTE* ptr = nullptr, BYTE b = RBMP_BINARY_DEFAULT);
	// Save bitmap to gray picture.
	BOOL SaveToGray(wstring wpath);
	// Save bitmap to binary picture.
	// The param 'b' if gray picture over this, it will be 255. otherwise is 0.
	BOOL SaveToBinary(wstring wpath, BYTE b = RBMP_BINARY_DEFAULT);
protected:
	BYTE* m_ptr_data;
	DWORD m_size_of_data;
	// Load HBITMAP type to bitmap data.
	BOOL LoadHBitmapData(HBITMAP hBmp);
};
