#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <windows.h>

TCHAR szClassName[] = TEXT("Window");

int CALLBACK EnumFontFamProc(const LOGFONT FAR* lpelf, const TEXTMETRIC FAR* lpntm, DWORD FontType, LPARAM lParam)
{
	if (lpelf->lfFaceName[0] != TEXT('@'))
	{
		SendMessage(*(HWND*)lParam, LB_ADDSTRING, 0, (LPARAM)lpelf->lfFaceName);
	}
	return 1;
}

int CALLBACK EnumFontFamiliesExProc(ENUMLOGFONTEX *lpelfe, NEWTEXTMETRICEX *lpntme, int FontType, LPARAM lParam)
{
	CopyMemory((void*)lParam, &lpelfe->elfLogFont, sizeof(LOGFONT));
	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND hList;
	static HWND hEdit;
	static HFONT hFont = 0;
	switch (msg)
	{
	case WM_CREATE:
		hList = CreateWindow(TEXT("LISTBOX"), 0, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | LBS_STANDARD | LBS_NOINTEGRALHEIGHT, 10, 10, 256, 512, hWnd, (HMENU)100, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hEdit = CreateWindow(TEXT("EDIT"), TEXT("ABCDEFGHIJKLMNOPQRSTUVWXY\r\nabcdefghijklmnopqrstuvwxy\r\n0123456789\r\n\r\nいろはにほへと ちりぬるを\r\nわかよたれそ つねならむ\r\nうゐのおくやま けふこえて\r\nあさきゆめみし ゑひもせすん"), WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY | ES_MULTILINE, 276, 10, 512, 512, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		{
			const HDC hdc = GetDC(hWnd);
			EnumFontFamilies(hdc, 0, EnumFontFamProc, (LPARAM)&hList);
			ReleaseDC(hWnd, hdc);
			SendMessage(hList, LB_SETCURSEL, 0, 0);
			SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(100, LBN_SELCHANGE), 0);
			SetFocus(hList);
		}
		break;
	case WM_SIZE:
		MoveWindow(hList, 10, 10, 256, HIWORD(lParam) - 20, TRUE);
		MoveWindow(hEdit, 276, 10, LOWORD(lParam) - 286, HIWORD(lParam) - 20, TRUE);
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == 100 && HIWORD(wParam) == LBN_SELCHANGE)
		{
			DWORD_PTR dwCursel = SendMessage(hList, LB_GETCURSEL, 0, 0);
			LPTSTR strText = (LPTSTR)GlobalAlloc(GMEM_FIXED, sizeof(TCHAR)*(SendMessage(hList, LB_GETTEXTLEN, dwCursel, 0) + 1));
			SendMessage(hList, LB_GETTEXT, dwCursel, (LPARAM)strText);
			LOGFONT lf;
			memset(&lf, 0, sizeof(LOGFONT));
			lf.lfCharSet = DEFAULT_CHARSET;
			lstrcpy(lf.lfFaceName, strText);
			GlobalFree(strText);
			const HDC hdc = GetDC(hWnd);
			EnumFontFamiliesEx(hdc, &lf, (FONTENUMPROC)EnumFontFamiliesExProc, (LONG_PTR)&lf, 0);
			ReleaseDC(hWnd, hdc);
			DeleteObject(hFont);
			hFont = CreateFontIndirect(&lf);
			SendMessage(hEdit, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(1, 0));
		}
		break;
	case WM_DESTROY:
		DeleteObject(hFont);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPSTR pCmdLine, int nCmdShow)
{
	MSG msg;
	WNDCLASS wndclass = {
		CS_HREDRAW | CS_VREDRAW,
		WndProc,
		0,
		0,
		hInstance,
		0,
		LoadCursor(0,IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		0,
		szClassName
	};
	RegisterClass(&wndclass);
	HWND hWnd = CreateWindow(
		szClassName,
		TEXT("インストール フォント 一覧"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		0,
		CW_USEDEFAULT,
		0,
		0,
		0,
		hInstance,
		0
	);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}
