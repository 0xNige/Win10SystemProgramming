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

This is the simplest form of sharing if its available. The cooperating processes would need to call a `Create` function using the same object name. The first call would create the object, and the second call would open a handle to the object. 
An example of this would be creating a driver then opening it to use it or [BasicSharing](BasicSharing/BasicSharing/BasicSharing.cpp). Excuse the poor code.

### Sharing by Handle Inheritance



### Sharing by Handle Duplication

Handle duplication has no inherent restrictions, except security, and can work on almost nay kernel object at any point in time. However this can end up being rather difficult.
You use the `DuplicationHandle` function to duplicate a handle, funnily enough.
It requires a source process, source handle and a target process. If this is successful, a new handle entry is written into the target processes handle table, pointing to the same object as the source handle.
A typical scenario for this is duplicating one of the callers handles into another process' handle table, but the source and target processes can be the same.
It is possible to create an object a level of access, and then duplicate the handle with a different level of access. 
```
HANDLE h1 = CreateSomeObject(FULL_ACCESS) // obviously not a real function
HANDLE h2;
DuplicateHandle(GetCurrentProcess(), h1, GetCurrentProcess(), &h2, READ_ACCESS)
```
That should, when written in proper C(++) with proper functions, will get a full access handle, then duplicate it with only read access
This example is rather simple. It gets complicated when trying to copy a handle into another process. 
The duplication part is simple, but telling the other process this has happened and how to get the handle is more complicated.
To do this, we'd need some form of IPC, which im not gonna go into here.

## Private Object Namespaces

Sharing objects by name is great, until some pesky code comes along and tries to access our object using the same name.
This may be by accident or malicious but either way, it'll either fail or they;ll get a legit handle back to our object.
I don't need to go into why this is bad.
We can use private namespaces to sort this. These prevent the full name being revealed.
Creating a private namespace is a two step process:
1. Create a helper object called a boundary descriptor.
   This allows adding certain SIDs that are allows to use the private namespace based on the descriptor 
2. Create the private 

See [PrivateSharing](PrivateSharing/PrivateSharing.cpp) for a, somewhat shitty, example of this.


### Other objects

Kernel objects are the main focus here but there are some other common objects used in windows.

## User objects

- Windows
- Menus
- hooks

Handles to these objects share the following attributes
- No ref counting. If destroyed by anyone, gone.
- Scoped under a windows station. Windows station contains clipboard, desktops and atom table. This means handles to these objects can be passed freely among all applications sharing a desktop.

## GDI objects

The Graphics Device Interface is the original graphics API used in Windows and is still used today.
again:
- no ref counting
- handles only valid in the process they're created
- cant be shared between processes

