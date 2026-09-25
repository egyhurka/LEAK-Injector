#include "Console.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <cstdio>

namespace leak::console {
    FILE* file = nullptr;

    void Init()
    {
        if (file != nullptr) {
            fclose(file);
            file = nullptr;
        }
        
        AllocConsole();
        freopen_s(&file, "CONOUT$", "w", stdout);
    }

    void Close()
    {
        if (file != nullptr) {
            fclose(file);
            file = nullptr;
        }
        FreeConsole();
    }
}