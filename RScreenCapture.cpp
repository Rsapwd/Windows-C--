#include "RScreenCapture.h"

RScreenCapture::RScreenCapture()
{

}
RScreenCapture::~RScreenCapture()
{

}

HBITMAP RScreenCapture::GetHBitmap8()
{
    return this->Get(8);
}
HBITMAP RScreenCapture::GetHBitmap16()
{
    return this->Get(16);
}
HBITMAP RScreenCapture::GetHBitmap24()
{
    return this->Get(24);
}

HBITMAP RScreenCapture::Get(WORD bitCount)
{
    BITMAPINFO bi;
    ::memset(&bi, 0, sizeof(bi));
    bi.bmiHeader.biSize = sizeof(BITMAPINFO);
    bi.bmiHeader.biWidth = ::GetSystemMetrics(SM_CXSCREEN);
    bi.bmiHeader.biHeight = ::GetSystemMetrics(SM_CYSCREEN);
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = bitCount;

    BYTE* Data;
    HDC hDC = ::GetDC(NULL);
    HDC MemDC = ::CreateCompatibleDC(hDC);
    HBITMAP hBmp = ::CreateDIBSection(MemDC, &bi, 
        DIB_RGB_COLORS, (void**)&Data, NULL, 0);
    if (hBmp) {
        ::SelectObject(MemDC, hBmp);
        ::BitBlt(MemDC, 0, 0, bi.bmiHeader.biWidth, bi.bmiHeader.biHeight, hDC, 0, 0, SRCCOPY);
    }
    ::ReleaseDC(NULL, hDC);
    ::DeleteDC(MemDC);
    return hBmp;
}