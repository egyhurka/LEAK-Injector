#include "Injector.h"
#include "Process/Process.h"
#include "File/File.h"

namespace injector
{

void Injector::LoadProcessId(const DWORD id)
{
	processId = id;
}

void Injector::LoadDll(const std::wstring& path)
{
	std::wstring p = file::CopyFileToTempDir(path);

	dllPath = p;
	dllPathSize = file::GetFilePathSize(p);
}

bool Injector::Inject(InjectionMethod method)
{
	if (processId == 0 || dllPath.empty())
		return false;

	switch (method)
	{
		case Standard_LoadLibrary:
			return StandardLoadLibraryInjection();
			break;
		case Thread_Hijacking:
		case Manual_Mapping:
			break;
	}

	return false;
}

bool Injector::StandardLoadLibraryInjection()
{
	bytesWritten = 0;

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

bool Injector::ThreadHijackingInjection()
{
	return false;
}

bool Injector::ManualMappingInjection()
{
	return false;
}

}
