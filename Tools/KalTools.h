#pragma once

using namespace std;

// ----------- End of prototypes -------------- //

#define engineStart 0x00400000
#define engineEnd 0x00700000

#define recvIAT 0x00725468
#define sendIAT 0x0072546C

enum Color
{
	orange = 16594,
	lightblue = 15073034,
	violett = 12615808,
	green = 32768,
	pink = 16751615,
	blue = 15453831,
	red = 255
};

struct Monster
{
	DWORD id;
	WORD classe;
	DWORD x;
	DWORD y;
	WORD z;
	WORD HP;
};

struct Player
{
	DWORD id;
	char name[16];
	BYTE classe;
	DWORD x;
	DWORD y;
	WORD z;
};

static int (__cdecl* SendPacket)(DWORD,LPCSTR,...) = (int (__cdecl*)(DWORD,LPCSTR,...))0x005A0100;

class KalTools
{
private:
	static FARPROC recvAddress;
	static FARPROC sendAddress;

	static DWORD chatAdd;
	static DWORD noticeAdd;
	static DWORD sendAdd;
	static BYTE* sendKey;
	static BYTE* tableKey;
	static DWORD syncClient;
	static DWORD charID;
public:

	KalTools(){}
	~KalTools(){}

	// Getters //
	static FARPROC getRecvAddress(){return recvAddress;}
	static DWORD getSendAddress(){return sendAdd;}
	static BYTE* getSendKey(){return sendKey;}
	static BYTE* getTableKey(){return tableKey;}
	static DWORD getSyncClient(){return syncClient;}
	static DWORD getCharID(){return charID;}

	static void hookRecv();

	static void hookIATRecv();
	static void hookIATSend();

	static void Log(string str);
	static void Log(const char* format, ...);
	static void LogPacket(char *str, LPCSTR type);
	static void send(DWORD type, LPCSTR format...);
	static void LogTextBoxPacket(char* text, LPCSTR type);
	static void LogTextBox(const char* mFormat, ...);
	static void LogTextBoxBytes(char *text, int len);
	static void LogTextBoxNl(const char *mFormat, ...);

	static void OpenDat(char * dat,int x,int y,int w, int h);
	static void Chat(int color,char* mFormat,...);
	static void Notice(int color,char* mFormat,...);
	static void SendEngine(DWORD Header,LPCSTR szFormat,...);

	static void HookIt();
	static void interpreter(char *buf);
};

