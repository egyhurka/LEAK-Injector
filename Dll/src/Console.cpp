#include "Console.h"

#include <cstdio>
#include <Windows.h>

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