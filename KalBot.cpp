// KalBot.cpp: Definiuje funkcje wyeksportowane dla aplikacji DLL.
//

#include "stdafx.h"
#include "MainWindow.h"
#include "KalTools.h"
#include "Memory.h"

extern HANDLE hThread;
extern HWND prnt_hWnd;
extern HMODULE inj_hModule;
extern HWND textEdit;
extern int (__cdecl* SendPacket)(DWORD,LPCSTR,...);

DWORD WINAPI windowThread(LPVOID lpParam)
{
	MSG messages;
	HWND hwnd;
	char *pString = reinterpret_cast<char * > (lpParam);
	HMENU hMenu = CreateDLLWindowMenu();
	if(!(RegisterDLLWindowClass("myWindow")))
		MessageBox(0,"Error registering window class",0,0);
	if(!(prnt_hWnd = FindWindowA(0, "KalOnline")))
		MessageBox(0,"Error finding window",0,0);
	if(!(hwnd = CreateWindowExA (0, "myWindow", pString, WS_EX_PALETTEWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 500, 300, 0, hMenu, inj_hModule, NULL )))
		MessageBox(0,"Error creating window",0,0);
	textEdit = createTextBox(hwnd);
	ShowWindow (hwnd, SW_SHOWNORMAL);
	while (GetMessage (&messages, NULL, 0, 0))
	{
		TranslateMessage(&messages);
		DispatchMessage(&messages);
	}
	return 1;
}

DWORD WINAPI mainThread(void * pParams)
{
	Sleep(2000);
	CMemory::myAllocConsole(L"Test");
	KalTools::HookIt();
	KalTools::hookIATRecv();
	KalTools::hookIATSend();

	string text;
	int option;
	while(1)
	{
		cout << "1. Send text\n2. Login\nOption: ";
		cin >> option;
		switch(option)
		{
		case 1:
			system("cls");
			cout << "Text to send: ";
			cin >> text;
			SendPacket(0x0e,"s",text.c_str());
			cout << "Your message has been sent [" << text << "]\n";
			break;
		case 2:
			system("cls");
			SendPacket(0x02,"ss","lOp85RxH","p_KrychoPL");
			Sleep(2000);
			SendPacket(0x75,"s","00000000");
			cout << "Logging in...\n";
		}
		_getch();
		system("cls");
	}

	CloseHandle(hThread);
	_endthreadex(0);
	return NULL;
}