#include "stdafx.h"
#include "KalTools.h"
#include "Send.h"

typedef int (__stdcall *mySend)(SOCKET s, char *buf, int len, int flags);

extern bool logPacket;
extern mySend oldSend;
extern long DecryptTable(unsigned long index, unsigned char *buf, unsigned long len);
extern int DecryptPacket(char* buf);

int fSendIAT(SOCKET s, char *buf, int len, int flags)
{
	unsigned char packet[1000];
	memcpy(packet,buf,len);
	if(KalTools::getSendKey() != 0)
	{
		DecryptTable(*KalTools::getSendKey(),(unsigned char*)packet+3,len-3);
		DecryptPacket((char*)packet);
		if(logPacket)
		{
			KalTools::LogTextBox(" ");
			for(int i=0;i<int(*(PWORD(packet)));i++)
			{
				KalTools::LogTextBoxNl(" %X ",(BYTE)packet[i]);
				if(i%22==0)
					KalTools::LogTextBox(" ");
			}
		}
	}
	return oldSend(s,buf,len,flags);
}