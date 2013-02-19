// KalBot.cpp: Definiuje funkcje wyeksportowane dla aplikacji DLL.
//

#include "stdafx.h"
#include "DLLDumper.h"
#include "detours.h"

#pragma comment(lib, "detours.lib")

using namespace std;

typedef int (WINAPI *myrecv)(SOCKET s, char *buf, int len, int flags);

myrecv NewRecv = NULL, OldRecv = NULL;

int WINAPI fRecv(SOCKET s, char *buf, int len, int flags)
{
	cout << "Hooked recv: ";
	for(int i=0;i<strlen(buf);i++)
		cout << hex << static_cast<WORD>(buf[i]) << ' ';
	cout << '\n';
	//return OldRecv(s,buf,len,flags);
	return len;
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

void *DetourFunction (BYTE *src, const BYTE *dst, const int len)
{
    BYTE *jmp;
    DWORD dwback;
    DWORD jumpto, newjump;

    VirtualProtect(src,len,PAGE_READWRITE,&dwback);
    
    if(src[0] == 0xE9)
    {
        jmp = (BYTE*)malloc(10);
        jumpto = (*(DWORD*)(src+1))+((DWORD)src)+5;
        newjump = (jumpto-(DWORD)(jmp+5));
        jmp[0] = 0xE9;
       *(DWORD*)(jmp+1) = newjump;
        jmp += 5;
        jmp[0] = 0xE9;
       *(DWORD*)(jmp+1) = (DWORD)(src-jmp);
    }
    else
    {
        jmp = (BYTE*)malloc(5+len);
        memcpy(jmp,src,len);
        jmp += len;
        jmp[0] = 0xE9;
       *(DWORD*)(jmp+1) = (DWORD)(src+len-jmp)-5;
    }
    src[0] = 0xE9;
   *(DWORD*)(src+1) = (DWORD)(dst - src) - 5;

    for(int i = 5; i < len; i++)
        src[i] = 0x90;
    VirtualProtect(src,len,dwback,&dwback);
    return (jmp-len);
} 

unsigned int __stdcall mainThread(void * pParams)
{
	myAllocConsole(L"Test Console");
	//OldRecv = (myrecv)GetProcAddress(GetModuleHandle(L"ws2_32.dll"),"recv");
	//cout << "Recv orig: " << &OldRecv << " Recv GPA: " << GetProcAddress(GetModuleHandle(L"ws2_32.dll"),"recv") << '\n';
	//cout << "fRecv: " << &fRecv;

	//
	////OldRecv = (myrecv)DetourFunction((BYTE*)OldRecv, (BYTE*)fRecv, 5);
	//cout << "\nAfter hooking...\n";
	//cout << "Recv orig: " << &OldRecv << " Recv GPA: " << GetProcAddress(GetModuleHandle(L"ws2_32.dll"),"recv") << '\n';
	//cout << "fRecv: " << &fRecv;

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