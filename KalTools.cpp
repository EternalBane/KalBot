#include "stdafx.h"
#include "KalTools.h"

// ------------------- REAL_FUNCTIONS ------------------- //
typedef int (__cdecl* OpenDat_org)(char * , int , int , int , int);
typedef void (__cdecl* Notice_org)(char*, int);
typedef int (__cdecl * Chat_org)(char, char*, int);
typedef int (__stdcall *myRecv)(SOCKET s, char *buf, int len, int flags);
typedef int (__stdcall *mySend)(SOCKET s, char *buf, int len, int flags);

myRecv oldRecv = NULL;
mySend oldSend = NULL;

FARPROC KalTools::recvAddress = GetProcAddress(GetModuleHandle(L"ws2_32.dll"),"recv");
FARPROC KalTools::sendAddress = GetProcAddress(GetModuleHandle(L"ws2_32.dll"),"send");

DWORD KalTools::chatAdd = 0;
DWORD KalTools::noticeAdd = 0;

void KalTools::Log(string str)
{
	cout << str << '\n';
}

void KalTools::Log(char *str)
{
	for(int i=0;i<strlen(str);i++)
		cout << str[i];
	cout << '\n';
}

void KalTools::LogPacket(char *str, LPCSTR type)
{
	cout << type << ' ';
	for(int i=0;i<strlen(str);i++)
		cout << hex << "0x" << static_cast<WORD>(str[i]) << ' ';
	cout << '\n';
}

void KalTools::Chat(int color,char* mFormat,...){ 
	char* mText = new char[255];
	va_list args;
	va_start(args, mFormat);
	vsprintf_s(mText,255,mFormat,args);
	va_end(args);

	((Chat_org)chatAdd)(0,mText,color);
}

void KalTools::Notice(int color,char* mFormat,...){
	char* mText = new char[255];
	va_list args;
	va_start(args, mFormat);
	vsprintf_s(mText,255,mFormat,args);
	va_end(args);

	((Notice_org)noticeAdd)(mText,color);
}

void KalTools::HookIt()
{
	// Chat BYTE pattern , char * mask //
	BYTE pChat[] = {0x55,0x8B,0xEC,0x83,0x3D,0x48,0x2B,0x86,0x00,0x00,0x74,0x17,0x8B,0x45,0x10,0x50}; // pattern //
	char * mChat = "xxx????????xxxx"; // mask //

	chatAdd = CMemory::dwFindPattern( 0x00400000,0x00700000,pChat,mChat);
	printf("[Chat Address]: 0x%x\n",chatAdd);

	// Notice BYTE pattern , char * mask //
	BYTE pNotice[] = {0x55,0x8B,0xEC,0x83,0x3D,0x5C,0x2B,0x86,0x00,0x00,0x74,0x34,0x8B,0x45,0x0C,0x50}; // pattern //
	char * mNotice = "xxx???????xxxxxx"; // mask //

	noticeAdd = CMemory::dwFindPattern( 0x00400000,0x00700000,pNotice,mNotice);  // obtain address //
	printf("[Notice Address]: 0x%x\n",noticeAdd);
}

// ------------- Hooking --------------- //

DWORD dwJMPbackRecv = (DWORD)KalTools::getRecvAddress() + 0xF; //The Jump Back address
DWORD dwJMPbackSend;

__declspec(naked) void fRecv()
{
	char *buf;

	__asm PUSHAD
	__asm PUSHFD

	// Getting data from func
	__asm mov eax, DWORD PTR [ebp+0xC] // buf pointer
	__asm MOV buf, eax

	KalTools::LogPacket(buf,"Recv: ");

	__asm POPFD
	__asm POPAD

	__asm SUB ESP, 0x18
	__asm PUSH EBX
	__asm JMP dwJMPbackRecv
} 

void KalTools::hookRecv()
{
	CMemory::placeJMP((BYTE*)(DWORD)recvAddress+0xA, (DWORD)fRecv, 5);
}

int fRecvIAT(SOCKET s, char *buf, int len, int flags)
{
	KalTools::LogPacket(buf,"Recv: ");
	return oldRecv(s,buf,len,flags);
}

void KalTools::hookIATRecv()
{
	PDWORD address;
	address = reinterpret_cast<PDWORD>(recvIAT);
	oldRecv = (myRecv)recvAddress;
	DWORD oldprot, oldprot2;

	VirtualProtect(address, sizeof(DWORD), PAGE_READWRITE, (DWORD *)&oldprot);
	*address = (DWORD)fRecvIAT;
	VirtualProtect(address, sizeof(DWORD), oldprot, (DWORD *)&oldprot2);
}
// ------ End of recv hooks ------ //

// ------ Send hooks ------- //

int fSendIAT(SOCKET s, char *buf, int len, int flags)
{
	KalTools::LogPacket(buf,"Send: ");
	return oldSend(s,buf,len,flags);
}

void KalTools::hookIATSend()
{
	PDWORD address;
	address = reinterpret_cast<PDWORD>(sendIAT);
	oldSend = (mySend)sendAddress;
	DWORD oldprot, oldprot2;

	VirtualProtect(address, sizeof(DWORD), PAGE_READWRITE, (DWORD *)&oldprot);
	*address = (DWORD)fSendIAT;
	VirtualProtect(address, sizeof(DWORD), oldprot, (DWORD *)&oldprot2);
}

void KalTools::send(DWORD type, LPCSTR format...)
{

}

// ------ End of send hooks ----- //