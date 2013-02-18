// KalBot.cpp: Definiuje funkcje wyeksportowane dla aplikacji DLL.
//

#include "stdafx.h"
#include "DLLDumper.h"

using namespace std;

typedef int (WINAPI *myrecv)(SOCKET s, char *buf, int len, int flags);

myrecv NewRecv = NULL, OldRecv = NULL;

int WINAPI fRecv(SOCKET s, char *buf, int len, int flags)
{
	cout << "Hooked recv: ";
	for(int i=0;i<len;i++)
		cout << hex << buf[i] << ' ';
	return OldRecv(s,buf,len,flags);
}

int g_hCrtIn		= NULL;
int g_hCrtOut		= NULL;

void myAllocConsole(wstring consoleName)
{
	FILE *conIn, *conOut;

	AllocConsole();
	g_hCrtIn = _open_osfhandle ((intptr_t) GetStdHandle(STD_INPUT_HANDLE), _O_TEXT);
	g_hCrtOut = _open_osfhandle ((intptr_t) GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT);
	conIn = _fdopen( g_hCrtIn, "r" );
	conOut = _fdopen( g_hCrtOut, "w" );
	*stdin = *conIn;
	*stdout = *conOut;
	SetConsoleTitle(consoleName.c_str());
} 

typedef INT (WINAPI *SendPtr)(SOCKET sock, CHAR* buf, INT len, INT flags);
typedef INT (WINAPI *RecvPtr)(SOCKET sock, CHAR* buf, INT len, INT flags);

INT WINAPI OurSend (SOCKET sock, CHAR* buf, INT len, INT flags);
INT WINAPI OurRecv (SOCKET sock, CHAR* buf, INT len, INT flags);

VOID *Detour(BYTE *source, CONST BYTE *destination, CONST INT length);

SendPtr RealSend;
RecvPtr RealRecv;

VOID *Detour(BYTE *source, CONST BYTE *destination, CONST INT length)
{
	DWORD back;
	BYTE *jmp = (BYTE*)malloc(length + 5);

	VirtualProtect(source, length, PAGE_READWRITE, &back);
	memcpy(jmp, source, length);
	jmp += length;

	jmp[0] = 0xE9;
	*(DWORD*)(jmp + 1) = (DWORD)(source + length - jmp) - 5;

	source[0] = 0xE9;
	*(DWORD*)(source + 1) = (DWORD)(destination - source) - 5;

	VirtualProtect(source, length, back, &back);

	return (jmp - length);
}

INT WINAPI OurRecv(SOCKET sock, CHAR* buf, INT len, INT flags)
{
	cout << "Received: ";
	for(int i=0;i<strlen(buf);i++)
		cout <<  hex << static_cast<WORD>(buf[i]) << ' ';
	cout << '\n';
	return RealRecv(sock, buf, len, flags);
}

unsigned int __stdcall mainThread(void * pParams)
{
	myAllocConsole(L"Test Console");
	DLLDumper dll;
	map<string,PDWORD>funcs;
	map<string,PDWORD>::iterator it;
	wstring name;
	string fname;
	int option = 0;

	while(option != -1)
	{
		cout << "1. Set DLL\n"
			<< "2. Print EAT\n"
			<< "3. Print IAT\n"
			<< "4. Hook EAT\n"
			<< "Option: ";
		cin >> option;
		switch(option)
		{
		case 1:
			system("cls");
			cout << "Name of the dll: ";
			wcin >> name;
			if(!dll.setDLL(name))
				cout << "Error retrieving dll.\n";
			break;
		case 2:
			system("cls");
			funcs = dll.getMappedEAT();
			it = funcs.begin();
			for(;it!=funcs.end();it++)
			{
				cout << it->first << " " << it->second << '\n';
			}
			break;
		case 3:
			system("cls");
			funcs = dll.getMappedIAT();
			it = funcs.begin();
			for(;it!=funcs.end();it++)
			{
				cout << it->first << " " << it->second << '\n';
			}
			break;
		case 4:
			system("cls");
			cout << "Func name to hook: ";
			cin >> fname;
			OldRecv = (myrecv)dll.hookEAT(fname,(void*)fRecv);
			NewRecv = (myrecv)GetProcAddress(GetModuleHandle(L"ws2_32.dll"),"recv");
			cout << "Checking hook...\n";
			NewRecv(NULL,"aaa",3,NULL);
		}
		_getch();
		system("cls");
	}
	_endthreadex(0);
	return NULL;
}