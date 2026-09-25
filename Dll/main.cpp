#include "main.h"
#include "Worker.h"
#include "src/Console.h"

// A Safe thread where you can do your stuff
unsigned __stdcall MainThread(void* parameter)
{
    auto* worker = static_cast<leak::dll::Worker*>(parameter);

    //leak::console::Init();

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
