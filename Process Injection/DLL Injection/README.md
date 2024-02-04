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
    First we need to start by compiling the Assembly project. Because we are externing the function we prepared with Assembly in the C project. We will compile this project and then convert it to dll.
</p>
