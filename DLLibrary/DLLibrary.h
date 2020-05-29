#pragma once
#include <Windows.h>

#ifdef DLLIBRARY_EXPORTS
#define DLLIBRARY_API __declspec(dllexport)
#else
#define DLLIBRARY_API __declspec(dllimport)
#endif

extern "C" DLLIBRARY_API void asyncRead(HANDLE hFile, char *buffer, DWORD nNumber, DWORD dRead, OVERLAPPED OL_read);
extern "C" DLLIBRARY_API void asyncWrite(HANDLE oFile, char *buffer, DWORD nNumber, DWORD dWrite, OVERLAPPED OL_write);

