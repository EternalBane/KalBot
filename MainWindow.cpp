#include "stdafx.h"
#include "MainWindow.h"

//Register our windows Class
BOOL RegisterDLLWindowClass(wchar_t szClassName[])
{
	WNDCLASSEX wc;
	wc.hInstance =  inj_hModule;
	wc.lpszClassName = (LPCWSTR)L"InjectedDLLWindowClass";
	wc.lpszClassName = (LPCWSTR)szClassName;
	wc.lpfnWndProc = DLLWindowProc;
	wc.style = CS_DBLCLKS;
	wc.cbSize = sizeof (WNDCLASSEX);
	wc.hIcon = LoadIcon (NULL, IDI_APPLICATION);
	wc.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor (NULL, IDC_ARROW);
	wc.lpszMenuName = NULL;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH) COLOR_BACKGROUND;
	if (!RegisterClassEx (&wc))
		return 0;
	return 1;
}

LRESULT CALLBACK DLLWindowProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
		switch(wParam)
		{
		case MYMENU_EXIT:
			SendMessage(hwnd, WM_CLOSE, 0, 0);
			break;
		case HACK1:		
			MessageBox(hwnd, L"No Recoil Activated!", L"", MB_ICONINFORMATION | MB_OK);
			break;
		case HACK2:		
			MessageBox(hwnd, L"No Spread Activated!", L"", MB_ICONINFORMATION | MB_OK);
			break;
		case HACK3:			
			MessageBox(hwnd, L"No Bullets Activated!", L"", MB_ICONINFORMATION | MB_OK);
			break;
		case HACK4:
			MessageBox(hwnd, L"Engine ESP Activated!", L"", MB_ICONINFORMATION | MB_OK);
			break;
		case HACK5:
			MessageBox(hwnd, L"Radar Hack Activated!", L"", MB_ICONINFORMATION | MB_OK);
			break;
		case ABOUT:
			MessageBox(hwnd, L"Created by : Crypt @DarkHook", L"", MB_ICONINFORMATION | MB_OK);
			break;
		}
		break;
	case WM_DESTROY:
		PostQuitMessage (0);
		break;
	default:
		return DefWindowProc (hwnd, message, wParam, lParam);
	}
	return 0;
}

HMENU CreateDLLWindowMenu()
{
	HMENU hMenu;
	hMenu = CreateMenu();
	HMENU hMenuPopup;
	if(hMenu==NULL)
		return FALSE;
	hMenuPopup = CreatePopupMenu();
	AppendMenu (hMenuPopup, MF_STRING, MYMENU_EXIT, TEXT("Exit"));
	AppendMenu (hMenu, MF_POPUP, (UINT_PTR) hMenuPopup, TEXT("File")); 

	hMenuPopup = CreatePopupMenu();
	AppendMenu (hMenuPopup, MF_STRING,HACK1, TEXT("No Recoil"));
	AppendMenu (hMenuPopup, MF_STRING,HACK2, TEXT("No Spread"));
	AppendMenu (hMenuPopup, MF_STRING,HACK3, TEXT("No Bullets"));
	AppendMenu (hMenuPopup, MF_STRING,HACK4, TEXT("Engine ESP"));
	AppendMenu (hMenuPopup, MF_STRING,HACK5, TEXT("Radar Hack"));
	AppendMenu (hMenu, MF_POPUP, (UINT_PTR) hMenuPopup, TEXT("Hacks")); 

	hMenuPopup = CreatePopupMenu();
	AppendMenu (hMenuPopup, MF_STRING,ABOUT, TEXT("Credits"));
	AppendMenu (hMenu, MF_POPUP, (UINT_PTR) hMenuPopup, TEXT("About")); 

	return hMenu;
}