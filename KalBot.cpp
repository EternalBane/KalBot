// KalBot.cpp: Definiuje funkcje wyeksportowane dla aplikacji DLL.
//

#include "stdafx.h"


WORD mainThread()
{
	AllocConsole();
	std::cout << "test";
	return NULL;
}