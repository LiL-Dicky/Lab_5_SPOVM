#include "DLLibrary.h"
#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <string>

using namespace std;

void asyncRead(HANDLE hFile, char *buffer, DWORD nNumber, DWORD dRead, OVERLAPPED OL_read)
{
	if(!ReadFile(hFile, buffer, nNumber, &dRead, &OL_read))
	{
		if (GetLastError() == ERROR_IO_PENDING)
			cout << endl << "AsyncReaded!" << endl;
		else
		{
			cout << "ErrorReadAsync!!!" << endl;
			return;
		}
	}
}

void asyncWrite(HANDLE oFile, char *buffer, DWORD nNumber, DWORD dWrite, OVERLAPPED OL_write)
{
	if(!WriteFile(oFile, buffer, nNumber, &dWrite, &OL_write))
	{
		if(GetLastError() == ERROR_IO_PENDING)
			cout << "AsyncWrited!" << endl;
		else
		{
			cout << "ErrorWriteAsync!!!" << endl;
			return;
		}
	}
}