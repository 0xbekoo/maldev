#include <ntddk.h>

#define FULL_DETOUR_SIZE			(sizeof(HkpDetour) + sizeof(PVOID))
#define INTERLOCKED_EXCHANGE_SIZE	(16ul)
#define HK_POOL_TAG					('  kh')

_IRQL_requires_max_(APC_LEVEL)
NTSTATUS HkDetourFunction(
	_In_ PVOID	 TargetFunction,
	_In_ PVOID	 Hook,
	_In_ SIZE_T  CodeLength,
	_Out_ PVOID* OriginalTrampoline
);

_IRQL_requires_max_(APC_LEVEL)
NTSTATUS HkRestoreFunction(
	_In_ PVOID	 HookedFunction,
	_In_ PVOID	 OriginalTrampoline
);

//
// jmp QWORD PTR [rip+0x0]
//
static const UCHAR HkpDetour[] = {
	0xff, 0x25, 0x00, 0x00, 0x00, 0x00
};

