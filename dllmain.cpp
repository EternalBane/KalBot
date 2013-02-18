// dllmain.cpp: Okreœla punkt wejœcia dla aplikacji DLL.
#include "stdafx.h"

extern unsigned int _stdcall mainThread(void *pParams);

HANDLE hThread = INVALID_HANDLE_VALUE;
unsigned int threadID;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		hThread = (HANDLE)_beginthreadex(NULL, 0, &mainThread, NULL, NULL, &threadID); 
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

