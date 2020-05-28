

#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <conio.h>
#include <string.h>

using namespace std;

HINSTANCE hDLLibrary;
WIN32_FIND_DATA ffd;
HANDLE hFile, oFile, sFile, thread_reader, thread_writer, hEvent;
OVERLAPPED read_lapped = { 0, 0, 0, 0, NULL }, write_lapped = { 0, 0, 0, 0, NULL };
DWORD dRead, dWrite;
char buffer[150];
bool quit = false;

void (*DLLRead) (HANDLE hFile, char *buffer, DWORD nNumber, DWORD dRead, OVERLAPPED OL_read);
void (*DLLWrite) (HANDLE oFile, char *buffer, DWORD nNumber, DWORD dWrite, OVERLAPPED OL_write);

DWORD WINAPI funcThreadReader(LPVOID t)
{	
	while(true)
	{
		WaitForSingleObject(read_lapped.hEvent, INFINITE);
		
		if(quit) ExitThread(0);
		
		DLLRead(hFile, buffer, sizeof(buffer), dRead, read_lapped);
		
		ResetEvent(read_lapped.hEvent);
		SetEvent(hEvent);
	}
}

DWORD WINAPI funcThreadWriter(LPVOID t)
{
	while(true)
	{
		WaitForSingleObject(write_lapped.hEvent, INFINITE);
		
		if(quit) ExitThread(0);
		
		DLLWrite(oFile, buffer, strlen(buffer), dWrite, write_lapped);
		
		ResetEvent(write_lapped.hEvent);
		SetEvent(hEvent);
	}
}

int main() 
{

write_lapped.hEvent = CreateEvent(NULL, true, false, NULL);
read_lapped.hEvent = CreateEvent(NULL, true, false, NULL);
hEvent = CreateEvent(NULL, false, false, NULL);

hDLLibrary = LoadLibrary(L"DLLibrary.dll");

if (!hDLLibrary)
{
	cout << "ErrorLoadLibraryDLL!" << endl;
	return 0;
}

DLLRead = (void (*) (HANDLE hFile, char *buffer, DWORD nNumber, DWORD dRead, OVERLAPPED OL_read))
            GetProcAddress(hDLLibrary, "asyncRead");
DLLWrite = (void (*) (HANDLE oFile, char *buffer, DWORD nNumber, DWORD dWrite, OVERLAPPED OL_write))
			GetProcAddress(hDLLibrary, "asyncWrite");

if (!DLLRead)
{
    cout << "ErrorDLLRead!" << endl;
    return 0;
}
else if(!DLLWrite)
{
	cout << "ErrorDLLWrite!" << endl;
	return 0;
}

thread_reader = CreateThread(NULL, 0, funcThreadReader, NULL, 0, NULL);
thread_writer = CreateThread(NULL, 0, funcThreadWriter, NULL, 0, NULL);

if(!SetCurrentDirectory(L"D:\\Direct"))	
{
	cout << "Error! Directory does not set!" << endl; 
	return 0;
}

if((oFile = CreateFile(L"OutFile.txt", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_DELETE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL)) == INVALID_HANDLE_VALUE)
	cout << "Error! File is not created/finded" << endl;

sFile = FindFirstFile(L".\\*.txt", &ffd);

if(sFile != INVALID_HANDLE_VALUE)
{
        do
		{
			hFile = CreateFile(ffd.cFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
		
			if(hFile == INVALID_HANDLE_VALUE)
			{
				if(GetLastError() == ERROR_SHARING_VIOLATION)
					break;
				
				else
				{
					cout << "Error! ReadFile is not opening!" << GetLastError() << endl;
					return 0;
				}
			}

			SetEvent(read_lapped.hEvent);		
			WaitForSingleObject(hEvent, INFINITE);

			puts(buffer); 
			memset(buffer, 0, NULL);
			
			SetEvent(write_lapped.hEvent);
			WaitForSingleObject(hEvent, INFINITE);
			write_lapped.Offset += strlen(buffer);
		}
		while(FindNextFile(sFile,&ffd) != 0 && ffd.cFileName != L"OutFile.txt");
		
		quit = true;
		SetEvent(read_lapped.hEvent);
		SetEvent(write_lapped.hEvent);
        FindClose(sFile);
}
else
{
	cout << "ErrorExtensionFile!" << endl;
	return 0;
}

puts("\nAsync operation is done!\nPress any but to check file: ");
getch();

system("cls");

CloseHandle(oFile);
CloseHandle(hFile);
CloseHandle(sFile);
CloseHandle(thread_reader);
CloseHandle(thread_writer);
CloseHandle(hEvent);
CloseHandle(read_lapped.hEvent);
CloseHandle(write_lapped.hEvent);

puts("Press any but to exit from the program");
getch();

DeleteFile(L"OutFile.txt");

FreeLibrary(hDLLibrary);
CloseHandle(hDLLibrary);

return 0;
}