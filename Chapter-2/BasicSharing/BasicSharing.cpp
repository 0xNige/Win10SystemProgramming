#include <iostream>
#include <Windows.h>

/*
* This code is ass. 
* Literally just threw together as an example. 
* Might fix later, might not.
*/

int main()
{
    printf("Basic Sharing program...");

    HANDLE hSharedMem = nullptr;

    hSharedMem = CreateFileMapping(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, 4096, L"MySharedMem");

    if (NULL != hSharedMem)
    {
        printf("Failed to create shared memory\n");
    }

    USHORT i = 0;
    printf("Press 1 for read and 2 for write. 0 for exit...\n");
    std::cin >> i;
    while (0 != i)
    {
        if (1 == i)
        {
            // read
            void* buf = MapViewOfFile(hSharedMem, FILE_MAP_READ, 0, 0, 0);
            if (NULL == buf)
            {
                printf("Failed to map memory\n");
                return 1;
            }

            printf("string: %ws\n", buf); // lolz

            
            UnmapViewOfFile(buf);
        }
        if (2 == i)
        {
            // write
            void* buf = MapViewOfFile(hSharedMem, FILE_MAP_WRITE, 0, 0, 0);
            if (NULL == buf)
            {
                printf("Failed to map memory\n");
                return 1;
            }

            wcscpy_s((PWSTR)buf, 29, L"WriteOperation"); // lolz v2

            UnmapViewOfFile(buf);
        }
        printf("Press 1 for read and 2 for write. 0 for exit...\n");
        std::cin >> i; //bleugh
    }


    //
    // cleanup
    //
    if (NULL != hSharedMem)
    {
        CloseHandle(hSharedMem);
        printf("Closed handle to shared memory\n");
    }
}