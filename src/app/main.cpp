#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "File/File.h"
#include "Injector/Injector.h"
#include "ui/Window/Window.h"
#include "AppController.h"
#include "AppState.h"

bool EnableDebugPrivilege();

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int)
{
    bool pr = EnableDebugPrivilege();

    if (!pr) 
        //MessageBoxA(NULL, "Failed to adjust token privileges (Are you running as Administrator?).", "Warning", MB_OK | MB_ICONWARNING);

	// Clean up leftover DLL copies
	injector::file::ClearTempDirectory();

	AppState state;
	injector::Injector injector;
	AppController controller(state, injector);
	Window window(controller, state);
	window.Init();
	window.Create(960, 620);
	window.Frame();

	return EXIT_SUCCESS;
}

bool EnableDebugPrivilege() {
    HANDLE hToken = NULL;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) 
        return false;

    TOKEN_PRIVILEGES tp;
    LUID luid;
    if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid)) 
    {
        CloseHandle(hToken);
        return false;
    }

    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    bool success = AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), (PTOKEN_PRIVILEGES)NULL, (PDWORD)NULL);

    CloseHandle(hToken);
    return success;
}