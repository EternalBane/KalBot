// proxydll.h
#pragma once

// Exported function
IDirect3D9* WINAPI Direct3DCreate9 (UINT SDKVersion);
__declspec(dllexport) myIDirect3DDevice9* __cdecl getD3DDevice();

// regular functions
void InitInstance(HANDLE hModule);
void ExitInstance(void);
void LoadOriginalDll(void);