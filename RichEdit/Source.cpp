#include <windows.h>
#include <windowsx.h>
#include <string>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <tchar.h>
#include <Richedit.h>
#include <queue>
#include <CommCtrl.h>
#include "resource.h"
#pragma comment(lib, "comctl32.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
using namespace std;

static TCHAR szWindowClass[] = _T("win32app");
static TCHAR szTitle[] = _T("RichEdit");

HINSTANCE hInst;
HWND hWnd;
HWND hWndEdit;
HWND hFontStyleToolbar;
HWND hColorToolbar;
int ColorMode = 0; //1: Text color 2:Text highlight 
COLORREF Color = RGB(255, 255, 255);
static HWND hWndComboBoxFont, hWndComboBoxSize, hWndComboBoxAllign, hWndFind, hWndReplace, hWndFindNextButton, hWndReplaceButton, hWndReplaceAllButton;
const wchar_t *FontList[] = { _T("Times New Roman"), _T("Tahoma"), _T("Arial") };
const wchar_t *AllignList[] = { _T("Left"), _T("Right"), _T("Center") };
bool Bold = false, Italic = false, Underline = false, Strikeout = false;
LONG StyleOffset = 0;
LONG ibegin = 0, iend = -1;
BOOL found = false;
const INT iFontStyleButton = 4;
TBBUTTON FontStyleButtons[iFontStyleButton] =
{
	{ 0, ID_STYLE_BOLD, TBSTATE_ENABLED, BTNS_CHECK | BTNS_AUTOSIZE, { 0 }, 0, 0 },
	{ 1, ID_STYLE_ITALIC, TBSTATE_ENABLED, BTNS_CHECK | BTNS_AUTOSIZE, { 0 }, 0, 0 },
	{ 2, ID_STYLE_UNDERLINE, TBSTATE_ENABLED, BTNS_CHECK | BTNS_AUTOSIZE, { 0 }, 0, 0 },
	{ 3, ID_STYLE_STRIKETHROUGH, TBSTATE_ENABLED, BTNS_CHECK | BTNS_AUTOSIZE, { 0 }, 0, 0 },
};
const INT iColorButton = 15;
TBBUTTON ColorButtons[iColorButton] =
{
	{ 0, ID_BLACK, TBSTATE_ENABLED | TBSTATE_WRAP, BTNS_BUTTON | BTNS_AUTOSIZE, { 0 }, 0L, 0 },
	{ 1, ID_BROWN, TBSTATE_ENABLED | TBSTATE_WRAP, BTNS_BUTTON | BTNS_AUTOSIZE, { 0 }, 0L, 0 },
	{ 2, ID_DGREEN, TBSTATE_ENABLED | TBSTATE_WRAP, BTNS_BUTTON | BTNS_AUTOSIZE, { 0 }, 0L, 0 },
	{ 3, ID_DYELLOW, TBSTATE_ENABLED | TBSTATE_WRAP, BTNS_BUTTON | BTNS_AUTOSIZE, { 0 }, 0L, 0 },
	{ 4, ID_DBLUE, TBSTATE_ENABLED | TBSTATE_WRAP, BTNS_BUTTON | BTNS_AUTOSIZE, { 0 }, 0L, 0 },
	{ 5, ID_PURPLE, TBSTATE_ENABLED | TBSTATE_WRAP, BTNS_BUTTON | BTNS_AUTOSIZE, { 0 }, 0L, 0 },
	{ 6, ID_TEAL, TBSTATE_ENABLED | TBSTATE_WRAP, BTNS_BUTTON | BTNS_AUTOSIZE, { 0 }, 0L, 0 },
	{ 7, ID_GRAY, TBSTATE_ENABLED | TBSTATE_WRAP, BTNS_BUTTON | BTNS_AUTOSIZE, { 0 }, 0L, 0 },
	{ 8, ID_RED, TBSTATE_ENABLED | TBSTATE_WRAP, BTNS_BUTTON | BTNS_AUTOSIZE, { 0 }, 0L, 0 },
	{ 9, ID_GREEN, TBSTATE_ENABLED | TBSTATE_WRAP, BTNS_BUTTON | BTNS_AUTOSIZE, { 0 }, 0L, 0 },
	{ 10, ID_YELLOW, TBSTATE_ENABLED | TBSTATE_WRAP, BTNS_BUTTON | BTNS_AUTOSIZE, { 0 }, 0L, 0 },
	{ 11, ID_BLUE, TBSTATE_ENABLED | TBSTATE_WRAP, BTNS_BUTTON | BTNS_AUTOSIZE, { 0 }, 0L, 0 },
	{ 12, ID_PINK, TBSTATE_ENABLED | TBSTATE_WRAP, BTNS_BUTTON | BTNS_AUTOSIZE, { 0 }, 0L, 0 },
	{ 13, ID_MINT, TBSTATE_ENABLED | TBSTATE_WRAP, BTNS_BUTTON | BTNS_AUTOSIZE, { 0 }, 0L, 0 },
	{ 14, ID_WHITE, TBSTATE_ENABLED | TBSTATE_WRAP, BTNS_BUTTON | BTNS_AUTOSIZE, { 0 }, 0L, 0 },
};


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void OpenDialog(HWND);
void LoadFile(LPSTR);
void SaveDialog(HWND);
void SaveFile(LPSTR);
void Format(HWND hWnd, int format);
BOOL FindNext(HWND hRich, LONG sbegin, LONG send);
BOOL Replace();
void ReplaceAll();

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDR_MENU2);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL,
			_T("Call to RegisterClassEx failed!"),
			_T("Win32 Guided Tour"),
			NULL);

		return 1;
	}
	hInst = hInstance;
	hWnd = CreateWindow(
		szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
		CW_USEDEFAULT, CW_USEDEFAULT,
		800, 600,
		NULL,
		NULL,
		hInstance,
		NULL
		);

	if (!hWnd)
	{
		MessageBox(NULL,
			_T("Call to CreateWindow failed!"),
			_T("Win32 Guided Tour"),
			NULL);

		return 1;
	}

	ShowWindow(hWnd,
		nCmdShow);
	UpdateWindow(hWnd);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	RECT rect;
	TBADDBITMAP FontStyleBmp = { (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), IDR_TOOLBAR1 };
	TBADDBITMAP ColorBmp = { (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), IDR_TOOLBAR2 };
	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		MoveToEx(hdc, 323, 3, NULL);
		LineTo(hdc, 323, 70);
		EndPaint(hWnd, &ps);
		break;
	case WM_CREATE:
		//add color mode radio button
		CreateWindowW(L"button", L"Text color",
			WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
			115, 52, 82, 16, hWnd, (HMENU)ID_TEXT_COLOR, hInst, NULL);
		CreateWindowW(L"button", L"Text highlight",
			WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
			200, 52, 110, 16, hWnd, (HMENU)ID_TEXT_HIGHLIGHT, hInst, NULL);

		//add font style toolbar
		InitCommonControls();
		GetWindowRect(hWnd, &rect);
		hFontStyleToolbar = CreateWindowEx(0,
			TOOLBARCLASSNAME,
			NULL,
			WS_CHILD | TBSTYLE_WRAPABLE | TBSTYLE_LIST | CCS_NOPARENTALIGN,
			3, 43, 317, 16,
			hWnd,
			(HMENU)IDR_TOOLBAR1,
			hInst,
			NULL);
		SendMessage(hFontStyleToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
		SendMessage(hFontStyleToolbar, TB_ADDBITMAP, (WPARAM)2, (LPARAM)&FontStyleBmp);
		SendMessage(hFontStyleToolbar, TB_ADDBUTTONS, iFontStyleButton, (LPARAM)&FontStyleButtons);
		ShowWindow(hFontStyleToolbar, SW_SHOW);

		//add font color toolbar
		hColorToolbar = CreateWindowEx(0,
			TOOLBARCLASSNAME,
			NULL,
			WS_CHILD | TBSTYLE_WRAPABLE | TBSTYLE_LIST | CCS_NOPARENTALIGN | CCS_VERT,
			-15, 73, 16, rect.bottom - rect.top,
			hWnd,
			(HMENU)IDR_TOOLBAR1,
			hInst,
			NULL);
		SendMessage(hColorToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
		SendMessage(hColorToolbar, TB_ADDBITMAP, (WPARAM)2, (LPARAM)&ColorBmp);
		SendMessage(hColorToolbar, TB_ADDBUTTONS, iColorButton, (LPARAM)&ColorButtons);
		ShowWindow(hColorToolbar, SW_SHOW);

		//add rich edit
		GetWindowRect(hWnd, &rect);
		LoadLibrary(TEXT("Msftedit.dll"));
		hWndEdit = CreateWindowEx(0, L"RICHEDIT50W", TEXT(""),
			ES_AUTOVSCROLL | ES_MULTILINE | WS_VISIBLE | WS_CHILD | WS_BORDER | WS_TABSTOP | WS_VSCROLL,
			0, 0, 0, 0,
			hWnd, NULL, hInst, NULL);
		Format(hWndEdit, -5);
		Format(hWndEdit, ID_COMBO_FONT);
		Format(hWndEdit, ID_COMBO_SIZE);

		//add combobox font
		CreateWindow(L"static", L"Font",
			WS_CHILD | WS_VISIBLE,
			45, 3, 90, 25, hWnd, NULL, hInst, NULL);
		hWndComboBoxFont = CreateWindow(L"combobox", NULL,
			WS_CHILD | WS_VISIBLE | CBS_DROPDOWN | WS_VSCROLL,
			3, 20, 120, 110, hWnd, (HMENU)ID_COMBO_FONT, hInst, NULL);
		for (int i = 0; i < 3; i++)
			SendMessage(hWndComboBoxFont, CB_ADDSTRING, 0, (LPARAM)FontList[i]);
		SendMessage(hWndComboBoxFont, CB_SETCURSEL, 0, 0);

		//add combobox aliign
		CreateWindow(L"static", L"Allignment",
			WS_CHILD | WS_VISIBLE,
			180, 3, 90, 25, hWnd, NULL, hInst, NULL);
		hWndComboBoxAllign = CreateWindow(L"combobox", NULL,
			WS_CHILD | WS_VISIBLE | CBS_DROPDOWN | WS_VSCROLL,
			175, 20, 80, 110, hWnd, (HMENU)ID_COMBO_ALLIGN, hInst, NULL);
		for (int i = 0; i < 3; i++)
			SendMessage(hWndComboBoxAllign, CB_ADDSTRING, 0, (LPARAM)AllignList[i]);
		SendMessage(hWndComboBoxAllign, CB_SETCURSEL, 0, 0);

		//add combobox size
		CreateWindow(L"static", L"Size",
			WS_CHILD | WS_VISIBLE,
			130, 3, 40, 25, hWnd, NULL, hInst, NULL);
		hWndComboBoxSize = CreateWindow(L"combobox", NULL,
			WS_CHILD | WS_VISIBLE | CBS_DROPDOWN | WS_VSCROLL,
			125, 20, 45, 110, hWnd, (HMENU)ID_COMBO_SIZE, hInst, NULL);
		TCHAR Size[20];
		for (int Count = 12; Count < 125; Count++)
		{
			_stprintf_s(Size, _T("%d"), Count);
			SendMessage(hWndComboBoxSize, CB_INSERTSTRING, -1, (LPARAM)Size);
		}
		SendMessage(hWndComboBoxSize, CB_SETCURSEL, 0, 0);

		//add find - replace control
		CreateWindow(L"static", L"Find what:",
			WS_CHILD | WS_VISIBLE,
			330, 3, 90, 16, hWnd, NULL, hInst, NULL);
		CreateWindow(L"static", L"Replace with:",
			WS_CHILD | WS_VISIBLE,
			330, 25, 90, 16, hWnd, NULL, hInst, NULL);
		hWndFind = CreateWindowW(L"Edit", NULL,
			WS_CHILD | WS_VISIBLE | WS_BORDER,
			420, 2, 150, 20, hWnd, (HMENU)ID_FIND,
			NULL, NULL);
		hWndReplace = CreateWindowW(L"Edit", NULL,
			WS_CHILD | WS_VISIBLE | WS_BORDER,
			420, 25, 150, 20, hWnd, (HMENU)ID_REPLACE,
			NULL, NULL);
		hWndFindNextButton = CreateWindowW(L"button", L"Find Next",
			WS_VISIBLE | WS_CHILD, 330, 50, 80, 20,
			hWnd, (HMENU)ID_BUTTON_FINDNEXT, NULL, NULL);
		hWndReplaceButton = CreateWindowW(L"button", L"Replace",
			WS_VISIBLE | WS_CHILD, 410, 50, 80, 20,
			hWnd, (HMENU)ID_BUTTON_REPLACE, NULL, NULL);
		hWndReplaceAllButton = CreateWindowW(L"button", L"Replace All",
			WS_VISIBLE | WS_CHILD, 490, 50, 80, 20,
			hWnd, (HMENU)ID_BUTTON_REPLACEALL, NULL, NULL);
		break;

	case WM_SETFOCUS:
		SetFocus(hWndEdit);
		break;

	case WM_SIZE:
		GetWindowRect(hWnd, &rect);
		MoveWindow(hFontStyleToolbar, 3, 43, 317, 16, TRUE);
		MoveWindow(hWndEdit, 28, 72, (rect.right - rect.left - 48), (rect.bottom - rect.top - 135), TRUE);
		break;
	
	case WM_COMMAND:
		if (HIWORD(wParam) == CBN_SELCHANGE)
		{
			Format(hWndEdit, LOWORD(wParam));
			SetFocus(hWndEdit);
		}
		else switch (wParam)
		{
		case ID_FILE_NEW:
			SendMessage(hWndEdit, EM_SETSEL, 0, -1);
			SendMessage(hWndEdit, EM_REPLACESEL, 0, NULL);
			break;
		case ID_FILE_OPEN:
				OpenDialog(hWnd);
				SetFocus(hWndEdit);
				break;
		case ID_FILE_SAVEAS:
				SaveDialog(hWnd);
				SetFocus(hWndEdit);
				break;
		case ID_FILE_EXIT:
			PostQuitMessage(0);
			SetFocus(hWndEdit);
			return 0;
		case ID_EDIT_FIND:
			break;
		case ID_STYLE_BOLD:
			Bold = !Bold;
			Format(hWndEdit, -5);
			SetFocus(hWndEdit);
			break;
		case ID_STYLE_ITALIC:
			Italic = !Italic;
			Format(hWndEdit, -5);
			SetFocus(hWndEdit);
			break;
		case ID_STYLE_UNDERLINE:
			Underline = !Underline;
			Format(hWndEdit, -5);
			SetFocus(hWndEdit);
			break;
		case ID_STYLE_STRIKETHROUGH:
			Strikeout = !Strikeout;
			Format(hWndEdit, -5);
			break;
		case ID_TEXT_COLOR:
			ColorMode = 1;
			SetFocus(hWndEdit);
			break;
		case ID_TEXT_HIGHLIGHT:
			ColorMode = 2;
			SetFocus(hWndEdit);
			break;
		case ID_BUTTON_FINDNEXT:
			FindNext(hWndEdit, ibegin, iend);
			break;
		case ID_BUTTON_REPLACE:
			Replace();
			break;
		case ID_BUTTON_REPLACEALL:
			ReplaceAll();
			break;
		case ID_BLACK:
			Color = RGB(0, 0, 0);
			Format(hWndEdit, -4);
			break;
		case ID_BROWN:			
			Color = RGB(136, 0, 21);
			Format(hWndEdit, -4);
			break;
		case ID_DGREEN:			
			Color = RGB(37, 177, 28);
			Format(hWndEdit, -4);
			break;
		case ID_DYELLOW:			
			Color = RGB(140, 140, 0);
			Format(hWndEdit, -4);
			break;
		case ID_DBLUE:			
			Color = RGB(28, 33, 104);
			Format(hWndEdit, -4);
			break;
		case ID_PURPLE:			
			Color = RGB(170, 0, 170);
			Format(hWndEdit, -4);
			break;
		case ID_TEAL:			
			Color = RGB(0, 126, 126);
			Format(hWndEdit, -4);
			break;
		case ID_GRAY:			
			Color = RGB(192, 192, 192);
			Format(hWndEdit, -4);
			break;
		case ID_RED:			
			Color = RGB(255, 0, 0);
			Format(hWndEdit, -4);
			break;
		case ID_GREEN:			
			Color = RGB(0, 255, 0);
			Format(hWndEdit, -4);
			break;
		case ID_YELLOW:			
			Color = RGB(255, 255, 0);
			Format(hWndEdit, -4);
			break;
		case ID_BLUE:			
			Color = RGB(0, 0, 255);
			Format(hWndEdit, -4);
			break;
		case ID_PINK:			
			Color = RGB(255, 0, 255);
			Format(hWndEdit, -4);
			break;
		case ID_MINT:			
			Color = RGB(0, 255, 255);
			Format(hWndEdit, -4);
			break;
		case ID_WHITE:			
			Color = RGB(255, 255, 255);
			Format(hWndEdit, -4);
			break;
		}
		break;
		
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
	return 0;
}

void OpenDialog(HWND hwnd) {

	OPENFILENAME ofn;
	TCHAR szFile[MAX_PATH];

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.hwndOwner = hwnd;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = TEXT("Rich Text Format(*.rtf)\0*.rtf\0");
	ofn.nFilterIndex = 1;
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrFileTitle = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetOpenFileName(&ofn))
		LoadFile((LPSTR)ofn.lpstrFile);
}

