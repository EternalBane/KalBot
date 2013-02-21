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

	//KalTools::HookIt();
	KalTools::hookIATRecv();
	KalTools::hookIATSend();
	

	CloseHandle(hThread);
	_endthreadex(0);
	return NULL;
}