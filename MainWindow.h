#pragma once

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

HINSTANCE  inj_hModule;          //Injected Modules Handle
HWND       prnt_hWnd;            //Parent Window Handle
bool logPacket = 0;

#define MYMENU_EXIT         (WM_APP + 101)
#define HACK1				(WM_APP + 102)
#define HACK2				(WM_APP + 103)
#define HACK3				(WM_APP + 104)
#define HACK4				(WM_APP + 105) 
#define HACK5				(WM_APP + 106) 
#define ABOUT				(WM_APP + 107)
#define START_LOG			(WM_APP + 108)
#define STOP_LOG			(WM_APP + 109)

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
		case START_LOG:
			logPacket = 1;
			break;
		case STOP_LOG:
			logPacket = 0;
			break;
		case HACK1:		
			MessageBox(hwnd, "No Recoil Activated!", "", MB_ICONINFORMATION | MB_OK);
			break;
		case HACK2:		
			MessageBox(hwnd, "No Spread Activated!", "", MB_ICONINFORMATION | MB_OK);
			break;
		case HACK3:			
			MessageBox(hwnd, "No Bullets Activated!", "", MB_ICONINFORMATION | MB_OK);
			break;
		case HACK4:
			MessageBox(hwnd, "Engine ESP Activated!", "", MB_ICONINFORMATION | MB_OK);
			break;
		case HACK5:
			MessageBox(hwnd, "Radar Hack Activated!", "", MB_ICONINFORMATION | MB_OK);
			break;
		case ABOUT:
			MessageBox(hwnd, "Created by : Crypt @DarkHook", "", MB_ICONINFORMATION | MB_OK);
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

BOOL RegisterDLLWindowClass(char szClassName[])
{
	WNDCLASSEX wc;
	wc.hInstance =  inj_hModule;
	wc.lpszClassName = szClassName;
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

HMENU CreateDLLWindowMenu()
{
	HMENU hMenu;
	hMenu = CreateMenu();
	HMENU hMenuPopup;
	if(hMenu==NULL)
		return FALSE;
	hMenuPopup = CreatePopupMenu();
	AppendMenu (hMenuPopup, MF_STRING, MYMENU_EXIT, TEXT("Exit"));
	AppendMenu (hMenuPopup, MF_STRING, START_LOG, TEXT("Start packet log"));
	AppendMenu (hMenuPopup, MF_STRING, STOP_LOG, TEXT("Stop packet log"));
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

HWND createTextBox(HWND hwnd)
{
	RECT rect;
	GetWindowRect(hwnd,&rect);
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;
	HWND hText = CreateWindowEx( 0, "EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | ES_MULTILINE | ES_WANTRETURN | ES_READONLY | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
		0, 0, width, height, hwnd, NULL, (HINSTANCE)GetWindowLongA(hwnd,GWL_HINSTANCE), NULL );
	return hText;
}



#endif