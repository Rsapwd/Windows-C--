#include "RBitmap.h"

RBitmap::RBitmap()
{
	this->m_ptr_data = nullptr;
	this->m_size_of_data = 0;
}
RBitmap::~RBitmap()
{
	this->Clear();
}

void RBitmap::Clear()
{
	if (nullptr != this->m_ptr_data)
		delete[] this->m_ptr_data;
	this->m_ptr_data = nullptr;
	this->m_size_of_data = 0;
}

BOOL RBitmap::Load(HBITMAP hBmp)
{
	this->Clear();
	return this->LoadHBitmapData(hBmp);
}

BOOL RBitmap::Load(wstring wpath)
{
	BOOL ok = FALSE;
	this->Clear();
	HANDLE hFile = ::CreateFileW(wpath.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (INVALID_HANDLE_VALUE != hFile) {
		this->m_size_of_data = ::GetFileSize(hFile, NULL);
		if (0 < this->m_size_of_data) {
			this->m_ptr_data = new BYTE[this->m_size_of_data];
			DWORD dwReaded = 0;
			if (::ReadFile(hFile,
				this->m_ptr_data, this->m_size_of_data, &dwReaded, NULL)) {
				if (dwReaded == this->m_size_of_data) {
					ok = TRUE;
				}
			}
		}
		::CloseHandle(hFile);
	}
	return ok;
}

BOOL RBitmap::Save(wstring wpath)
{
	if (nullptr == this->m_ptr_data || 0 == this->m_size_of_data) return FALSE;
	BOOL ok = FALSE;
	::DeleteFileW(wpath.c_str());
	HANDLE hFile = ::CreateFileW(wpath.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (INVALID_HANDLE_VALUE != hFile) {
		DWORD dwWritten = 0;
		if (::WriteFile(hFile, 
			this->m_ptr_data, this->m_size_of_data, &dwWritten, NULL)) {
			if (dwWritten == this->m_size_of_data) {
				ok = TRUE;
			}
		}
		::CloseHandle(hFile);
	}
	if (!ok) ::DeleteFileW(wpath.c_str());
	return ok;
}
#pragma warning(push)
#pragma warning(disable:6386)
#pragma warning(disable:6385)
DWORD RBitmap::ToGray(BYTE* ptr)
{
	DWORD size = 0;
	if (nullptr != this->m_ptr_data && 0 != this->m_size_of_data) {
		size = this->m_size_of_data;
		BYTE* ptrCopy = new BYTE[size];
		::memcpy(ptrCopy, this->m_ptr_data, size);
		BITMAPFILEHEADER bmpFileHeader;
		BITMAPINFOHEADER bmpInfoHeader;
		int offset = 0;
		::memcpy(&bmpFileHeader, ptrCopy + offset, sizeof(BITMAPFILEHEADER));
		offset += sizeof(BITMAPFILEHEADER);
		::memcpy(&bmpInfoHeader, ptrCopy + offset, sizeof(BITMAPINFOHEADER));
		offset += sizeof(BITMAPINFOHEADER);
		BYTE* ptrBmpData = ptrCopy + offset;

		bmpFileHeader.bfOffBits += (sizeof(RGBQUAD) * 256);
		bmpInfoHeader.biBitCount = 8;
		int lineBytes = (bmpInfoHeader.biWidth * 8 + 31) / 32 * 4;
		int oldLineBytes = (bmpInfoHeader.biWidth * 24 + 31) / 32 * 4;
		int oldSize = bmpInfoHeader.biSizeImage;
		bmpInfoHeader.biSizeImage = lineBytes * bmpInfoHeader.biHeight;

		RGBQUAD* pColorTable = new RGBQUAD[256];
		for (int i = 0; i < 256; i++)
		{
			(*(pColorTable + i)).rgbBlue = i;
			(*(pColorTable + i)).rgbGreen = i;
			(*(pColorTable + i)).rgbRed = i;
			(*(pColorTable + i)).rgbReserved = 0;
		}

		int red, green, blue;
		BYTE gray;
		BYTE* pGrayData = new BYTE[bmpInfoHeader.biSizeImage];
		::memset(pGrayData, 0, bmpInfoHeader.biSizeImage);
		for (int i = 0; i < bmpInfoHeader.biHeight; i++)
		{
			for (int j = 0; j < bmpInfoHeader.biWidth; j++)
			{
				red = *(ptrBmpData + i * oldLineBytes + 3 * j);
				green = *(ptrBmpData + i * oldLineBytes + 3 * j + 1);
				blue = *(ptrBmpData + i * oldLineBytes + 3 * j + 2);
				gray = (BYTE)((77 * red + 151 * green + 28 * blue) >> 8);
				*(pGrayData + i * lineBytes + j) = gray;
			}
		}

		size = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
		size += sizeof(RGBQUAD) * 256;
		size += sizeof(BYTE) * bmpInfoHeader.biSizeImage;
		if (nullptr != ptr) {
			DWORD off = 0;
			::memcpy(ptr + off, &bmpFileHeader, sizeof(BITMAPFILEHEADER));
			off += sizeof(BITMAPFILEHEADER);
			::memcpy(ptr + off, &bmpInfoHeader, sizeof(BITMAPINFOHEADER));
			off += sizeof(BITMAPINFOHEADER);
			::memcpy(ptr + off, pColorTable, sizeof(RGBQUAD) * 256);
			off += sizeof(RGBQUAD) * 256;
			::memcpy(ptr + off, pGrayData, sizeof(BYTE) * bmpInfoHeader.biSizeImage);
			off += sizeof(BYTE) * bmpInfoHeader.biSizeImage;
			size = off;
		}
		delete[] ptrCopy;
		delete[] pGrayData;
		delete[] pColorTable;
	}
	return size;
}

DWORD RBitmap::ToBinary(BYTE* ptr, BYTE b)
{
	DWORD size = this->ToGray();
	BYTE* ptrGray = new BYTE[size];
	if (nullptr != ptr && size == this->ToGray(ptrGray)) {
		BITMAPFILEHEADER bmpFileHeader;
		BITMAPINFOHEADER bmpInfoHeader;
		int offset = 0;
		::memcpy(&bmpFileHeader, ptrGray + offset, sizeof(BITMAPFILEHEADER));
		offset += sizeof(BITMAPFILEHEADER);
		::memcpy(&bmpInfoHeader, ptrGray + offset, sizeof(BITMAPINFOHEADER));
		offset += sizeof(BITMAPINFOHEADER);
		BYTE* ptrBmpData = ptrGray + offset;

		bmpFileHeader.bfOffBits += (sizeof(RGBQUAD) * 256);
		bmpInfoHeader.biBitCount = 1;
		int lineBytes = (bmpInfoHeader.biWidth * 8 + 31) / 32 * 4;
		int oldLineBytes = (bmpInfoHeader.biWidth * 24 + 31) / 32 * 4;
		int oldSize = bmpInfoHeader.biSizeImage;
		bmpInfoHeader.biSizeImage = lineBytes * bmpInfoHeader.biHeight;

		RGBQUAD* pColorTable = new RGBQUAD[256];
		for (int i = 0; i < 256; i++)
		{
			(*(pColorTable + i)).rgbBlue = i;
			(*(pColorTable + i)).rgbGreen = i;
			(*(pColorTable + i)).rgbRed = i;
			(*(pColorTable + i)).rgbReserved = 0;
		}
		DWORD off = 0;
		for (LONG i = 0; i < bmpInfoHeader.biHeight; i++)
		{
			LONG j;
			for (j = 0; j < (bmpInfoHeader.biWidth + 3) / 4 * 4; j++)
			{
				BYTE bTmp = ptrBmpData[off];
				ptrBmpData[off++] = (bTmp > b) ? 255 : 0;
			}
		}
		::memcpy(ptr, ptrGray, size);
		delete[] ptrGray;
		delete[] pColorTable;
	}
	return size;
}


BOOL RBitmap::SaveToGray(wstring wpath)
{
	BOOL ok = FALSE;
	DWORD size = this->ToGray();
	BYTE* ptr = new BYTE[size];
	if (size == this->ToGray(ptr)) {
		::DeleteFileW(wpath.c_str());
		HANDLE hFile = ::CreateFileW(wpath.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
		if (INVALID_HANDLE_VALUE != hFile) {
			DWORD dwWritten = 0;
			::WriteFile(hFile, ptr, size, &dwWritten, NULL);
			::CloseHandle(hFile);
			if (size == dwWritten) ok = TRUE;
		}
		if (!ok) ::DeleteFileW(wpath.c_str());
	}
	delete[] ptr;
	return ok;
}

BOOL RBitmap::SaveToBinary(wstring wpath, BYTE b)
{
	BOOL ok = FALSE;
	DWORD size = this->ToBinary();
	BYTE* ptr = new BYTE[size];
	if (size == this->ToBinary(ptr, b)) {
		::DeleteFileW(wpath.c_str());
		HANDLE hFile = ::CreateFileW(wpath.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
		if (INVALID_HANDLE_VALUE != hFile) {
			DWORD dwWritten = 0;
			::WriteFile(hFile, ptr, size, &dwWritten, NULL);
			::CloseHandle(hFile);
			if (size == dwWritten) ok = TRUE;
		}
		if (!ok) ::DeleteFileW(wpath.c_str());
	}
	delete[] ptr;
	return ok;
}

BOOL RBitmap::LoadHBitmapData(HBITMAP hBmp)
{
	BITMAP bmp;
	if (0 != ::GetObject(hBmp, sizeof(bmp), &bmp)) {
		HDC hDC = ::CreateDCA("DISPLAY", NULL, NULL, NULL);
		int iBits = ::GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES);
		::DeleteDC(hDC);

		WORD wBitCount = 24;
		if (iBits <= 1)
			wBitCount = 1;
		else if (iBits <= 4)
			wBitCount = 4;
		else if (iBits <= 8)
			wBitCount = 8;
		else if (iBits <= 24)
			wBitCount = 24;
		else if (iBits <= 32)
			wBitCount = 24;

		DWORD dwPaletteSize = 0;
		if (wBitCount <= 8)
			dwPaletteSize = (1 << wBitCount) * sizeof(RGBQUAD);

		BITMAPFILEHEADER   bmfHdr; 
		BITMAPINFOHEADER   bi;
		LPBITMAPINFOHEADER lpbi;

		bi.biSize = sizeof(BITMAPINFOHEADER);
		bi.biWidth = bmp.bmWidth;
		bi.biHeight = bmp.bmHeight;
		bi.biPlanes = 1;
		bi.biBitCount = wBitCount;
		bi.biCompression = BI_RGB;
		bi.biSizeImage = 0;
		bi.biXPelsPerMeter = 0;
		bi.biYPelsPerMeter = 0;
		bi.biClrUsed = 0;
		bi.biClrImportant = 0;
		DWORD dwBmBitsSize = ((bmp.bmWidth * wBitCount + 31) / 32) * 4 * bmp.bmHeight;

		HANDLE hDib = ::GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));
		if (hDib) {
			lpbi = (LPBITMAPINFOHEADER)::GlobalLock(hDib);
			if (lpbi) {
				*lpbi = bi;
				HANDLE hOldPal = NULL;
				HANDLE hPal = ::GetStockObject(DEFAULT_PALETTE);
				if (hPal) {
					hDC = ::GetDC(NULL);
					hOldPal = ::SelectPalette(hDC, (HPALETTE)hPal, FALSE);
					RealizePalette(hDC);
				}
				::GetDIBits(hDC, hBmp, 0, (UINT)bmp.bmHeight,
					(LPSTR)lpbi + sizeof(BITMAPINFOHEADER) + dwPaletteSize,
					(LPBITMAPINFO)lpbi, DIB_RGB_COLORS);
				// recover palette.
				if (hOldPal)
				{
					::SelectPalette(hDC, (HPALETTE)hOldPal, TRUE);
					::RealizePalette(hDC);
					::ReleaseDC(NULL, hDC);
				}

				bmfHdr.bfType = 0x4D42;
				DWORD dwDIBSize = sizeof(BITMAPFILEHEADER);
				dwDIBSize += sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize;
				bmfHdr.bfSize = dwDIBSize;
				bmfHdr.bfReserved1 = 0;
				bmfHdr.bfReserved2 = 0;
				bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER);
				bmfHdr.bfOffBits += (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize;
				this->m_size_of_data = dwDIBSize + sizeof(BITMAPFILEHEADER);
				this->m_ptr_data = new BYTE[this->m_size_of_data];
				size_t offset = 0;
				char* ptrHdr = reinterpret_cast<char*>(&bmfHdr);
				::memcpy(this->m_ptr_data + offset, ptrHdr, sizeof(BITMAPFILEHEADER));
				offset += sizeof(BITMAPFILEHEADER);
				::memcpy(this->m_ptr_data + offset, (LPSTR)lpbi, dwDIBSize);
				::GlobalUnlock(hDib);
				::GlobalFree(hDib);
				return TRUE;
			}
		}
	}
	return FALSE;
}
#pragma warning(pop)
