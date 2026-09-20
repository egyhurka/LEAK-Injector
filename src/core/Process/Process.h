#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace injector::process
{
	struct ProcessEntry
	{
		std::string name;
		DWORD id;
	};

	[[nodiscard]] std::optional<DWORD> FindId(std::wstring_view nameOrId);
	[[nodiscard]] std::vector<ProcessEntry> EnumerateProcesses();
	[[nodiscard]] HANDLE Open(DWORD processId, DWORD access = NULL);
	void Close(HANDLE& handle);

	[[nodiscard]] SIZE_T GetRemotePathSize(const std::wstring& dllPath);
	[[nodiscard]] LPVOID AllocateRemotePath(HANDLE process, SIZE_T pathSize);
	[[nodiscard]] BOOL FreeRemotePath(HANDLE process, LPVOID remotePath);
	[[nodiscard]] BOOL WriteRemotePath(HANDLE process, LPVOID remotePath, const std::wstring& dllPath,
		SIZE_T* bytesWritten = nullptr);
	[[nodiscard]] HANDLE CreateRemoteLoaderThread(HANDLE process, LPVOID remotePath);
}
