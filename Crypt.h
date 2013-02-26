/*
Made by BakaBug - http://www.BakaBug.net
don't share it ! or it will get fixed xD
*/

#ifndef DecryptHeader
#define DecryptHeader

#define _DWORD DWORD
#define _WORD WORD
#define _BYTE BYTE
int DecryptPacket(char* buf);
signed int __stdcall DecryptAES(char* source, int size);
int EncryptPacket(char* buf);
signed int __stdcall EncryptAES(char* source, int size);
long DecryptTable(unsigned long index, unsigned char *buf, unsigned long len);
long EncryptTable(unsigned long index, unsigned char *buf, unsigned long len);
#endif