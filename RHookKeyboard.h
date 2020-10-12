/********************************************************************************
********************************* Hook keyboard *********************************
* Monitor keyboard key down and up.
********************************************************************************/
#pragma once
#include "Header.h"
class RHookKeyboard
{
public:
	RHookKeyboard();
	~RHookKeyboard();
	// Monitor keyboard click.
	void Running();
	// Check the key 'VK_CAPITAL' is lock.
	static BOOL IsCapsLk();
	// The key down or up record.
	static void KeyboardRecord(DWORD vCode, BOOL _isDown = TRUE);
protected:
	HHOOK m_keyboardHook = NULL;
};

