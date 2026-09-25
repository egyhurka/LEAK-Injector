#pragma once

#define SDL_MAIN_HANDLED
#include "SDL3/SDL.h"
#include <SDL3/SDL_main.h>

#include <memory>

#define TITLE "LEAK Injector"

class InjectorView;
class AppController;
struct AppState;

class Window
{
public:
	Window(AppController& controller, AppState& state);
	~Window();
	void Init();
	void Create(unsigned int width, unsigned int height);
	void Frame();

private:
	const char* title = TITLE;
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
	bool done = false;
	AppController& controller;
	AppState& state;
	std::unique_ptr<InjectorView> view;
};
