#include "stdafx.h"
#include "KalTools.h"
#include "Memory.h"

// ------------------- REAL_FUNCTIONS ------------------- //
typedef int (__cdecl* OpenDat_org)(char * , int , int , int , int);
typedef void (__cdecl* Notice_org)(char*, int);
typedef int (__cdecl * Chat_org)(char, char*, int);
typedef int (__stdcall *myRecv)(SOCKET s, char *buf, int len, int flags);
typedef int (__stdcall *mySend)(SOCKET s, char *buf, int len, int flags);

// Extern data from other files //
extern HWND textEdit;
extern DWORD dwJMPbackRecv;
extern bool logPacket;

extern int fRecvIAT(SOCKET s, char *buf, int len, int flags);
extern int fSendIAT(SOCKET s, char *buf, int len, int flags);
// --------------------------- //

myRecv oldRecv = NULL;
mySend oldSend = NULL;

FARPROC KalTools::recvAddress = GetProcAddress(GetModuleHandle("ws2_32.dll"),"recv");
FARPROC KalTools::sendAddress = GetProcAddress(GetModuleHandle("ws2_32.dll"),"send");

DWORD KalTools::chatAdd = 0;
DWORD KalTools::noticeAdd = 0;

void KalTools::Log(string str)
{
	cout << str << '\n';
}

void KalTools::Log(const char* mFormat, ... )
{
	char* mText = new char[255];
	va_list args;
	va_start(args, mFormat);
	vsprintf_s(mText,255,mFormat,args);
	va_end(args);

	printf(mFormat,mText);
}

void KalTools::LogTextBoxPacket(char *str, LPCSTR type)
{
	stringstream result;
	char *oldText;

	int txtlen=GetWindowTextLength(textEdit);
	if(txtlen > 2500)
		txtlen = 0;
	oldText = new char[txtlen+1];
	if(txtlen == 0)
		oldText[0] = '\0';

	GetWindowText(textEdit,oldText,txtlen);

	WORD size;
	memcpy(&size,str,2);
	result << oldText;
	if(txtlen != 0)
		result << "\r\n";
	result<< type << "Type[0x" << hex << static_cast<WORD>(str[2])
		<< dec	<< "] Size[" << size << "]\r\n";

	SendMessage(textEdit,WM_SETTEXT,0,(LPARAM)result.str().c_str());
	SendMessage(textEdit, LOWORD(WM_VSCROLL), SB_BOTTOM, 0);

	delete[]oldText;
}

void KalTools::LogTextBox(const char *mFormat, ...)
{
	char* mText = new char[255];
	va_list args;
	va_start(args, mFormat);
	vsprintf_s(mText,255,mFormat,args);
	va_end(args);

	string result;
	char *oldText;
	int txtlen=GetWindowTextLength(textEdit);
	if(txtlen > 5000)
		txtlen = 0;
	oldText = new char[txtlen+1];
	if(txtlen == 0)
		oldText[0] = '\0';
	GetWindowText(textEdit,oldText, txtlen);
	result += oldText;
	if(txtlen != 0)
		result += "\r\n";
	result += mText;

	SendMessage(textEdit,WM_SETTEXT,0,(LPARAM)result.c_str());
	SendMessage(textEdit, LOWORD(WM_VSCROLL), SB_BOTTOM, 0);

	delete[]mText;
}

void KalTools::LogPacket(char *str, LPCSTR type)
{
	// Dummy
}

void KalTools::Chat(int color,char* mFormat,...)
{ 
	char* mText = new char[255];
	va_list args;
	va_start(args, mFormat);
	vsprintf_s(mText,255,mFormat,args);
	va_end(args);

	((Chat_org)chatAdd)(0,mText,color);
	delete[]mText;
}

void KalTools::Notice(int color,char* mFormat,...)
{
	char* mText = new char[255];
	va_list args;
	va_start(args, mFormat);
	vsprintf_s(mText,255,mFormat,args);
	va_end(args);

	((Notice_org)noticeAdd)(mText,color);
	delete[]mText;
}

