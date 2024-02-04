<div align="center">
    <h2> 💀💉 DLL Injection </h2>
</div>

<p style="text-align: justify">
    Welcome to the DLL Injection section! This repository showcases projects that demonstrate a straightforward implementation of DLL Injection using Assembly and C. <br/>
</p>

<h3> What is DLL Injection? </h3>
<p style="text-align: justify">
    DLL Injection is a technique in software development where a dynamic-link library (DLL) is forcibly inserted into the address space of a running process. This method allows the injected DLL to execute code within the 
context of the target process, enabling various capabilities and modifications.
</p>

<p style="text-align: justify"> 
    In this code, the target process is opened in the C project, then memory is allocated and finally the path to the DLL is added to that memory. Finally, this DLL is executed with CreateRemoteThread with Assembly. 
</p>

<h3> Preparing Projects </h3>

<p style="text-align: justify"> 
    You don't have to do much as I've arranged everything. Just run the following code with gcc:
</p>

```shell
gcc -o injection main.c -L./dll -lexecution
```

<p>
    This code will quickly compile the dll with the c project and give you a file called injection.exe.
</p>

<p>
    If you don't have gcc in a Windows environment, you can download scoop by <a href="https://scoop.sh/">clicking here</a> and download gcc.
</p>
