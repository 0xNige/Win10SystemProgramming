#include <iostream>
#include <Windows.h>

int main()
{
	SYSTEM_INFO si;
	::GetNativeSystemInfo(&si);

	printf("GetNativeSystemInfo:\n");
	printf("Number of logical processors: %d\n", si.dwNumberOfProcessors);
	printf("Page size (bytes):            %d\n", si.dwPageSize);
	printf("Processor mask:               0x%p\n", (PVOID)si.dwActiveProcessorMask);
	printf("Minimum process address:      0x%p\n", si.lpMinimumApplicationAddress);
	printf("Maximum process address:      0x%p\n", si.lpMaximumApplicationAddress);

	printf("\n");
	TCHAR NameBuffer[MAX_COMPUTERNAME_LENGTH+1];
	DWORD NameSize = sizeof(NameBuffer) / sizeof(NameBuffer[0]);
	::GetComputerName(NameBuffer, &NameSize);
	printf("GetComputerName:\n");
	printf("Computer name: %ws\n", NameBuffer);

	printf("\n");
	TCHAR DirectoryBuffer[MAX_PATH + 1];
	UINT DirSize = sizeof(DirectoryBuffer) / sizeof(DirectoryBuffer[0]);
	UINT ret = ::GetWindowsDirectoryW(DirectoryBuffer, DirSize);
	if (0 == ret)
	{
		return 1;
	}
	printf("GetWindowsDirectory:\n");
	printf("Directory: %ws\n", DirectoryBuffer);

	printf("\n");
	LARGE_INTEGER PerfCount;
	::QueryPerformanceCounter(&PerfCount);
	printf("QueryPerformanceCounter\n");
	printf("PerfCount QuadPart: %lld\n", PerfCount.QuadPart);

	// The remaining exercises for this section were GetVersionInfo (Depreciated function) and something else. A lot of typing...

	return 0;
}
