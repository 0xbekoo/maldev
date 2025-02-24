#include <ntddk.h>
#include <ntimage.h>
#include <windef.h>

#define IOCTL_TRAMPOLINE \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)

extern PVOID GetAddress(PVOID Address);
extern BOOLEAN ChangePreviousMode(int Mode);

NTSTATUS HookedNtLoadDriver(
	_In_ PVOID Buffer
);

PVOID GetSSDTAddress(
	_In_ PVOID KeAddSystemServiceAddr
);

NTSTATUS IoCreateClose(
	_In_ PDEVICE_OBJECT DeviceObject,
	_In_ PIRP Irp
);

NTSTATUS IoControl(
	_In_ PDEVICE_OBJECT DeviceObject,
	_In_ PIRP Irp
);

NTSTATUS(*OriginalNtLoadDriver)(
	_In_ PUNICODE_STRING DriverServiceName
);

typedef NTSTATUS(NTAPI* _NtLoadDriver)(
	_In_ PUNICODE_STRING DriverServiceName
);