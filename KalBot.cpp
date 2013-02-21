// KalBot.cpp: Definiuje funkcje wyeksportowane dla aplikacji DLL.
//

#include "stdafx.h"
#include "Memory.h"
#include "KalTools.h"

extern HANDLE hThread;                 

unsigned int __stdcall mainThread(void * pParams)
{
	Sleep(2000);
	CMemory::myAllocConsole(L"Test Console");
	KalTools tools;

	KalTools::HookIt();
	KalTools::hookIATRecv();
	KalTools::hookIATSend();
	while(1)
	{
		int option;
		cout << "1. Dance\nOption: ";
		cin >> option;
		if(option == 1)
		{
			DWORD pID,aniID;
			cin >> pID >> aniID;
			KalTools::send(0x3d,"db",pID,aniID);
		}
		system("cls");
		Sleep(100);
	}

	CloseHandle(hThread);
	_endthreadex(0);
	return NULL;
}