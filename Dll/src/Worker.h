#pragma once

#include <Windows.h>
#include <atomic>

namespace leak::dll
{
    class Worker final
    {
    public:
        Worker() = default;
        ~Worker();

        Worker(const Worker&) = delete;
        Worker& operator=(const Worker&) = delete;

        using ThreadFunction = unsigned(__stdcall*)(void*);

        bool Start(ThreadFunction function);
        bool Stop(DWORD timeoutMs = 5000);

        [[nodiscard]] bool IsRunning() const noexcept;
        [[nodiscard]] bool StopRequested() const noexcept;

    private:
        static unsigned __stdcall ThreadEntry(void* parameter);

        HANDLE threadHandle = nullptr;
        HANDLE stopEvent = nullptr;

        std::atomic_bool running = false;
        ThreadFunction function = nullptr;
    };
}
