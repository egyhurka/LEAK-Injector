#pragma once

#define DLL_TEMP_DIRECTORY_NAME L"LInjectorDllTemp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <filesystem>

namespace injector::file
{
	[[nodiscard]] std::filesystem::path CreateTempDir();
	bool ClearTempDirectory();
	[[nodiscard]] std::wstring CopyFileToTempDir(const std::wstring& filePath);
	[[nodiscard]] SIZE_T GetFilePathSize(const std::wstring& filePath);
}