#include "stdafx.h"
#include "KalTools.h"

// ------------------- REAL_FUNCTIONS ------------------- //
typedef int (__cdecl* OpenDat_org)(char * , int , int , int , int);
typedef void (__cdecl* Notice_org)(char*, int);
typedef int (__cdecl * Chat_org)(char, char*, int);
typedef int (__stdcall *myRecv)(SOCKET s, char *buf, int len, int flags);
typedef int (__stdcall *mySend)(SOCKET s, char *buf, int len, int flags);

extern HWND textEdit;
extern bool logPacket;

myRecv oldRecv = NULL;
mySend oldSend = NULL;

FARPROC KalTools::recvAddress = GetProcAddress(GetModuleHandle("ws2_32.dll"),"recv");
FARPROC KalTools::sendAddress = GetProcAddress(GetModuleHandle("ws2_32.dll"),"send");

DWORD KalTools::chatAdd = 0;
DWORD KalTools::noticeAdd = 0;

SOCKET KalTools::sock = 0;

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

void KalTools::LogTextBox(char *str, LPCSTR type)
{
	string result;
	char *oldText;
	int txtlen=GetWindowTextLength(textEdit);
	if(txtlen > 5000)
		txtlen = 0;
	oldText = new char[txtlen+1];
	if(txtlen == 0)
		oldText[0] = '\0';
	GetWindowText(textEdit,oldText, txtlen);
	WORD size;
	memcpy(&size,str,2);
	stringstream sstream, sstream2;
	sstream << hex << static_cast<WORD>(str[2]);
	string packetType = sstream.str();
	string packetSize;
	sstream2 << size;
	packetSize = sstream2.str();
	result += oldText;
	if(txtlen != 0)
		result += "\r\n";
	result += type;
	result += "Type[";
	result += packetType;
	result += "] Size[";
	result += packetSize;
	result += "]\r\n";
	SendMessage(textEdit,WM_SETTEXT,0,(LPARAM)result.c_str());
}

void KalTools::LogPacket(char *str, LPCSTR type)
{
	//cout << type;
	//printf("0x%02x",str[2]);
	//WORD size;
	//memcpy(&size,str,2);
	//cout << " size: " << size;
	//cout << '\n';
	if(logPacket)
		LogTextBox(str,type);
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

	KalTools::LogPacket(buf,"Server->Client: ");

	__asm POPFD
	__asm POPAD

	__asm SUB ESP, 0x18
	__asm PUSH EBX
	__asm JMP dwJMPbackRecv
} 

void KalTools::hookRecv()
{
	cout << "Recv addr: " << recvAddress << '\n';
	CMemory::placeJMP((BYTE*)(DWORD)recvAddress+0xA, (DWORD)fRecv, 5);
}

int fRecvIAT(SOCKET s, char *buf, int len, int flags)
{
	KalTools::LogPacket(buf,"Server->Client: ");
	KalTools::interpreter(buf);
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
	KalTools::sock = s;
	KalTools::LogPacket(buf,"Client->Server: ");
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

// ------ End of send hooks ----- //

int countSize(LPCSTR str)
{
	int result = 0;
	for(int i=0;i<strlen(str);i++)
	{
		if(str[i]=='b')
			result += 1;
		if(str[i]=='d')
			result += 4;
	}
	return result;
}

void KalTools::send(DWORD type, LPCSTR format...)
{
	char SendText[] = "PACKET TYPE:0x%02x FORMAT:%s\n";
	DWORD temp = 0;
	char *packet;

	printf(SendText,type,format);

	va_list args;
	va_start(args, format);
	int i,s=0;
	char* something;
	packet = new char[countSize(format)];
	for (i=0;i<strlen(format);i++)
	{
		switch (format[i])
		{
		case 'b': //BYTE
			memcpy(packet+s,format+s,1);
			s+=1;
			temp=va_arg( args, BYTE);
			printf(" %d: %d\n",i+1,temp);
			break;
		case 'd': //DWORD
			memcpy(packet+s,format+s,4);
			s+=4;
			temp=(DWORD)va_arg( args, DWORD);
			printf(" %d: %d\n",i+1,temp);
			break;       
		case 'w': //WORD
			printf(" %d: %d\n",i+1,(WORD)va_arg( args, DWORD));
			break;
		case 's': //STRING
			something=va_arg( args, char*);
			printf(" %d: %s\n",i+1,something);
			break;
		case 'm':
			printf(" %d: %d\n",i+1,(DWORD)va_arg( args, DWORD));
			break;
		}
	}
	va_end(args);
	fSendIAT(KalTools::sock,packet,countSize(format),0);
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