void KalTools::HookIt()
{
	// Chat BYTE pattern , char * mask //
	BYTE pChat[] = {0x55,0x8B,0xEC,0x83,0x3D,0x48,0x2B,0x86,0x00,0x00,0x74,0x17,0x8B,0x45,0x10,0x50}; // pattern //
	char * mChat = "xxx????????xxxx"; // mask //

	chatAdd = CMemory::dwFindPattern( 0x00400000,0x00700000,pChat,mChat);
	LogTextBox("[Chat Address]: 0x%x\n",chatAdd);


	// Notice BYTE pattern , char * mask //
	BYTE pNotice[] = {0x55,0x8B,0xEC,0x83,0x3D,0x5C,0x2B,0x86,0x00,0x00,0x74,0x34,0x8B,0x45,0x0C,0x50}; // pattern //
	char * mNotice = "xxx???????xxxxxx"; // mask //

	noticeAdd = CMemory::dwFindPattern( 0x00400000,0x00700000,pNotice,mNotice);  // obtain address //
	LogTextBox("[Notice Address]: 0x%x\n",noticeAdd);
}

// ------------- Hooking recv --------------- //

__declspec(naked) void fRecv()
{
	char *buf;

	__asm PUSHAD
	__asm PUSHFD

	// Getting data from func
	__asm mov eax, DWORD PTR [ebp+0xC] // buf pointer
	__asm MOV buf, eax

	if(logPacket)
	{
		KalTools::LogTextBoxPacket(buf,"Server->Client: ");
		KalTools::interpreter(buf);
	}

	__asm POPFD
	__asm POPAD

	__asm SUB ESP, 0x18
	__asm PUSH EBX
	__asm jmp $
	__asm JMP dwJMPbackRecv
} 

void KalTools::hookRecv()
{
	CMemory::placeJMP((BYTE*)(DWORD)recvAddress+0xA, (DWORD)fRecv, 5);
}

void KalTools::hookIATRecv()
{
	PDWORD address;
	address = reinterpret_cast<PDWORD>(recvIAT);
	oldRecv = (myRecv)*address;
	DWORD oldprot, oldprot2;

	VirtualProtect(address, sizeof(DWORD), PAGE_READWRITE, (DWORD *)&oldprot);
	*address = (DWORD)fRecvIAT;
	VirtualProtect(address, sizeof(DWORD), oldprot, (DWORD *)&oldprot2);
}
// ------ End of recv hooks ------ //

// ------ Send hooks ------- //

void KalTools::hookIATSend()
{
	PDWORD address;
	address = reinterpret_cast<PDWORD>(sendIAT);
	oldSend = (mySend)*address;
	DWORD oldprot, oldprot2;

	VirtualProtect(address, sizeof(DWORD), PAGE_READWRITE, (DWORD *)&oldprot);
	*address = (DWORD)fSendIAT;
	VirtualProtect(address, sizeof(DWORD), oldprot, (DWORD *)&oldprot2);
}

// ------ End of send hooks ----- //

void KalTools::send(DWORD type, LPCSTR format...)
{

}

void KalTools::interpreter(char *packet)
{
	Player player;
	Monster monster;
	if (packet[2] == 0x32)
	{
		//player appear
		memcpy((void*)&player.id,(void*)((DWORD)packet+3),4); // player id
		memcpy((void*)&player.name,(void*)((DWORD)packet+7),16); // player name
		int namelen = strlen(player.name);
		memcpy(&player.classe,(packet+8+namelen),1); // player class
		memcpy(&player.x,(packet+9+namelen),4); // player x
		memcpy(&player.y,(void*)((DWORD)packet+13+namelen),4); // player y
		memcpy(&player.z,(packet+17+namelen),2); // player z
		if(namelen>2)
		{
			char playerClass[7];
			switch(player.classe)
			{
			case 0:
				strcpy_s(playerClass,"Knight");
				break;
			case 1:
				strcpy_s(playerClass,"Mage");
				break;
			case 2:
				strcpy_s(playerClass,"Archer");
				break;
			case 3:
				strcpy_s(playerClass,"Thief");
			}
			Chat(Color::pink," Player: %s ID: %d",player.name,player.id);
			Chat(Color::pink," Class: %s X: %d Y: %d Z: %d",playerClass,player.x,player.y,player.z);
		}
	}
	if (packet[2] == 0x33)
	{
		//mob apear
		memcpy(&(monster.classe),(packet+3),2);
		memcpy(&(monster.id),(packet+5),4);
		memcpy(&(monster.x),(packet+9),4);
		memcpy(&(monster.y),(packet+13),4);
		memcpy(&(monster.z),(packet+17),2);
		memcpy(&(monster.HP),(packet+19),2);
		Chat(Color::violett," Mob: %d(%d) X: %d Y: %d Z: %d HP: %d",monster.classe,monster.id,monster.x,monster.y,monster.z,monster.HP);
	}
}