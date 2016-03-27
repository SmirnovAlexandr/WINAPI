#pragma once
#include <Windows.h>
class CEditControlWindow {
public:
	CEditControlWindow();
	~CEditControlWindow();
	bool Create(HWND parentHandle);
	HWND GetHandle();
protected:

private:
	HWND handle;
};