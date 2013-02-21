#pragma once

#include "Memory.h"
#include <iostream>
#include <winsock.h>

using namespace std;

// ----------- End of prototypes -------------- //

#define engineStart 0x00400000
#define engineEnd 0x00700000

#define recvIAT 0x00725468
#define sendIAT 0x0072546C

enum color {
	orange = 16594,
	lightblue = 15073034,
	violett = 12615808,
	green = 32768,
	pink = 16751615,
	blue = 15453831,
	red = 255 };

class KalTools
{
private:
	static FARPROC recvAddress;
	static FARPROC sendAddress;

	static DWORD chatAdd;
	static DWORD noticeAdd;
public:

	KalTools(){}
	~KalTools(){}

	// Getters //
	static FARPROC getRecvAddress(){return recvAddress;}

	static void hookRecv();
	static void hookSend();

	static void hookIATRecv();
	static void hookIATSend();

	static void Log(string str);
	static void Log(char *str);
	static void LogPacket(char *str, LPCSTR type);
	static void send(DWORD type, LPCSTR format...);

	static void OpenDat(char * dat,int x,int y,int w, int h);
	static void Chat(int color,char* mFormat,...);
	static void Notice(int color,char* mFormat,...);

    static void HookIt();
};
