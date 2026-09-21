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
        // write here

        Sleep(10);
    }

    //leak::console::Close();

    return 0;
}
