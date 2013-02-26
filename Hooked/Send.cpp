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
	string packet(buf,buf+len);
	if(KalTools::getTableKey() != 0)
	{
		DWORD tKey = ((*KalTools::getTableKey())-1) & 0x3F;
		DecryptTable(tKey,(unsigned char*)packet.c_str()+2,len-2);
		DecryptPacket((char*)packet.c_str());
		if(packet[2] == 0x09)
			KalTools::interpreter((char*)packet.c_str());
		if(logPacket)
		{
			//KalTools::LogTextBox(" ");
			//for(int i=0;i<int(*(PWORD(packet.c_str())));i++)
			//{
			//	KalTools::LogTextBoxNl(" %X ",(BYTE)packet[i]);
			//	if(i%22==0)
			//		KalTools::LogTextBox(" ");
			//}
		}
	}
	return oldSend(s,buf,len,flags);
}