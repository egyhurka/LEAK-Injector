#include "Console.h"

#include <cstdio>
#include <Windows.h>

void leak::console::Init()
{
	AllocConsole();
	FILE* file;
	freopen_s(&file, "CONOUT$", "w", stdout);
}

void leak::console::Close()
{
	fclose(stdout);
	FreeConsole();
}
