#include <stdio.h>
#include <Windows.h>

#define DEVICE_NAME L"\\\\.\\MyDriver"

#define IOCTL_TRAMPOLINE \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef struct _UNICODE_STRING
{
	USHORT Length;
	USHORT MaximumLength;
	_Field_size_bytes_part_opt_(MaximumLength, Length) PWCH Buffer;
} UNICODE_STRING, * PUNICODE_STRING;

FORCEINLINE VOID RtlInitUnicodeString(
	_Out_ PUNICODE_STRING DestinationString,
	_In_opt_z_ PCWSTR SourceString
)
{
	if (SourceString)
		DestinationString->MaximumLength = (DestinationString->Length = (USHORT)(wcslen(SourceString) * sizeof(WCHAR))) + sizeof(UNICODE_NULL);
	else
		DestinationString->MaximumLength = DestinationString->Length = 0;

	DestinationString->Buffer = (PWCH)SourceString;
}

typedef NTSTATUS(NTAPI* RtlAppendUnicodeToString)(
	_Inout_ PUNICODE_STRING Destination,
	_In_opt_z_ PCWSTR Source
);

typedef NTSTATUS(NTAPI* NtLoadDriver)(
	PUNICODE_STRING DriverServiceName
);