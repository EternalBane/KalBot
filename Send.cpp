#include "stdafx.h"
#include "KalTools.h"
#include "Send.h"

typedef int (__stdcall *mySend)(SOCKET s, char *buf, int len, int flags);

extern bool logPacket;
extern mySend oldSend;

int fSendIAT(SOCKET s, char *buf, int len, int flags)
{
	if(logPacket)
		KalTools::LogPacket(buf,"Client->Server: ");
	return oldSend(s,buf,len,flags);
}