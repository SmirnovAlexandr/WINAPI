#include "EditControl.h"
CEditControlWindow::CEditControlWindow()
{
	handle = 0;
}

CEditControlWindow::~CEditControlWindow() 
{
}

bool CEditControlWindow::Create(HWND parentHandle)
{
	handle = CreateWindowEx(
		0,
		L"EDIT",
		0,
		WS_CHILD | WS_VISIBLE | WS_VSCROLL |
		ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		parentHandle,
		0,
		GetModuleHandle(0),
		0);
	SetFocus(handle);
	return (handle != 0);
}

HWND CEditControlWindow::GetHandle()
{
	return handle;
}
