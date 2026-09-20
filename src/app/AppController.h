#pragma once

#include <string>
#include <future>
#include <chrono>

#include "AppState.h"
#include "Injector/Injector.h"

class AppController
{
public:
	explicit AppController(AppState& state, injector::Injector& injector);

	enum class UiAction
	{
		ValidateTarget,
		SelectDll,
		Inject,
		Clear,
		Update
	};

	void Dispatch(UiAction action);
	void SetTarget(std::string target);
	void SetDllPath(std::string dllPath);

private:
	AppState& state;
	injector::Injector& injector;
	std::future<bool> injectionTask;
	std::chrono::steady_clock::time_point operationStarted{};
};
