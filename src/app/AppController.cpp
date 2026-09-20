#include "AppController.h"

#include <exception>
#include <algorithm>
#include <string>
#include <utility>

AppController::AppController(AppState& state, injector::Injector& injector) : state(state), injector(injector)
{
}

void AppController::SetTarget(std::string target)
{
	state.target = std::move(target);
}

void AppController::SetDllPath(std::string dllPath)
{
	state.dllPath = std::move(dllPath);
	if (!state.dllPath.empty())
	{
		state.status = "File selected";
		injector.LoadDllPath(std::wstring(state.dllPath.begin(), state.dllPath.end()));
	}
}

void AppController::Dispatch(const UiAction action)
{
	switch (action)
	{
	case UiAction::ValidateTarget:
	{
			try
			{
				const DWORD pid = static_cast<DWORD>(std::stoul(state.target));
				injector.LoadProcessId(pid);
				state.status = "Process accepted";
			}
			catch (const std::exception&)
			{
				state.status = "Enter a valid process ID";
			}
			break;
		}

	case UiAction::SelectDll:
	{
			state.status = "Select the DLL file";
			break;
		}
	
	case UiAction::Inject:
	{
			if (state.busy)
				break;

			state.status = "Writing DLL path...";
			state.bytesWritten = 0;
			state.totalBytes = injector.TotalBytes();
			state.progress = 0.0f;
			state.busy = true;
			operationStarted = std::chrono::steady_clock::now();
			injectionTask = std::async(std::launch::async, [this] { return injector.Inject(); });
			break;
		}

	case UiAction::Clear:
		state.target.clear();
		state.targetName.clear();
		state.dllPath.clear();
		state.status = "Ready";
		state.bytesWritten = 0;
		state.totalBytes = 0;
		state.progress = 0.0f;
		break;

	case UiAction::Update:
	{
			if (!state.busy || !injectionTask.valid())
				break;

			const auto elapsed = std::chrono::duration<float>(std::chrono::steady_clock::now() - operationStarted).count();
			state.progress = (std::min)(0.88f, 0.10f + elapsed * 0.16f);
			state.status = elapsed < 0.7f ? "Preparing..." : "Writing DLL path...";

			if (injectionTask.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
			{
				const bool succeeded = injectionTask.get();
				state.bytesWritten = injector.BytesWritten();
				state.totalBytes = injector.TotalBytes();
				state.progress = succeeded ? 1.0f : 0.0f;
				state.busy = false;
				state.status = succeeded ? "Injected successfully" : "Injection failed";
			}
			break;
		}
	}
}
