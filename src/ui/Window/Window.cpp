#include "Window.h"

#include "ui/InjectorView.h"

#include "imgui.h"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_sdlrenderer3.h"

namespace
{
	constexpr int TitleBarHeight = 48;
	constexpr int TitleBarControlWidth = 76;

	SDL_HitTestResult SDLCALL WindowHitTest(SDL_Window* window, const SDL_Point* area, void*)
	{
		int width = 0;
		SDL_GetWindowSize(window, &width, nullptr);
		return area->y < TitleBarHeight && area->x < width - TitleBarControlWidth
			? SDL_HITTEST_DRAGGABLE
			: SDL_HITTEST_NORMAL;
	}
}

Window::Window(AppController& appController, AppState& appState)
	: controller(appController), state(appState)
{
}

void Window::Init()
{
	SDL_Init(SDL_INIT_VIDEO);
}

void Window::Create(const unsigned int width, const unsigned int height)
{
	window = SDL_CreateWindow(title, width, height, SDL_WINDOW_BORDERLESS);
	if (window == nullptr)
		return;

	SDL_SetWindowHitTest(window, WindowHitTest, nullptr);
	renderer = SDL_CreateRenderer(window, nullptr);
	if (renderer == nullptr)
		return;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::GetIO().IniFilename = nullptr;
	view = std::make_unique<InjectorView>(window, controller, state);
	ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer3_Init(renderer);
}

void Window::Frame()
{
	while (!done && renderer != nullptr)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL3_ProcessEvent(&event);
			if (event.type == SDL_EVENT_QUIT)
				done = true;
		}

		ImGui_ImplSDLRenderer3_NewFrame();
		ImGui_ImplSDL3_NewFrame();
		ImGui::NewFrame();
		view->Render(done);
		ImGui::Render();
		SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
		SDL_RenderClear(renderer);
		ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
		SDL_RenderPresent(renderer);
	}
}

Window::~Window()
{
	view.reset();
	if (ImGui::GetCurrentContext() != nullptr)
	{
		ImGui_ImplSDLRenderer3_Shutdown();
		ImGui_ImplSDL3_Shutdown();
		ImGui::DestroyContext();
	}
	if (renderer != nullptr) SDL_DestroyRenderer(renderer);
	if (window != nullptr) SDL_DestroyWindow(window);
	SDL_Quit();
}
