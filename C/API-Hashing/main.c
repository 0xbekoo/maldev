/*

>* API Hashing *<

@author:  0xCyberGenji 
@website: 0xCyberGenji.github.io
@warning: This project has been developed for educational purposes only. Its use in real scenarios is at one's own risk. 

*/
#include "utils.h"

DWORD GetHash(char* ApiString) {
	size_t	APILen	 = strnlen_s(ApiString, 50);
	DWORD	Hash	 = 0x75;
	int	x	 = 0;

	do {
		Hash += (Hash * 0xab19f29f + ApiString[x]) & 0xffffff;
		x++;
	} while (x < APILen);
	return Hash;
}

PDWORD GetFunctionAddress(char* ApiLibrary, DWORD hash)
{
	PDWORD functionAddress = (PDWORD)0;

	HMODULE LibraryAddress = LoadLibraryA(ApiLibrary);
	if (LibraryAddress == NULL) {
		printf("Failed to Load Library! Error Code: 0x%lx\n", GetLastError());
		return NULL;
	}
	PIMAGE_DOS_HEADER	DosHeader       = (PIMAGE_DOS_HEADER)LibraryAddress;
	PIMAGE_NT_HEADERS	ImageNTHeaders	= (PIMAGE_NT_HEADERS)((DWORD_PTR)LibraryAddress + DosHeader->e_lfanew);
	DWORD_PTR		ExportDirectory	= ImageNTHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
	PIMAGE_EXPORT_DIRECTORY ImageExportDirectory = (PIMAGE_EXPORT_DIRECTORY)((DWORD_PTR)LibraryAddress + ExportDirectory);

	PDWORD addresOfFunctionsRVA     = (PDWORD)((DWORD_PTR)LibraryAddress + ImageExportDirectory->AddressOfFunctions);
	PDWORD addressOfNamesRVA        = (PDWORD)((DWORD_PTR)LibraryAddress + ImageExportDirectory->AddressOfNames);
	PWORD  addressOfNameOrdinalsRVA = (PWORD)((DWORD_PTR)LibraryAddress + ImageExportDirectory->AddressOfNameOrdinals);

	for (DWORD i = 0; i < ImageExportDirectory->NumberOfFunctions; i++)
	{
		DWORD	  functionNameRVA  = addressOfNamesRVA[i];
		DWORD_PTR functionNameVA   = (DWORD_PTR)LibraryAddress + functionNameRVA;
		char*	  functionName	   = (char*)functionNameVA;
		DWORD_PTR functionAddressRVA = 0;

		DWORD functionNameHash = GetHash(functionName);
		if (functionNameHash == hash)
		{
			functionAddressRVA = addresOfFunctionsRVA[addressOfNameOrdinalsRVA[i]];
			functionAddress = (PDWORD)((DWORD_PTR)LibraryAddress + functionAddressRVA);
			printf("%s (0x00%x): %p\n", functionName, functionNameHash, functionAddress);
			return functionAddress;
		}
	}
	return NULL;
}
 
int main(int argc, char* argv[]) {
	DWORD  ApiHash	 = 0;
	DWORD  TID	 = 0;
	PDWORD FunctionAddress = NULL;

	ApiHash = GetHash("MessageBoxA");
	printf("0x00%x\n", ApiHash);

	FunctionAddress = GetFunctionAddress((char*)"user32", ApiHash);
	if (FunctionAddress == NULL) {
		printf("Failed to get Api Address!\n");
		return -1;
	}

	MyMessageBoxA MessageBoxA = (MyMessageBoxA)FunctionAddress;
	int result = MessageBoxA(NULL, "YEPUUUU THIS IS API HASHING!", "Message", MB_OK);
	if (result == 0) {
		printf("Failed to Show Message Box! Error Code: 0x%lx\n", GetLastError());
		return -1;
	}
	printf("Message Box Shown!\n");
	return 0;
}
