## `⚠️` Malware Development 

<img src="https://github.com/x1nerama/maldev/blob/main/gif/video.gif?raw=true">

This repo contains projects I have prepared for malware development. I am new to malware development and I will add every project I make to this repo to improve myself. Projects in this repository:

- `🟢` [**DLL Injection**](https://github.com/x1nerama/maldev/tree/main/Process%20Injection/DLL%20Injection)
- `🛑` **Thread Hijacking**
- `🟢` [**Using NTDLL**](https://github.com/x1nerama/maldev/tree/main/Process%20Injection/NTAPI%20Injection)
    - `🟢` [Execute Payload with NTAPI](https://github.com/x1nerama/maldev/tree/main/Process%20Injection/NTAPI%20Injection/Execute%20Payload%20with%20NTAPI)
    - `🟢` [Execute DLL with NTAPI](https://github.com/x1nerama/maldev/tree/main/Process%20Injection/NTAPI%20Injection/Execute%20DLL%20with%20NTAPI) 
- `🛑` **Using Direct System Calls**

## `💻` NOTE

In some projects you will also see Assembly projects. You can use the appropriate syntax below to run these projects:

```shell
gcc -o <project> <project.c> -L. -l<dllName (without .dll)>
```

For example, if you want to compile and run the [**DLL Injection**](https://github.com/x1nerama/maldev/tree/main/Process%20Injection/DLL%20Injection) project, you need to run the following code:

```shell
gcc -o injection injection.c -L. -lexecution
```

In order to use the functions I created with assembly in C projects, I convert assembly projects into dlls and include them as libraries during compilation.

So you don't need to compile the assembly projects separately. You only need to compile the main C project with the dll. 

## `⚠️` WARNING
> The projects shared here are purely for educational and informative purposes. While developing the projects, I develop them in a way that can prevent illegal situations and I do not condone the use of these programs for illegal activities.
