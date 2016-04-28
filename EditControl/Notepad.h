#pragma once
#include <Windows.h>
#include "EditControl.h"
#include "resource1.h"

class CNotepadWindow {
public:
	CNotepadWindow();
	~CNotepadWindow();
	static bool RegisterClass();
	bool Create();
	void Show(int cmdShow);
	HWND GetHandle();
	HWND GetEditControlHandle();
protected:
	
	void OnCreate(HWND _handle);
	void OnDestroy();
	void OnSize();
	bool OnClose();
	void OnTitle();
	HBRUSH OnDraw(WPARAM wParam);
	void OnTextEditControl();
	void ViewSettings();
	int newFontSize;
	int newAlpha;
	int oldFontSize;
	int oldAlpha;
	COLORREF newBackGroundColor;
	COLORREF oldBackGroundColor;
	COLORREF newFontColor;
	COLORREF oldFontColor;
	bool preView;
	void SetAllParameters(HWND windowHandle, HWND editHandle, int fontSize, int alpha, COLORREF fontColor, COLORREF backgroundColor);
	HMENU Menu;
private:
	CEditControlWindow editControl;
	HWND handle;
	bool edited;
	static const LPCWSTR NotepadClassName;
	static LRESULT __stdcall windowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
	static BOOL CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void saveText();
	void setFont(HWND targetHandle, int size);
	void setAlpha(HWND targetHandle, int alpha);
};