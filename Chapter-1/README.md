# Chapter 1: Foundations

## Processes

A process is a container that represents a running instance of the a program. Processes don't run, they manage. Threads run code.
A process owns the following:

- Executable program, containing the code and data used to execute code within the process.
- Private virtual address space used for allocating memory.
- Access token storing the default security context for the process.
- Handle table to executive (kernel) objects such as events, semaphores and files.
- One or more threads of execution. Without one its useless and kernel will most likely kill it.

Processes are uniquely identified by their process ID, which remains as long as the kernel process object exists.
When destroyed, the pid can be reused for new processes. 
The exe itself isn't uniquely identifiable, as you could have multiple instances of the same program running. 
Each process will have its own address space, access token, threads etc but will use the same image file(.exe) as their initial code/data.

## Dynamic Link Libraries

DLLs are executable files that contain at least one of the following: code, data or resources.
They are loaded into a process either at process initialization time (static linking) or when requested (dynamic linking).
DLLs don't contain main functions so cant be run directly but allow for sharing of their code in physical memory between multiple processes.
See the MathClient/Server folder.

## Virtual Memory

Every process has its own virtual, private and linear address space. This space starts out close to empty, the executable image and ntdll are normally first to be mapped.
Since this is private, other processes shouldn't have access to this space.
The size of this space is dependant on the bitness of the OS.

- 2Gb for 32 bit processes on 32 bit systems
- 3Gb for 32 bit processes on 32 bit systems with the LARGEADDRESSAWARE linker flag
- 8Tb for 64 bit processes on 64 bit systems
- 4Gb for 32 bit processes on 64 bit systems with the LARGEADDRESSAWARE linker flag. If not, 2Gb.

As the name states, this is virtual so there is an indirect relationship between an address range and the exact location in RAM.
An allocated buffer may reside in physical memory, or it could be mapped out to a page file. There is no reason that the program needs to know this.
If the memory is in RAM when it needs to be accessed, the CPU will access the data. If not it will raise a page fault that causes the memory managers page fault handler
to fetch the data from the appropriate page file, copy it to RAM and make the necessary changes to the page table entries that map the buffer, and tell the CPU to try again.

## Threads

This is where the execution begins :)/ Threads are contained within processes and use the resources from the process to do work.
Important things the thread owns:

- Current access mode, user or kernel
- Execution context
- Stack
- Thread Local Storage (TLS) array. Provides storage of thread private data
- Base priority and current (dynamic) priority
- Processor affinity. Indicates with processors the thread is allowed to run on

Threads can be in many states, but the main ones are:

- Running - currently executing code
- Ready - Waiting to execute code
- Waiting - Waiting on some event to occur before doing work. Once the event occurs, moves to ready

## General System Architecture

Windows is roughly made up of the following:

- User processes: normal processes, notepad cmd etc etc.
- Subsystem DLLs: libs that implement the API of the subsystem, kernel32.dll, user32.dll etc etc.
- NTDLL.dll: System wide DLL implementing the Windows native API. Lowest level of code in user mode. Manages the transition from user mode to kernel mode. Also implements the heap manager, image loader and some of the user mode thread pool.
- Service processes: processes that communicate with the Service Control Manager (scm)
- Executive: upper layer of ntoskrnl. Hosts most of the kernel code. Includes most of the managers; Object/memory/IO/... manager
- Kernel: implements fundamental and time sensitive parts of kernel OS code. Includes thread scheduling, interrupt/exception dispatching, and implementation of mutex/semaphores. Some of this is written in CPU-specific machine code.
- Device drivers: loadable kernel modules. Run in kernel mode and have full power of the kernel. Usually provide for some hardware and the rest of the OS. These are covered more in the KernelProgramming part of my git(soon?).
- win32k.sys: kernel mode component of the Windows subsystem. Handles the UI part of windows and graphic device interface(GDI) APIs.
- Hardware Abstraction Layer: as the name suggests; abstraction layer over hardware closest to the CPU. Allows device drivers to use APIs that don't require detailed/specific knowledge of things like interrupt controller or DMA controller.
- System processes: important processes that do a range of things. smss, lsass, winlogon etc etc
- Subsystem process: csrss - can be viewed as helper to kernel for managing processes running under Windows. Critical so bsod if killed. One per session, so session 0 and 1 for logged on user (so 2). Csrss does more than this, but thats out of scope here.
- Hyper-V hypervisor: Win10/Server2016+ if they support Virtualization Based Security (VBS). This provides an extra layer of security where the actual machine is a VM controller by hyper-v. More on this in Windows Internals.
