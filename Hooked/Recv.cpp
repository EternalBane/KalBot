#include "stdafx.h"
#include "KalTools.h"
#include "Recv.h"

int fRecvIAT(SOCKET s, char *buf, int len, int flags)
{
	if (ASyncPos==FinalSize && FinalSize>0)
	{
		if(logPacket)
		{
			//KalTools::LogTextBoxPacket(buf,"Server->Client: ");
			KalTools::interpreter(buf);
		}
		ASyncPos = 0;
	}
	int ret = oldRecv(s,buf,len,flags);
	if (ret<0)
	{
		return ret;
	}
	if (ASyncPos==0)
		FinalSize = *((short int*) buf);
	ASyncPos+=ret;
	return ret;
}