# Chapter 2 - Objects and Handles

## Kernel Objects

The kernel exposes a range of objects that user mode processes, kernel code and device drivers can use.
These are data structures held in kernel space and are managed by the Object Manager.
These objects are reference counted so when the last reference is released the object is destroyed/freed.
You can see these objects using the `WinObj` tool in sysinternals.

The main attributes of a kernel object are:

- Name
- Directory
- Handle count; Usually user mode references
- Pointer count; Usually pointers in kernel code

Kernel objects reside in the kernel therefore cant be directly accessed by user mode code. Applications must use indirect mechanisms, known as handles, in order to access these objects. Handles provide a number of benefits, but basically means if the object type in kernel is changed later on our code should be fine, and security access checks.

Kernel objects are reference counted and the object manager maintains a handle and pointer count, where the sum is the total ref count for an object. The pointer count will be incremented when a direct pointer is obtained from kernel code. When user mode code is finished with the handle to the object, it should call ```CloseHandle()```. If this causes the object count to drop to 0, the Object Manager will delete the object.

A handle is logically a index into an array of entries into a handle table maintained on a process by process basis, that points to a kernel object residing in kernel space.

## Handles

A handle points to a small data structure in kernel that holds information for that handle.
```
31                        2 1 0 
[ ptr to obj header      |A I P]
[         Access mask          ]        A = Audit on close
                                        I = Inheritance flag
63                        2 1 0         P = Protect on close
[ ptr to obj header      |A I P]
            [    Access mask   ]
```

On 32 bit the handle entry is 8 bytes, and on 64 its 16 bytes. Each entry has a pointer to the actual object, an access mask and three flag fields.
The access mask is a bitmask, where each 1 indicates a certain operation that can be carried out using that handle. This is set when the handle is created by creating or opening the object. eg if you want to open a process and query it, you'd use the PROCESS_QUERY_INFORMATION flag. These can all be viewed in Process Explorer/Hacker.
Some handles have special values and aren't closable. These are psuedo handles. Functions that return these are:

- GetCurrentProcess()
- GetCurrentThread()
- GetCurrentProcessToken()
- GetCurrentThreadToken()
- GetCurrentThreadEffectiveToken()

## Object Names

Some objects can have string based names, and these can be used with suitable `Open...` functions to open objects. Note that some don't like this, such as `OpenProcess()` where a PID is required. From user mode, calling a `Create...` function with a name will create the object if it doesn't exist, but if it exists it will open the existing object and return a handle to the object and `GetLastError()` will contain ERROR_ALREADY_EXISTS. The name passed to the create function isn't the final name given to the object. On a Windows desktop the object name is prepended with `\Sessions\x\BaseNamedObjects\` where x is the session ID of the caller. If its session 0, its prepended with `\BaseNamedObjects\` only. If the caller is in an AppContainer then the prepended string contains their unique AppContainer SID and `\Sessions\x\AppContainerNamedObjects`. You can view these objects using Process Explorer/Hacker.

## Sharing Kernel Objects

Kernel objects are private to a process and therefore you cant simply pass handles between processes. This is because handles are kept in the processes handle table, and passing the handle to another process may result in the handle pointing to a different object in the receiving process handle table. To share handles between processes, the following mechanisms were implemented:

- Share by name
- Share by handle inheritance
- Share by handle duplication

### Sharing by name

This is the simplest form of sharing if its available. The cooperating processes would need to call a `Create` function both using the same object name. The first call would create the object, and the second call would open a handle to the object. An example of this would be creating a driver then opening it to use it.

### Sharing by Handle Inheritance



### Sharing by Handle Duplication

