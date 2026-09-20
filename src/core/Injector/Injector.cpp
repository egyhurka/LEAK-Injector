#include "Injector.h"
#include "Process/Process.h"

namespace injector
{

void Injector::LoadProcessId(const DWORD id)
{
	processId = id;
}

void Injector::LoadDllPath(const std::wstring& path)
{
	dllPath = path;
	dllPathSize = process::GetRemotePathSize(dllPath);
}

bool Injector::Inject()
{
	bytesWritten = 0;
	if (processId == 0 || dllPath.empty())
		return false;

	HANDLE process = process::Open(processId);

	if (process == nullptr)
		return false;

	LPVOID remotePath = process::AllocateRemotePath(process, dllPathSize);

	if (remotePath == nullptr)
	{
		process::Close(process);
		return false;
	}

	bool succeeded = process::WriteRemotePath(process, remotePath, dllPath, &bytesWritten);
	HANDLE thread = succeeded ? process::CreateRemoteLoaderThread(process, remotePath) : nullptr;
	if (thread != nullptr)
	{
		WaitForSingleObject(thread, INFINITE);
		process::Close(thread);
	}
	else
	{
		succeeded = false;
	}

	(void)process::FreeRemotePath(process, remotePath);
	process::Close(process);
	return succeeded;
}

}