void SaveDialog(HWND hwnd) {

	OPENFILENAME ofn;
	TCHAR szFile[MAX_PATH];

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.hwndOwner = hwnd;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = TEXT("Rich Text Format(*.rtf)\0*.rtf\0");
	ofn.nFilterIndex = 1;
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrFileTitle = NULL;
	ofn.lpstrDefExt = _T("dat");
	ofn.nMaxFile = _MAX_PATH;
	ofn.lpstrTitle = _T("Save File");
	ofn.Flags = OFN_OVERWRITEPROMPT;

	if (GetSaveFileName(&ofn))
		SaveFile((LPSTR)ofn.lpstrFile);
}

DWORD CALLBACK EditStreamOutCallback(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
	HANDLE hFile = (HANDLE)dwCookie;
	DWORD NumberOfBytesWritten;
	if (!WriteFile(hFile, pbBuff, cb, &NumberOfBytesWritten, NULL))
	{
		return 1;
	}
	*pcb = NumberOfBytesWritten;

	return 0;
}

DWORD CALLBACK EditStreamInCallback(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
	HANDLE hFile = (HANDLE)dwCookie;
	DWORD NumberOfBytesRead;
	if (!ReadFile(hFile, pbBuff, cb, &NumberOfBytesRead, NULL))
	{
		return 1;
	}
	*pcb = NumberOfBytesRead;

	return 0;
}

