// dllmain.cpp: Okreœla punkt wejœcia dla aplikacji DLL.
#include "stdafx.h"

extern WORD mainThread();

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	std::thread myThread;
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		myThread = std::thread(mainThread);
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

