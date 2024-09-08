## `⚠️` Malware Development 

<img src="https://github.com/x1nerama/maldev/blob/main/gif/video.gif?raw=true">

Welcome to the maldev repo. This repository contains projects I have prepared as part of my journey into malware development. As a beginner in this field, I will be continuously adding new projects to this repo as I learn and develop my skills. 

> [!Warning]
> The content of this repository is intended for educational purposes only. I do not encourage or endorse any illegal activity or malicious use of the projects contained here. Please ensure you adhere to all applicable laws and ethical guidelines when experimenting with or studying this material.

The projects in this repository are organized into two categories: ASM and C/C++. Each section contains various malware development projects that focus on different techniques and concepts using these programming languages:

### ASM 
- `🟩` [**Using Direct Syscalls**](https://github.com/x1nerama/maldev/tree/main/ASM/Using%20Direct%20Syscalls)
- `🟩` [**NTAPI Injection**](https://github.com/x1nerama/maldev/tree/main/ASM/NTAPI%20Injection)
- `🟩` [**DLL Injection**](https://github.com/x1nerama/maldev/tree/main/ASM/DLL%20Injection)

### C/C++
- `🟩` [**Malware Resurrection**](https://github.com/0xbekoo/maldev/tree/main/C/Malware-Resurrection)
  - `🟩` [**User-mode Program**](https://github.com/0xbekoo/maldev/tree/main/C/Malware-Resurrection/user_mode)
  - `🟩` [**Kernel-mode Driver**](https://github.com/0xbekoo/maldev/tree/main/C/Malware-Resurrection/kernel_mode)
- `🟩` [**File Icon Spoofing**](https://github.com/0xbekoo/maldev/tree/main/C/File-Icon-Spoofing)
- `🟩` [**API Hashing**](https://github.com/x1nerama/maldev/tree/main/C/API-Hashing)
- `🟩` [**Threadhijacking**](https://github.com/x1nerama/maldev/tree/main/C/ThreadHijacking)
- `🟩` [**Using Direct Syscalls**](https://github.com/x1nerama/maldev/tree/main/C/Direct-Syscall)
- `🟩` [**NTAPI Injection**](https://github.com/x1nerama/maldev/tree/main/C/NTAPI-Injection)
- `🟩` [**Shellcode Injection**](https://github.com/x1nerama/maldev/tree/main/C/Shellcode-Injection)
- `🟩` [**DLL Injection**](https://github.com/x1nerama/maldev/tree/main/C/DLL-Injection)

## Compiling and running projects

Assuming that you have development tools like Visual Studio, I have not included executable files (e.g., .exe or .sys). You will need to compile the projects yourself using the provided source code.

### 1. ASM Projects

You need to make sure you have a Windows 10/11 x64 system, especially if you want to run Assembly programs. 

To run the assembly programs, you must have a Windows 10/11 x64 system. Follow these steps to turn the assembly projects into executable files:
- In Visual Studio, right-click on the project.
- Navigate to **Build Dependencies** -> **Build Customization** and enable **masm(.targets, .props)**.
- After enabling MASM, create a project in the solution with files that have `.asm` and `.inc` extensions.
- Copy the provided assembly code into these files.

### 2. C/C++ Projects

To work with the C/C++ projects, ensure you have Visual Studio or a compatible development environment set up. Follow these steps to build and run the projects:

- Create projects with .c and .h extensions and copy the code.
- Build the project by selecting the appropriate build configuration (e.g., Debug or Release) and compiling the code.
- After a successful build, you can run the resulting executable to test the project.
