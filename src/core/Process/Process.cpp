#include "Process.h"

#include <tlhelp32.h>
#include <utility>

namespace injector::process
{
	std::optional<DWORD> FindId(const std::wstring_view nameOrId)
	{
		if (nameOrId.empty())
			return std::nullopt;

		std::wstring target(nameOrId);
		if (!target.ends_with(L".exe"))
			target.append(L".exe");

		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (snapshot == INVALID_HANDLE_VALUE)
			return std::nullopt;

		PROCESSENTRY32W entry{};
		entry.dwSize = sizeof(entry);
		if (Process32FirstW(snapshot, &entry))
		{
			do
			{
				if (target == entry.szExeFile || nameOrId == std::to_wstring(entry.th32ProcessID))
				{
					CloseHandle(snapshot);
					return entry.th32ProcessID;
				}
			} while (Process32NextW(snapshot, &entry));
		}

		CloseHandle(snapshot);
		return std::nullopt;
	}

	std::vector<ProcessEntry> EnumerateProcesses()
	{
		std::vector<ProcessEntry> processes;
		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (snapshot == INVALID_HANDLE_VALUE)
			return processes;

		PROCESSENTRY32W entry{};
		entry.dwSize = sizeof(entry);
		if (Process32FirstW(snapshot, &entry))
		{
			do
			{
				const int length = WideCharToMultiByte(CP_UTF8, 0, entry.szExeFile, -1, nullptr, 0, nullptr, nullptr);
				std::string name(static_cast<size_t>(length > 0 ? length : 0), '\0');
				if (length > 1)
				{
					WideCharToMultiByte(CP_UTF8, 0, entry.szExeFile, -1, name.data(), length, nullptr, nullptr);
					name.resize(static_cast<size_t>(length - 1));
				}
				processes.push_back({ std::move(name), entry.th32ProcessID });
			} while (Process32NextW(snapshot, &entry));
		}

		CloseHandle(snapshot);
		return processes;
	}

	HANDLE Open(const DWORD processId, DWORD access)
	{
		if (access == NULL)
			access = PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ;
		
		return OpenProcess(access, FALSE, processId);
	}

	void Close(HANDLE& handle)
	{
		if (handle != nullptr && handle != INVALID_HANDLE_VALUE)
		{
			CloseHandle(handle);
			handle = nullptr;
		}
	}

	SIZE_T GetRemotePathSize(const std::wstring& dllPath)
	{
		return (dllPath.length() + 1) * sizeof(wchar_t);
	}

	LPVOID AllocateRemotePath(HANDLE process, const SIZE_T pathSize)
	{
		return VirtualAllocEx(process, nullptr, pathSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	}

	BOOL FreeRemotePath(HANDLE process, LPVOID remotePath)
	{
		return VirtualFreeEx(process, remotePath, 0, MEM_RELEASE);
	}

	BOOL WriteRemotePath(HANDLE process, LPVOID remotePath, const std::wstring& dllPath, SIZE_T* bytesWrittenOut)
	{
		SIZE_T bytesWritten = 0;
		const SIZE_T pathSize = GetRemotePathSize(dllPath);
		const BOOL succeeded = WriteProcessMemory(process, remotePath, dllPath.c_str(), pathSize, &bytesWritten);
		if (bytesWrittenOut != nullptr)
			*bytesWrittenOut = bytesWritten;
		return succeeded && bytesWritten == pathSize;
	}

	HANDLE CreateRemoteLoaderThread(HANDLE process, LPVOID remotePath)
	{
		const HMODULE kernel32 = GetModuleHandleW(L"kernel32.dll");
		if (kernel32 == nullptr)
			return nullptr;

		const FARPROC loadLibrary = GetProcAddress(kernel32, "LoadLibraryW");
		if (loadLibrary == nullptr)
			return nullptr;

		return CreateRemoteThread(process, nullptr, 0,
			reinterpret_cast<LPTHREAD_START_ROUTINE>(loadLibrary), remotePath, 0, nullptr);
	}
}
