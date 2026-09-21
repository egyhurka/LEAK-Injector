// dllmain.cpp : Defines the entry point for the DLL application.
#include "framework.h"
#include "main.h"
#include "src/Worker.h"

namespace
{
    leak::dll::Worker worker;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
        {
            DisableThreadLibraryCalls(hModule);

            worker.Start(MainThread); // main.cpp
            break;
        }

        case DLL_THREAD_ATTACH:
        {

            break;
        }

        case DLL_THREAD_DETACH:
        {
            // Prepare for the dll to be removed
            break;
        }

        case DLL_PROCESS_DETACH:
        {

            break;
        }
    }

    return TRUE;
}
