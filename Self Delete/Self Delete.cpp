#include <windows.h>
#include <tchar.h>

#pragma pack(push, 1)
#define CODESIZE 0x200
typedef struct _SELFDEL
{
    struct _SELFDEL *Arg0;

    BYTE    opCodes[CODESIZE];

    HANDLE  hParent;

    FARPROC fnWaitForSingleObject;
    FARPROC fnCloseHandle;
    FARPROC fnDeleteFile;
    FARPROC fnSleep;
    FARPROC fnExitProcess;
    FARPROC fnRemoveDirectory;
    FARPROC fnGetLastError;

    BOOL    fRemDir;

    TCHAR   szFileName[MAX_PATH];

} SELFDEL;
#pragma pack(pop)

#ifdef _DEBUG
#define FUNC_ADDR(func) (PVOID)(*(DWORD *)((BYTE *)func + 1) + (DWORD)((BYTE *)func + 5))
#else
#define FUNC_ADDR(func) func
#endif

static void remote_thread(SELFDEL *remote)
{
    ((DWORD (WINAPI *)(HANDLE, DWORD))remote->fnWaitForSingleObject)(remote->hParent, INFINITE);
    ((DWORD (WINAPI *)(HANDLE))remote->fnCloseHandle)(remote->hParent);

    while(!((BOOL (WINAPI *)(LPCWSTR))remote->fnDeleteFile)(remote->szFileName))
    {
        ((void (WINAPI *)(DWORD))remote->fnSleep)(1000);
    }

    ((void (WINAPI *)(UINT))remote->fnExitProcess)(0);
}

BOOL SelfDelete(BOOL fRemoveDirectory)
{
#ifdef _DEBUG
    return TRUE;
#endif

    STARTUPINFO         si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    CONTEXT             context;
    DWORD               oldProt;
    SELFDEL             local;
    DWORD               entrypoint;

    TCHAR               szExe[MAX_PATH] = _T("explorer.exe");

    if(CreateProcess(0, szExe, 0, 0, 0, CREATE_SUSPENDED|IDLE_PRIORITY_CLASS, 0, 0, &si, &pi))
    {
        local.fnWaitForSingleObject     = (FARPROC)WaitForSingleObject;
        local.fnCloseHandle             = (FARPROC)CloseHandle;
        local.fnDeleteFile              = (FARPROC)DeleteFile;
        local.fnSleep                   = (FARPROC)Sleep;
        local.fnExitProcess             = (FARPROC)ExitProcess;
        local.fnRemoveDirectory         = (FARPROC)RemoveDirectory;
        local.fnGetLastError            = (FARPROC)GetLastError;

        local.fRemDir                   = fRemoveDirectory;

        DuplicateHandle(GetCurrentProcess(), GetCurrentProcess(),
            pi.hProcess, &local.hParent, 0, FALSE, 0);

        GetModuleFileName(0, local.szFileName, MAX_PATH);

        memcpy(local.opCodes, FUNC_ADDR(remote_thread), CODESIZE);

        context.ContextFlags = CONTEXT_INTEGER|CONTEXT_CONTROL;
        GetThreadContext(pi.hThread, &context);

        entrypoint = (context.Esp - sizeof(SELFDEL)) & ~0x1F;

        local.Arg0 = (SELFDEL *)entrypoint;

        context.Esp = entrypoint - 4;
        context.Eip = entrypoint + 4;

        VirtualProtectEx(pi.hProcess,   (PVOID)entrypoint, sizeof(local), PAGE_EXECUTE_READWRITE, &oldProt);
        WriteProcessMemory(pi.hProcess, (PVOID)entrypoint, &local, sizeof(local), 0);

        FlushInstructionCache(pi.hProcess, (PVOID)entrypoint, sizeof(local));

        SetThreadContext(pi.hThread, &context);

        ResumeThread(pi.hThread);
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);

        return TRUE;
    }

    return FALSE;
}


int _tmain(int argc, _TCHAR* argv[])
{
    if (argc == 2 && _tcsicmp(argv[1], _T("delete")) == 0)
    {
        SelfDelete(FALSE);
    }
    else
    {
        _tprintf(_T("Call with \"delete\" paramater to have this executable delete itself\r\n"));
    }
    return 0;
}