void LoadFile(LPSTR file) 
{
	HANDLE hFile;
	hFile = CreateFile((LPCWSTR)file, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	EDITSTREAM es = { 0 };
	es.dwCookie = (DWORD_PTR)hFile;
	es.pfnCallback = &EditStreamInCallback;
	SendMessage(hWndEdit, EM_STREAMIN, SF_RTF, (LPARAM)&es);

	CloseHandle(hFile);
}

void SaveFile(LPSTR file)
{

	HANDLE hFile;
	hFile = CreateFile((LPCWSTR)file, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL);
	EDITSTREAM es = {0};
	es.dwCookie = (DWORD_PTR)hFile;
	es.pfnCallback = &EditStreamOutCallback;
	SendMessage(hWndEdit, EM_STREAMOUT, SF_RTF, (LPARAM)&es);

	CloseHandle(hFile);
}

void Format(HWND hWnd, int format)
{
	PARAFORMAT pf;
	CHARFORMAT2 cf;
	DWORD charRange;
	LRESULT select;
	
	switch (format)
	{
	case ID_COMBO_FONT:
		select = SendMessage(hWndComboBoxFont, CB_GETCURSEL, 0, 0);
		memset(&cf, 0, sizeof cf);
		cf.cbSize = sizeof cf;
		cf.dwMask = CFM_FACE;
		wcscpy(cf.szFaceName, FontList[select]);
		SendMessage(hWndEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
		//SendMessage(hWnd, EM_EXSETSEL, 0, (LPARAM)&charRange);
		break;
	case ID_COMBO_SIZE:
		select = SendMessage(hWndComboBoxSize, CB_GETCURSEL, 0, 0);
		memset(&cf, 0, sizeof cf);
		cf.cbSize = sizeof cf;
		cf.dwMask = CFM_SIZE;
		cf.yHeight = ((select + 12)*1440) / 72;
		SendMessage(hWndEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
		//SendMessage(hWnd, EM_EXSETSEL, 0, (LPARAM)&charRange);
		break;
	case ID_COMBO_ALLIGN:
		select = SendMessage(hWndComboBoxAllign, CB_GETCURSEL, 0, 0);
		memset(&pf, 0, sizeof pf);
		pf.cbSize = sizeof pf;
		pf.dwMask = PFM_ALIGNMENT;
		if (select == 0)
			pf.wAlignment = PFA_LEFT;
		else if (select == 1)
			pf.wAlignment = PFA_RIGHT;
		else pf.wAlignment = PFA_CENTER;
		SendMessage(hWndEdit, EM_SETPARAFORMAT, FALSE, (LPARAM)&pf);
		//SendMessage(hWnd, EM_EXSETSEL, 0, (LPARAM)&charRange);
		break;
	case -5:
		// add bold text
		memset(&cf, 0, sizeof cf);
		cf.cbSize = sizeof cf;
		cf.dwMask = CFM_ITALIC | CFM_BOLD | CFM_UNDERLINE | CFM_STRIKEOUT;
		cf.dwEffects = NULL;
		if (Bold)
			cf.dwEffects = cf.dwEffects | CFM_BOLD;
		if (Italic)
			cf.dwEffects = cf.dwEffects | CFM_ITALIC;
		if (Underline)
			cf.dwEffects = cf.dwEffects | CFM_UNDERLINE;
		if (Strikeout)
			cf.dwEffects = cf.dwEffects | CFM_STRIKEOUT;
		if (!Bold)
			cf.dwEffects = cf.dwEffects &~ CFM_BOLD;
		if (!Italic)
			cf.dwEffects = cf.dwEffects &~ CFM_ITALIC;
		if (!Underline)
			cf.dwEffects = cf.dwEffects &~ CFM_UNDERLINE;
		if (!Strikeout)
			cf.dwEffects = cf.dwEffects &~ CFM_STRIKEOUT;
		SendMessage(hWndEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
		//SendMessage(hWnd, EM_EXSETSEL, 0, (LPARAM)&charRange);
		break;
	case -4:
		memset(&cf, 0, sizeof cf);
		cf.cbSize = sizeof cf;
		if (ColorMode == 2)
		{
			cf.dwMask = CFM_BACKCOLOR;
			cf.crBackColor = Color;
		}
		else
		{
			cf.dwMask = CFM_COLOR;
			cf.crTextColor = Color;
		}
		SendMessage(hWndEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
		//SendMessage(hWnd, EM_EXSETSEL, 0, (LPARAM)&charRange);
	}
}

BOOL FindNext(HWND hRich, LONG sbegin, LONG send)
{
	INT len = GetWindowTextLengthW(hWndFind) + 1;
	WCHAR target[100];
	GetWindowTextW(hWndFind, target, len);

	CHARRANGE selectionRange;
	selectionRange.cpMax = send;
	selectionRange.cpMin = sbegin;

	FINDTEXTEX ftex;

	ftex.lpstrText = target;
	ftex.chrg.cpMin = selectionRange.cpMin;
	ftex.chrg.cpMax = selectionRange.cpMax;

	LRESULT lr = SendMessage(hRich, EM_FINDTEXTEXW, (WPARAM)FR_DOWN , (LPARAM)&ftex);

	if (lr >= 0)
	{
		found = true;
		LRESULT lr1 = SendMessage(hRich, EM_EXSETSEL, 0, (LPARAM)&ftex.chrgText);
		ibegin = ftex.chrgText.cpMax;
		SendMessage(hRich, EM_HIDESELECTION, (LPARAM)FALSE, 0);

		return TRUE;
	}
	SendMessage(hRich, EM_SETSEL, -1, -1);
	ibegin = 0;
	iend = -1;
	if (found)
		MessageBox(hWnd, L"End of text", L"Rich Text Editor" , 0);
	else
		MessageBox(hWnd, L"Not found", L"Rich Text Editor" , 0);
	found = false;
	return FALSE;
}

BOOL Replace()
{
	if (!found)
	{
		FindNext(hWndEdit, ibegin, iend);
		return false;
	}
	found = false;
	INT len = GetWindowTextLengthW(hWndReplace) + 1;
	WCHAR target[100];
	GetWindowTextW(hWndReplace, target, len);
	SendMessage(hWndEdit, EM_REPLACESEL, FALSE, (LPARAM)target);
	return true;
}

void ReplaceAll()
{
	while (FindNext(hWndEdit,ibegin, iend))
	{
		Replace();
	}
	return;
}