#include <stdio.h>
#include <Windows.h>
#include <iostream>

//
// yeeee this should be a class or something. Ill rewrite all of these as classes... maybe
// Is doing it all in main bad? yes. Do I care? no
// Honestly not sure if ive done this properly, but will look again later
//

int main()
{
	printf("Private sharing\n");

	HANDLE hBoundaryDesc;
	//
	// Create our boundary descriptor
	//
	hBoundaryDesc = CreateBoundaryDescriptor(L"PrivateDescriptor", 0);
	if (NULL == hBoundaryDesc)
	{
		printf("Failed to create boundary descriptor\n");
	}

	BYTE sid[SECURITY_MAX_SID_SIZE];
	auto psid = reinterpret_cast<PSID>(sid);
	DWORD sidLength;
	//
	// Only admins can access the namespace
	//
	if (!CreateWellKnownSid(WinBuiltinAdministratorsSid, nullptr, psid, &sidLength))
	{
		printf("Failed to create well known sid\n");
	}

	if (!AddSIDToBoundaryDescriptor(&hBoundaryDesc, sid))
	{
		printf("Failed to add sid to boundary descriptor\n");
	}

	//
	// Now create the private namespace
	//
	HANDLE hPrivateNamespace;
	hPrivateNamespace = CreatePrivateNamespace(nullptr, hBoundaryDesc, L"MyPrivateNamespace");
	if (NULL == hPrivateNamespace)
	{
		//
		// Maybe we've already made it?
		//
		hPrivateNamespace = OpenPrivateNamespace(hBoundaryDesc, L"MyPrivateNamespace");
		if (NULL == hPrivateNamespace)
		{
			printf("Fuck knows mate cant create private namespace\n");
		}
	}

	printf("Made the namespace :)\n");

	//
	// Now do the shit from BasicSharing
	//
	HANDLE hSharedMem = nullptr;
	hSharedMem = CreateFileMapping(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, 4096, L"MyPrivateNamespace\\MySharedMem");
	if (NULL == hSharedMem)
	{
		printf("Failed to create file mapping\n");
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

			wcscpy_s((PWSTR)buf, 30, L"WriteOperation2"); // lolz v2

			UnmapViewOfFile(buf);
		}
		printf("Press 1 for read and 2 for write. 0 for exit...\n");
		std::cin >> i; //bleugh
	}


	//
	// Cleanup 
	//
	ClosePrivateNamespace(hPrivateNamespace, 0);
	DeleteBoundaryDescriptor(hBoundaryDesc);
}
