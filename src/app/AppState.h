#pragma once

#include <cstddef>
#include <string>

struct AppState
{
	std::string target;
	std::string targetName;
	std::string dllPath;
	std::string status = "Ready";
	std::size_t bytesWritten = 0;
	std::size_t totalBytes = 0;
	float progress = 0.0f;
	bool busy = false;
};
