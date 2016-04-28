#include "Notepad.h"
#include <Commctrl.h>
#include <string>
#include <Windowsx.h>

const LPCWSTR CNotepadWindow::NotepadClassName = L"CNotepadWindow";



CNotepadWindow::CNotepadWindow()
{
	handle = 0;
	edited = false;
	editControl = CEditControlWindow();
	oldFontSize = newFontSize = 22;
	oldAlpha = newAlpha = 255;
	oldBackGroundColor = newBackGroundColor = RGB(255, 255, 255);
	oldFontColor = newFontColor = RGB(0, 0, 0);
	preView = false;
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
	window.hIcon = (HICON)LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON,
		0,0,  LR_DEFAULTCOLOR);
	window.hIconSm = (HICON)LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(IDI_ICON3), IMAGE_ICON,
		0, 0, LR_DEFAULTCOLOR);
	window.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
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
	setFont(editControl.GetHandle(), oldFontSize);
	Menu = ::GetMenu(handle);
	//::EnableMenuItem(Menu, ID_FILE_SAVE, MF_DISABLED | MF_GRAYED );
	//::EnableMenuItem(Menu, ID_VIEW_SETTINGS, MF_DISABLED | MF_GRAYED);

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
		DWORD bom = 0xFEFF;
		WriteFile(fileHandle, &bom, 2, &a, NULL);
		WriteFile(fileHandle, buffer, textLength * sizeof(wchar_t), &a, 0);
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

HWND CNotepadWindow::GetHandle()
{
	return handle;
}

HWND CNotepadWindow::GetEditControlHandle() {
	return editControl.GetHandle();
}

void CNotepadWindow::OnTitle()
{
	wchar_t title[256];
	::LoadString(GetModuleHandle(0), ID_TITLE, title, sizeof(title));
	SetWindowText(handle, title);
}

void CNotepadWindow::OnTextEditControl()
{
	HRSRC hrsrc = ::FindResource(GetModuleHandle(0), MAKEINTRESOURCE(IDR_INFO1), L"info");
	HGLOBAL hglobal = ::LoadResource(GetModuleHandle(0), hrsrc);
	wchar_t *dataPointer = (wchar_t*)::LockResource(hglobal);
	int sizeOfRes = ::SizeofResource(GetModuleHandle(0), hrsrc);
	SetWindowText(editControl.GetHandle(), dataPointer);
}

void CNotepadWindow::setFont(HWND targetHandle, int fontSize)
{
	HFONT tempFont = (HFONT)SendMessage(targetHandle, WM_GETTEXT, 0, 0);
	if (!tempFont) {
		tempFont = (HFONT)::GetStockObject(DEFAULT_GUI_FONT);
	}
	LOGFONT tempLogFont;
	::GetObject(tempFont, sizeof(LOGFONT), &tempLogFont);
	tempLogFont.lfHeight = fontSize;
	SendMessage(targetHandle, WM_SETFONT,
		(WPARAM)CreateFontIndirect(&tempLogFont), 1);
}

void CNotepadWindow::setAlpha(HWND targetHandle, int alpha) 
{
	SetWindowLong(targetHandle,	GWL_EXSTYLE, GetWindowLong(targetHandle, GWL_EXSTYLE) | WS_EX_LAYERED);
	SetLayeredWindowAttributes(targetHandle, 0, alpha, LWA_ALPHA);
}

HBRUSH CNotepadWindow::OnDraw(WPARAM wParam)
{
	SetTextColor((HDC)wParam, newFontColor);
	SetBkColor((HDC)wParam, newBackGroundColor);
	return (HBRUSH)oldBackGroundColor;
}

void CNotepadWindow::SetAllParameters(HWND windowHandle, HWND editControlHandle,
	int fontSize, int alpha, COLORREF fontColor, COLORREF backgroundColor)
{
	setFont(editControlHandle, fontSize);
	setAlpha(windowHandle, alpha);
	::InvalidateRect(editControlHandle, NULL, true);
}

