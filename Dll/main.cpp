#include "main.h"
#include "Worker.h"
#include "src/Console.h"
#include "src/Mem.h"

using namespace leak;

// A Safe thread where you can do your stuff
unsigned __stdcall MainThread(void* parameter)
{
    auto* worker = static_cast<thread::Worker*>(parameter);

    //leak::console::Init();

    uintptr_t moduleBase = mem::GetModuleBase();

    while (!worker->StopRequested())
    {
        // EXIT (Terminate Thread, the dll stays loaded!)
        if (GetAsyncKeyState(VK_F9) && 0x800)
            break;



        Sleep(10);
    }

    worker->Stop();
    //leak::console::Close();

    return 0;
}
