#include <windows.h>

#include "ui/Window/Window.h"
#include "AppController.h"
#include "Injector/Injector.h"
#include "AppState.h"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int)
{
	AppState state;
	injector::Injector injector;
	AppController controller(state, injector);
	Window window(controller, state);
	window.Init();
	window.Create(960, 620);
	window.Frame();

	return EXIT_SUCCESS;
}
