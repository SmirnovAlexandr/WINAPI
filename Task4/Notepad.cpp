#include "Notepad.h"

const LPCWSTR CNotepadWindow::NotepadClassName = L"CNotepadWindow";

CNotepadWindow::CNotepadWindow()
{
	handle = 0;
	edited = false;
	editControl = CEditControlWindow();
}

CNotepadWindow::~CNotepadWindow()
{
}

bool CNotepadWindow::RegisterClass() {
	WNDCLASSEX window = {};
	window.cbSize = sizeof(WNDCLASSEX);
	window.lpfnWndProc = CNotepadWindow::windowProc;
	window.hInstance = GetModuleHandle(0);
	window.lpszClassName = NotepadClassName;
	return (::RegisterClassEx(&window) != 0);
}

bool CNotepadWindow::Create() {
	CreateWindowEx(
		0,
		NotepadClassName,
		L"Notepad",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		0,
		0,
		GetModuleHandle(0),
		this);
	return (handle != 0);

}

void CNotepadWindow::Show(int cmdShow)
{
	ShowWindow(handle, cmdShow);
}

void CNotepadWindow::OnCreate(HWND _handle) 
{
	handle = _handle;
	editControl.Create(handle);
}

void CNotepadWindow::OnDestroy()
{
	PostQuitMessage(0);
}

void CNotepadWindow::OnSize()
{
	RECT rect;
	::GetClientRect(handle, &rect);
	SetWindowPos(editControl.GetHandle(), 0, 0, 0, rect.right - rect.left, rect.bottom - rect.top, 0);
}

void CNotepadWindow::saveText()
{
	LRESULT textLength = SendMessage(editControl.GetHandle(), WM_GETTEXTLENGTH, 0, 0);
	textLength++;
	wchar_t *buffer = new wchar_t[textLength];
	SendMessage(editControl.GetHandle(), WM_GETTEXT, textLength, LPARAM(buffer));
	OPENFILENAME ofn = {};
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = handle;
	wchar_t filename[1000];
	filename[0] = '\0';
	ofn.lpstrFile = filename;
	ofn.nMaxFile = sizeof(filename);
	
	if (GetSaveFileName(&ofn)) {
		HANDLE fileHandle = CreateFile(filename, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		DWORD a;
		WriteFile(fileHandle, buffer, textLength, &a, 0);
		CloseHandle(fileHandle);
	} 	
}

bool CNotepadWindow::OnClose()
{
	LPCTSTR closeMessage = L"Do you want to save this text?";
	LPCTSTR title = L"Notepad";
	bool answer = true;
	if (edited) {
		int messageboxResult = ::MessageBox(handle, closeMessage, title, MB_YESNOCANCEL);
		switch (messageboxResult) {
		case IDYES:
		{
			saveText();
			answer = true;
			break;
		}
		case IDNO:
		{
			answer = true;
			break;
		}
		case IDCANCEL:
		{
			answer = false;
			break;
		}
		}
	}
	return answer;
}
LRESULT CNotepadWindow::windowProc(
	HWND handle,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
{
	if (message == WM_NCCREATE) {
		CNotepadWindow* window = (CNotepadWindow*)((CREATESTRUCT*)lParam)->lpCreateParams;
		SetLastError(0);
		SetWindowLongPtr(handle, GWLP_USERDATA, (LONG)window);
		if (GetLastError() != 0) {
			return GetLastError();
		}
		window->OnCreate(handle);
		return DefWindowProc(handle, message, wParam, lParam);
	}
	else {
		CNotepadWindow* window = (CNotepadWindow*)GetWindowLongPtr(handle, GWLP_USERDATA);
		switch (message) {
		case WM_SIZE:
		{
			window->OnSize();
			return DefWindowProc(handle, message, wParam, lParam);
		}
		case WM_COMMAND:
		{
			if (HIWORD(wParam) == EN_CHANGE) {
				window->edited = true;
			}
			return DefWindowProc(handle, message, wParam, lParam);
		}
		case WM_CLOSE:
		{
			if (window->OnClose()) {
				return  DefWindowProc(handle, message, wParam, lParam);
			} else {
				return 0;
			}

		}
		case WM_DESTROY:
		{
			window->OnDestroy();
			return 0;
		}
		default:
			return DefWindowProc(handle, message, wParam, lParam);
		}
	}
}