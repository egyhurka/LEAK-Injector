#include "Worker.h"

#include <process.h>

leak::thread::Worker::~Worker()
{
	Stop();
}

bool leak::thread::Worker::Start(ThreadFunction workerFunction)
{
    if (workerFunction == nullptr)
        return false;

    if (running.exchange(true))
        return false;

    function = workerFunction;

    stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);

    if (stopEvent == nullptr)
    {
        running = false;
        return false;
    }

    const uintptr_t handle = _beginthreadex(nullptr, 0, &Worker::ThreadEntry, this, 0, nullptr);

    if (handle == 0)
    {
        CloseHandle(stopEvent);
        stopEvent = nullptr;
        running = false;
        return false;
    }

    threadHandle = reinterpret_cast<HANDLE>(handle);

    return true;
}

bool leak::thread::Worker::Stop(DWORD timeoutMs)
{
    if (!running.exchange(false))
        return true;

    if (stopEvent != nullptr)
        SetEvent(stopEvent);

    if (threadHandle != nullptr)
    {
        const DWORD result = WaitForSingleObject(threadHandle, timeoutMs);

        if (result != WAIT_OBJECT_0)
            return false;

        CloseHandle(threadHandle);
        threadHandle = nullptr;
    }

    if (stopEvent != nullptr)
    {
        CloseHandle(stopEvent);
        stopEvent = nullptr;
    }

    return true;
}

bool leak::thread::Worker::IsRunning() const noexcept
{
    return running.load();
}

bool leak::thread::Worker::StopRequested() const noexcept
{
    return stopEvent != nullptr &&
        WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0;
}

unsigned __stdcall leak::thread::Worker::ThreadEntry(void* parameter)
{
    auto* worker = static_cast<Worker*>(parameter);

    if (worker->function != nullptr)
        return worker->function(worker);

    return 0;
}