BOOL CALLBACK CNotepadWindow::DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CNotepadWindow* parentWindow = (CNotepadWindow*)GetWindowLongPtr(::GetParent(hwndDlg), GWLP_USERDATA);
	switch (uMsg){
	case WM_INITDIALOG:
	{
		SendDlgItemMessage(hwndDlg, IDC_FONT_SLIDER, TBM_SETRANGEMIN, true, 8);
		SendDlgItemMessage(hwndDlg, IDC_FONT_SLIDER, TBM_SETRANGEMAX, true, 72);
		SendDlgItemMessage(hwndDlg, IDC_FONT_SLIDER, TBM_SETPOS, true, parentWindow->oldFontSize);
		SendDlgItemMessage(hwndDlg, IDC_OPACITY_SLIDER, TBM_SETRANGEMIN, true, 0);
		SendDlgItemMessage(hwndDlg, IDC_OPACITY_SLIDER, TBM_SETRANGEMAX, true, 255);
		SendDlgItemMessage(hwndDlg, IDC_OPACITY_SLIDER, TBM_SETPOS, true, parentWindow->oldAlpha);

		return true;
	}
	case WM_COMMAND:
	{
		switch (LOWORD(wParam)) {
		case IDC_BUTTON_OK:
		{
			//GET
			int fontSize = parentWindow -> newFontSize = SendDlgItemMessage(hwndDlg, IDC_FONT_SLIDER, TBM_GETPOS, 0, 0);
			int alpha = parentWindow -> newAlpha = SendDlgItemMessage(hwndDlg, IDC_OPACITY_SLIDER, TBM_GETPOS, 0, 0);
			//SET
			parentWindow->SetAllParameters(parentWindow->GetHandle(), parentWindow->GetEditControlHandle(),
				fontSize, alpha, NULL, NULL);
			//CHANGE
			parentWindow->oldFontSize = fontSize;
			parentWindow->oldAlpha = alpha;
			EndDialog(hwndDlg, 0);
			return true;
		}
		case IDC_BUTTON_CANCEL:
		{
			int fontSize = parentWindow->oldFontSize;
			int alpha = parentWindow->oldAlpha;
			parentWindow->SetAllParameters(parentWindow->GetHandle(), parentWindow->GetEditControlHandle(),
				fontSize, alpha, NULL, NULL);
			//CHANGE
			parentWindow->newFontSize = fontSize;
			parentWindow->newAlpha = alpha;

			parentWindow->newFontColor = parentWindow->oldFontColor;
			parentWindow->newBackGroundColor = parentWindow->oldBackGroundColor;
			::InvalidateRect(parentWindow->GetEditControlHandle(), NULL, 1);
			//just exit without saving
			EndDialog(hwndDlg, 0);
			return true;
		}
		case IDC_CHECK_VIEW:
		{
			LRESULT state = Button_GetCheck(GetDlgItem(hwndDlg, IDC_CHECK_VIEW));
			if (state == BST_CHECKED) {
				parentWindow->preView = true;
				::InvalidateRect(parentWindow->GetEditControlHandle(), NULL, true);
			} else if (state == BST_UNCHECKED) {
				parentWindow->preView = false;
			}
			return true;
		}
		case IDC_COLOR_FAMILY:
		{
			//prepare for battle
			COLORREF oldcolor = parentWindow->oldFontColor;
			COLORREF newcolor;
			CHOOSECOLOR colstr = {};
			colstr.lStructSize = sizeof(CHOOSECOLOR);
			colstr.hwndOwner = parentWindow->GetHandle();
			colstr.lpCustColors = &oldcolor;
			colstr.rgbResult = oldcolor;
			colstr.Flags = CC_FULLOPEN | CC_RGBINIT;
			if (::ChooseColor(&colstr)) {
				parentWindow->newFontColor = colstr.rgbResult;
			}
			if (parentWindow->preView) {
				::InvalidateRect(parentWindow->GetEditControlHandle(), NULL, 1);
			}
			return true;

		}
		case IDC_COLOR_BACKGROUND:
		{
			//prepare for battle 2
			COLORREF oldcolor = parentWindow->oldBackGroundColor;
			COLORREF newcolor;
			CHOOSECOLOR colstr = {};
			colstr.lStructSize = sizeof(CHOOSECOLOR);
			colstr.hwndOwner = parentWindow->GetHandle();
			colstr.lpCustColors = &oldcolor;
			colstr.rgbResult = oldcolor;
			colstr.Flags = CC_FULLOPEN | CC_RGBINIT;
			if (::ChooseColor(&colstr)) {
				parentWindow->newBackGroundColor = colstr.rgbResult;
			}
			if (parentWindow->preView) {
				::InvalidateRect(parentWindow->GetEditControlHandle(), NULL, 1);
			}
			return true;
		}
		default:
			return true;
		
		}
	}
	case WM_HSCROLL:
	{
		int sliderId = GetDlgCtrlID((HWND)lParam);
		switch (sliderId) {
		case IDC_FONT_SLIDER:
		{
			if (parentWindow->preView) {
				int fontSize = parentWindow->newFontColor = SendDlgItemMessage(hwndDlg, IDC_FONT_SLIDER, TBM_GETPOS, 0, 0);
				parentWindow->setFont(parentWindow->GetEditControlHandle(), fontSize);
			}
		}
		case IDC_OPACITY_SLIDER:
		{
			if (parentWindow->preView) {
				int alpha = parentWindow->newAlpha = SendDlgItemMessage(hwndDlg, IDC_OPACITY_SLIDER, TBM_GETPOS, 0, 0);
				parentWindow->setAlpha(parentWindow->GetHandle(), alpha);
			}
		}
		}

		/*int pos = SendDlgItemMessage(hwndDlg, sliderId, TBM_GETPOS, 0, 0);
		std::string temp = std::to_string(pos);
		::MessageBoxA(hwndDlg, temp.c_str(), "position", NULL);*/
		return true;
	}
	case WM_CLOSE:
	{
		EndDialog(hwndDlg, 0);
		return true;
	}
	}
	return false;
}

void CNotepadWindow::ViewSettings() 
{
	INT_PTR intptr = 
		DialogBox(GetModuleHandle(0), MAKEINTRESOURCE(IDD_DIALOG1), handle, CNotepadWindow::DialogProc);
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
		case WM_CREATE:
		{
			window->OnTitle();
			window->OnTextEditControl();
			return DefWindowProc(handle, message, wParam, lParam);
		}
		case WM_SIZE:
		{
			window->OnSize();
			return DefWindowProc(handle, message, wParam, lParam);
		}
		case WM_COMMAND:
		{
			if (HIWORD(wParam) == EN_CHANGE) {
				window->edited = true;
			} else if (LOWORD(wParam) == ID_FILE_SAVE) {
				window -> saveText();
			} else if (LOWORD(wParam) == ID_FILE_EXIT) {
				SendMessage(handle, WM_CLOSE, 0, 0);
			} else if (LOWORD(wParam) == ID_EXIT_ACCELERATOR) {
				SendMessage(handle, WM_DESTROY, 0, 0);
			} else if (LOWORD(wParam) == ID_VIEW_SETTINGS) {
				window->ViewSettings();
				
				DrawMenuBar(handle)
;			}	

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
		case WM_CTLCOLOREDIT:
		{
			return (LRESULT)window->OnDraw(wParam);
		}
		default:
			return DefWindowProc(handle, message, wParam, lParam);
		}
	}
}