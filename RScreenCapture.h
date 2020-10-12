/********************************************************************************
******************************** Screen Capture *********************************
* Screen capture to get HBITMAP.
* Get 8,16,24bitmap.
********************************************************************************/
#pragma once
#include"Header.h"
class RScreenCapture
{
public:
	RScreenCapture();
	~RScreenCapture();
	HBITMAP GetHBitmap8();
	HBITMAP GetHBitmap16();
	HBITMAP GetHBitmap24();

protected:
	HBITMAP Get(WORD bitCount = 24);
};

