#include <Windows.h>
#include <ctime>
#include <cstdio>
    time_t systemStartTime = time(NULL) - (GetTickCount() / 1000);
    struct LOG_ENTRY;
    struct LOG_ENTRY
    {
    CHAR title[256];
    CHAR exename[256];
    time_t time;
    LOG_ENTRY* pnext;
	} ;
    LOG_ENTRY* head = new LOG_ENTRY;
    LOG_ENTRY* tail = head;
    HANDLE reportfile = CreateFile("report.txt", GENERIC_READ | GENERIC_WRITE, NULL, NULL, OPEN_ALWAYS, NULL, NULL);

VOID CALLBACK WinEventProcCallback(HWINEVENTHOOK hWinEventHook, DWORD dwEvent, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime)
{

    DWORD exenamesize = 256;
    GetWindowText(hwnd, head->title, 256);
    DWORD pid = 0;
    BOOL fRc = FALSE;
    if (GetWindowThreadProcessId(hwnd, &pid)) {
        HANDLE hProcess = OpenProcess(
            PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
        if (hProcess) {
            fRc = QueryFullProcessImageName(
                hProcess, NULL, head->exename, &exenamesize);
            CloseHandle(hProcess);
        }
        head->time = systemStartTime + dwmsEventTime / 1000;
        char line[1024];
        DWORD len = sprintf_s(line, "%s %s %d \n", head->title, head->exename, head->time);
        DWORD written;
        WriteFile(reportfile, line, len, &written, NULL);


    }
    head->pnext = new LOG_ENTRY;
    head = head->pnext;
}
int main(int, char**)
{
    MSG m;
    SetFilePointer(reportfile, 0, 0, FILE_END);
    SetWinEventHook(EVENT_SYSTEM_FOREGROUND,
        EVENT_SYSTEM_FOREGROUND, NULL,
        WinEventProcCallback, 0, 0,
        WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
    while (GetMessage(&m, NULL, NULL, NULL));
    CloseHandle(reportfile);
}
