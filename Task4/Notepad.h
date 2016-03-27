#pragma once
#include <Windows.h>
#include "EditControl.h"
class CNotepadWindow {
public:
	CNotepadWindow();
	~CNotepadWindow();
	static bool RegisterClass();
	bool Create();
	void Show(int cmdShow);
protected:
	void OnCreate(HWND _handle);
	void OnDestroy();
	void OnSize();
	bool OnClose();
	

private:
	CEditControlWindow editControl;
	HWND handle;
	bool edited;
	static const LPCWSTR NotepadClassName;
	static LRESULT __stdcall windowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
	void saveText();

};