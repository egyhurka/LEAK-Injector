#pragma once

#include <SDL3/SDL.h>

#include <array>
#include <mutex>
#include <string>
#include <vector>

#include "app/AppController.h"
#include "Process/Process.h"

class InjectorView
{
public:
	InjectorView(SDL_Window* window, AppController& controller, AppState& state);
	void Render(bool& shouldClose);

private:
	static void SDLCALL OnFileSelected(void* userdata, const char* const* fileList, int filter);
	void OpenModFileDialog();
	void ApplySelectedFile();
	void RefreshProcesses();
	void RenderProcessFinder();

	SDL_Window* window;
	std::array<char, 520> dllPathInput{};
	std::array<char, 260> targetInput{};
	AppController& controller;
	AppState& state;
	std::mutex selectionMutex;
	std::string selectedPath;
	bool hasSelectedPath = false;
	bool showProcessFinder = false;
	std::array<char, 260> processSearch{};
	std::vector<injector::process::ProcessEntry> processes{};
};
