#pragma once
#include <stdio.h>
#include <windows.h>

/* ==>  Defines  <== */

#define STATUS_SUCCESS (NTSTATUS)0x00000000L
#define okay(MSG, ...) printf("[+] " MSG "\n", ##__VA_ARGS__)
#define info(MSG, ...) printf("[*] " MSG "\n", ##__VA_ARGS__)
#define warn(MSG, ...) printf("[-] " MSG "\n", ##__VA_ARGS__)

#pragma region INJECTION GLOBALS
DWORD g_NtOpenProcessSSN;
DWORD g_NtAllocateVirtualMemorySSN;
DWORD g_NtWriteVirtualMemorySSN;
DWORD g_NtCreateThreadExSSN;
DWORD g_NtWaitForSingleObjectSSN;
DWORD g_NtCloseSSN;
#pragma endregion

/*  ==>  Parameters  <==*/

#pragma region STRUCTURES
typedef struct _PS_ATTRIBUTE
{
    ULONG  Attribute;
    SIZE_T Size;
    union
    {
        ULONG Value;
        PVOID ValuePtr;
    } u1;
    PSIZE_T ReturnLength;
} PS_ATTRIBUTE, * PPS_ATTRIBUTE;

typedef struct _UNICODE_STRING
{
    USHORT Length;
    USHORT MaximumLength;
    PWSTR  Buffer;
} UNICODE_STRING, * PUNICODE_STRING;

typedef struct _OBJECT_ATTRIBUTES
{
    ULONG           Length;
    HANDLE          RootDirectory;
    PUNICODE_STRING ObjectName;
    ULONG           Attributes;
    PVOID           SecurityDescriptor;
    PVOID           SecurityQualityOfService;
} OBJECT_ATTRIBUTES, * POBJECT_ATTRIBUTES;

#ifndef InitializeObjectAttributes
#define InitializeObjectAttributes( p, n, a, r, s ) { \
    (p)->Length = sizeof( OBJECT_ATTRIBUTES );        \
    (p)->RootDirectory = r;                           \
    (p)->Attributes = a;                              \
    (p)->ObjectName = n;                              \
    (p)->SecurityDescriptor = s;                      \
    (p)->SecurityQualityOfService = NULL;             \
}
#endif

typedef struct _CLIENT_ID
{
    HANDLE UniqueProcess;
    HANDLE UniqueThread;
} CLIENT_ID, * PCLIENT_ID;

typedef struct _PS_ATTRIBUTE_LIST
{
    SIZE_T       TotalLength;
    PS_ATTRIBUTE Attributes[1];
} PS_ATTRIBUTE_LIST, * PPS_ATTRIBUTE_LIST;
#pragma endregion
