#include <iostream>
#include <Windows.h>

int main(int argc, const char* argv[])
{
    if (argc < 2)
    {
        printf("Usage: ShowError <number>\n");
        return 1;
    }

    int message = atoi(argv[1]);

    LPWSTR text;
    DWORD chars = ::FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, // Let the function allocate for us
        nullptr, message, 0,
        (LPWSTR)&text, // eugh
        0, nullptr
    );

    if (chars > 0)
    {
        printf("Message %d: %ws\n", message, text);
        ::LocalFree(text);
    }
    else
    {
        printf("No error exists\n");
    }

    return 0;
}
